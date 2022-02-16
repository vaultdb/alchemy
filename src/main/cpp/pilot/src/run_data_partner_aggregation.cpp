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

#define TESTBED 0

auto start_time = emp::clock_start();
auto cumulative_runtime = emp::time_from(start_time);


std::string getRollupExpectedResultsSql(const std::string &groupByColName) {
    std::string expectedResultSql = "SELECT " + groupByColName + ", SUM(CASE WHEN numerator AND NOT denom_excl THEN 1 ELSE 0 END)::INT numerator_cnt, "
                                                                 "SUM(CASE WHEN ((NOT denom_excl) AND denominator) THEN 1 ELSE 0 END)::INT denominator_cnt \n";
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

    BOOST_LOG(logger) <<  "***Done " << colName << " rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime << "epoch " << Utilities::getEpoch() << endl;


    return stratified;
}

// usage: ./run_data_partner_aggregation <alice ip address> <port> <party \in (1,2)> <local db name> <year \in (2018, 2019, 2020, all)> <secret share of partial aggs from third site>
// example: ./bin/run_data_partner_aggregation 127.0.0.1 54321 2 enrich_htn_unioned_bob all pilot/secret_shares/tables/chi_counts.bob
int main(int argc, char **argv) {
    if(argc < 7) {
        cout << "usage: ./run_data_partner_aggregation <alice host> <port> <party> <local db name> <year> <secret share of partial aggs> <optional logger file prefix>" << endl;
        exit(-1);
    }

    string output_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/xor/";
    string host = string(argv[1]);
    int port = atoi(argv[2]);
    int party = atoi(argv[3]);
    assert(party == 1 || party == 2);
    string party_name = (party == 1) ? "alice" : "bob";

    string db_name = argv[4];
    string input_query_file = "pilot/queries/partial-count-no-dedupe-" + string(argv[5]) + ".sql";
    string partial_aggregate_query = DataUtilities::readTextFileToString(input_query_file);
    string secret_share_file = argv[6];
    // otherwise output to stdout
    string logger_prefix = (argc == 8) ? string(argv[7]) + "-" + party_name : "";
    size_t cardinality_bound = 441;

    Logger::setup(logger_prefix);
    auto logger = vaultdb_logger::get();




    NetIO *netio =  new emp::NetIO(party == ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(netio, party,  port);
    uint64_t epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    BOOST_LOG(logger) << "Starting epoch " << epoch << endl;

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

    double runtime = time_from(start_time);
    BOOST_LOG(logger) <<  "Test completed on " << party << " in " <<    (runtime+0.0)*1e6*1e-9 << " ms." << endl;

    epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    BOOST_LOG(logger) << "Ending epoch " << epoch << endl;

    cout << "Age rollup: " << endl;
    cout << ageRollup->reveal()->toString() << endl;

    cout << "Sex rollup: " << endl;
    cout << genderRollup->reveal()->toString() << endl;

    emp::finalize_semi_honest();
    delete netio;

}
