#include <pilot/src/common/shared_schema.h>
#include <pilot/src/common/pilot_utilities.h>
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

auto start_time = emp::clock_start();
auto cumulative_runtime = emp::time_from(start_time);

string partial_aggregate_query = "WITH cohort AS (SELECT  pat_id, min(age_strata) age_strata, sex, ethnicity, race, numerator, denom_excl\n"
                                 "                FROM patient\n"
                                 "                GROUP BY pat_id, sex, ethnicity, race, numerator, denom_excl\n"
                                 "                ORDER BY pat_id),\n"
                                 "     aggs AS (SELECT age_strata, sex, ethnicity, race, SUM(CASE WHEN numerator AND NOT denom_excl THEN 1 ELSE 0 END)::INT numerator_cnt,\n"
                                 "                                                       SUM(CASE WHEN NOT denom_excl THEN 1 ELSE 0 END)::INT denominator_cnt\n"
                                 "                                  FROM cohort\n"
                                 "                                  GROUP BY age_strata, sex, ethnicity, race)\n"
                                 "SELECT d.*, COALESCE(numerator_cnt, 0) numerator_cnt, COALESCE(denominator_cnt, 0) denominator_cnt\n"
                                 "FROM demographics_domain d LEFT JOIN aggs a on d.age_strata = a.age_strata  AND d.sex = a.sex  AND d.ethnicity = a.ethnicity AND d.race = a.race\n"
                                 "ORDER BY age_strata, sex, ethnicity, race";




std::string getRollupExpectedResultsSql(const std::string &groupByColName) {
    std::string expectedResultSql = "SELECT " + groupByColName + ", SUM(CASE WHEN numerator AND NOT denom_excl THEN 1 ELSE 0 END)::INT numerator_cnt, "
                                                                 "SUM(CASE WHEN NOT denom_excl THEN 1 ELSE 0 END)::INT denominator_cnt \n";
    expectedResultSql += " FROM patient \n"
                         " GROUP BY " + groupByColName + " \n"
                                                         " ORDER BY " + groupByColName;

    return expectedResultSql;

}


shared_ptr<SecureTable>
runRollup(int idx, string colName, int party, std::shared_ptr<SecureTable> & data_cube, const string &output_path) {
    auto start_time = emp::clock_start();
    auto logger = vaultdb_logger::get();

    SortDefinition sortDefinition{ColumnSort(idx, SortDirection::ASCENDING)};
    Sort sort(data_cube, sortDefinition);
    shared_ptr<SecureTable> sorted = sort.run();

    std::vector<int32_t> groupByCols{idx};
    // ordinals 0...4 are group-by cols in input schema
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(4, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(5, AggregateId::SUM, "denominator_cnt")
    };

    GroupByAggregate rollupStrata(sorted, groupByCols, aggregators );



    shared_ptr<SecureTable> stratified =  rollupStrata.run();


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
        PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, query, orderBy, revealed);

        // write it out
        string csv, schema;
        string out_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/validate";
        string out_file = out_path + "/" + colName + ".csv";
        Utilities::mkdir(out_path);
        shared_ptr<PlainTable> result = DataUtilities::getExpectedResults(PilotUtilities::unioned_db_name_, query, false, 1);

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
    size_t cardinality_bound = 441;

    Logger::setup();

    //Logger::setup("pilot-" + party_name);
    auto logger = vaultdb_logger::get();


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


    chi = UnionHybridData::readSecretSharedInput(secret_share_file, QuerySchema::toPlain(*(alice->getSchema())), party);
    assert(alice->getTupleCount() == bob->getTupleCount());
    assert(alice->getTupleCount() == chi->getTupleCount());
    assert(*(alice->getSchema()) == *(bob->getSchema()));
    assert(*(alice->getSchema()) == *(chi->getSchema()));

    shared_ptr<SecureTable> data_cube(new SecureTable(*alice));
    size_t tuple_cnt = data_cube->getTupleCount();

    // for each tuple
    for(size_t i = 0; i < tuple_cnt; ++i) {
        SecureTuple dst = (*data_cube)[i];
        SecureTuple b = (*bob)[i];
        SecureTuple c = (*chi)[i];


        dst.setDummyTag(dst.getDummyTag() | b.getDummyTag() | c.getDummyTag());
        dst.setField(4, dst[4] + b[4] + c[4]);
        dst.setField(5, dst[5] + b[5] + c[5]);
    }


    // verify data cube
    if(TESTBED) {
        std::shared_ptr<PlainTable> revealed = data_cube->reveal();
        SortDefinition patientSortDef = DataUtilities::getDefaultSortDefinition(4);
        PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, partial_aggregate_query, patientSortDef, revealed);

    }


    shared_ptr<SecureTable> ageRollup = runRollup(0, "age_strata", party, data_cube, output_path);
    shared_ptr<SecureTable> genderRollup = runRollup(1, "sex", party, data_cube, output_path);
    shared_ptr<SecureTable> ethnicityRollup = runRollup(2, "ethnicity", party, data_cube, output_path);
    shared_ptr<SecureTable> raceRollup = runRollup(3, "race", party, data_cube, output_path);

    emp::finalize_semi_honest();

    delete netio;
    double runtime = time_from(start_time);
    BOOST_LOG(logger) <<  "Test completed on " << party << " in " <<    (runtime+0.0)*1e6*1e-9 << " ms." << endl;





}
