#include <pilot/src/common/shared_schema.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <util/data_utilities.h>
#include <operators/sort.h>
#include "common/union_hybrid_data.h"
#include "enrich_htn_query.h"
#include <util/utilities.h>
#include <util/logger.h>
#include <data/csv_reader.h>


using namespace std;
using namespace vaultdb;
using namespace emp;

#define TESTBED 1

string partial_aggregate_query =  "WITH labeled as (\n"
                                 "        SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl\n"
                                 "        FROM patient\n"
                                 "        ORDER BY pat_id),\n"
                                 "  deduplicated AS (    SELECT p.pat_id,  age_strata, sex, ethnicity, race, MAX(p.numerator::INT) numerator, COUNT(*) cnt\n"
                                 "    FROM labeled p\n"
                                 "    GROUP BY p.pat_id, age_strata, sex, ethnicity, race\n"
                                 "    HAVING MAX(denom_excl::INT) = 0\n"
                                 "    ORDER BY p.pat_id, age_strata, sex, ethnicity, race ), \n"
                                 "  data_cube AS (SELECT  age_strata, sex, ethnicity, race, SUM(numerator)::INT numerator_cnt, COUNT(*)::INT denominator_cnt,\n"
                                 "                       SUM(CASE WHEN (numerator > 0 AND cnt> 1) THEN 1 ELSE 0 END)::INT numerator_multisite_cnt, SUM(CASE WHEN (cnt > 1) THEN 1 else 0 END)::INT  denominator_multisite_cnt\n"
                                 "  FROM deduplicated\n"
                                 "  GROUP BY  age_strata, sex, ethnicity, race\n"
                                 "  ORDER BY  age_strata, sex, ethnicity, race)\n"
                                 "SELECT d.*, COALESCE(numerator_cnt, 0) numerator_cnt, COALESCE(denominator_cnt, 0) denominator_cnt, COALESCE(numerator_multisite_cnt, 0) numerator_multisite_cnt, COALESCE(denominator_multisite_cnt, 0) denominator_multisite_cnt "
                                 "FROM demographics_domain d LEFT JOIN data_cube p on d.age_strata = p.age_strata  AND d.sex = p.sex  AND d.ethnicity = p.ethnicity AND d.race = p.race "
                                 "ORDER BY d.age_strata, d.sex, d.ethnicity, d.race";



shared_ptr<SecureTable>
runRollup(int idx, string colName, int party, EnrichHtnQuery &enrich, const string &output_path) {
    auto start_time = emp::clock_start();
    auto logger = vaultdb_logger::get();


    shared_ptr<SecureTable> stratified = enrich.rollUpAggregate(idx);
    std::vector<int8_t> results = stratified->reveal(emp::XOR)->serialize();

    std::string suffix = (party == emp::ALICE) ? "alice" : "bob";
    std::string output_file = output_path + "/" + colName + "." + suffix;
    DataUtilities::writeFile(output_file, results);

    // validate it against the DB for testing
    if(TESTBED) {
        SortDefinition orderBy = DataUtilities::getDefaultSortDefinition(1);

        shared_ptr<PlainTable> revealed = stratified->reveal();
        revealed = DataUtilities::removeDummies(revealed);

        string query = getRollupExpectedResultsSql(colName);
        validateInputTable(unioned_db_name, query, orderBy, revealed);

        // write it out
        string csv, schema;
        string out_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/validate";
        string out_file = out_path + "/" + colName + ".csv";
        Utilities::mkdir(out_path);
        shared_ptr<PlainTable> result = DataUtilities::getExpectedResults(unioned_db_name, query, false, 1);

        std::stringstream schema_str;
        schema_str << *result->getSchema() << std::endl;
        csv = schema_str.str();

        for(size_t i = 0; i < result->getTupleCount(); ++i) {
            csv += (*result)[i].toString() + "\n";
        }
        DataUtilities::writeFile(out_file, csv);


    }


    auto delta = time_from(start_time);
    cumulative_runtime += delta;

    Utilities::checkMemoryUtilization(colName);
    BOOST_LOG(logger) <<  "***Done " << colName << " rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;


    return stratified;
}

// usage: ./run_data_partner_aggregation <alice ip address> <port> <party \in (1,2)> <local db name> <secret share of partial aggs from third site>
int main(int argc, char **argv) {
    if(argc < 6) {
        cout << "usage: ./run_data_partner_aggregation <alice host> <port> <party> <local db name> <secret share of partial aggs>" << endl;
        exit(-1);
    }

    string output_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/xor/";
    string host = string(argv[1]);
    int port = atoi(argv[2]);
    int party = atoi(argv[3]);
    assert(party == 1 || party == 2);
    string db_name = argv[4];
    string secret_share_file = argv[5];

    NetIO *netio =  new emp::NetIO(party == ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(netio, party,  port);

    shared_ptr<SecureTable> alice, bob, chi;

    shared_ptr<PlainTable> local_partial_counts =  DataUtilities::getQueryResults(db_name, partial_aggregate_query, false);
    assert(local_partial_counts->getTupleCount() == cardinality_bound);

    // ship local, partial counts - alice, then bob
    if (party == 1) { // alice
        alice = SecureTable::secret_share_send_table(local_partial_counts, netio, 1);
        bob = SecureTable::secret_share_recv_table(*local_partial_counts->getSchema(), SortDefinition(), netio,
                                                   2);
    } else { // bob
        alice = SecureTable::secret_share_recv_table(*local_partial_counts->getSchema(), SortDefinition(),
                                                     netio, 1);
        bob = SecureTable::secret_share_send_table(local_partial_counts, netio, 2);
    }


    chi = UnionHybridData::readSecretSharedInput(secret_share_file, SharedSchema::getPartialCountSchema(), party);
    assert(alice->getTupleCount() == bob->getTupleCount() == chi->getTupleCount());
    assert(*(alice->getSchema()) == *(bob->getSchema()) == *(chi->getSchema()));

    shared_ptr<SecureTable> output(new SecureTable(*alice));
    size_t tuple_cnt = output->getTupleCount();

    // for each tuple
    for(size_t i = 0; i < tuple_cnt; ++i) {
        SecureTuple dst = (*output)[i];
        SecureTuple b = (*bob)[i];
        SecureTuple c = (*chi)[i];


        dst.setDummyTag(dst.getDummyTag() | b.getDummyTag() | c.getDummyTag());
        dst.setField(4, dst[4] + b[4] + c[4]);
        dst.setField(5, dst[5] + b[5] + c[5]);
        dst.setField(6, dst[6] + b[6] + c[6]);
        dst.setField(7, dst[7] + b[7] + c[7]);

    }








}
