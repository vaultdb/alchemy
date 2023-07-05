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
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include <query_table/row_table.h>



using namespace std;
using namespace vaultdb;
using namespace emp;

#define TESTBED 0

auto start_time_ = emp::clock_start();
auto cumulative_runtime_ = emp::time_from(start_time_);
int min_cell_count_ = 11;


std::string getRollupExpectedResultsSql(const std::string &groupByColName) {
    std::string expectedResultSql = "SELECT study_year, " + groupByColName + ", SUM(CASE WHEN numerator AND NOT denom_excl THEN 1 ELSE 0 END)::INT numerator_cnt, "
                                                                 "SUM(CASE WHEN ((NOT denom_excl) AND denominator) THEN 1 ELSE 0 END)::INT denominator_cnt \n";
    expectedResultSql += " FROM patient \n"
                         " WHERE :selection"
                         " GROUP BY study_year, " + groupByColName + " \n"
                                                         " ORDER BY study_year, " + groupByColName;

    return expectedResultSql;

}


SecureTable *
runRollup(int idx, string colName, int party, SecureTable  *data_cube, const string & selection_clause,  const string &output_path) {
    auto start_time = emp::clock_start();

    SortDefinition sortDefinition{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(idx, SortDirection::ASCENDING)};
    Sort sort(data_cube, sortDefinition);
    SecureTable *sorted = sort.run();

    std::vector<int32_t> groupByCols{0, idx};
    // ordinals 0...4 are group-by cols in input schema
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt")
    };

    GroupByAggregate rollupStrata(sorted, groupByCols, aggregators );



    SecureTable *stratified = rollupStrata.run()->clone();


    // null out the ones with cell count below threshold
    PilotUtilities::redactCellCounts(stratified, min_cell_count_);

    std::vector<int8_t> results = stratified->reveal(emp::XOR)->serialize();

    std::string suffix = (party == emp::ALICE) ? "alice" : "bob";
    std::string output_file = output_path + "/" + colName + "." + suffix;
    DataUtilities::writeFile(output_file, results);

    // validate it against the DB for testing
    if(TESTBED) {
        SortDefinition orderBy = DataUtilities::getDefaultSortDefinition(2);

        PlainTable * revealed = stratified->reveal();
        DataUtilities::removeDummies(revealed);

        string query = getRollupExpectedResultsSql(colName);
        query = PilotUtilities::replaceSelection(query, selection_clause);
        PilotUtilities::validateTable(PilotUtilities::unioned_db_name_, query, orderBy, revealed);

        // write it out
        string csv, schema;
        string out_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/validate";
        string out_file = out_path + "/" + colName + ".csv";
        Utilities::mkdir(out_path);
        PlainTable *result = DataUtilities::getExpectedResults(PilotUtilities::unioned_db_name_, query, false, 1);

        std::stringstream schema_str;
        schema_str << result->getSchema() << std::endl;
        csv = schema_str.str();

        for(size_t i = 0; i < result->getTupleCount(); ++i) {
            csv += result->getPlainTuple(i).toString() + "\n";
        }
        DataUtilities::writeFile(out_file, csv);
        delete revealed;
        delete result;
    }


    auto delta = time_from(start_time);
    cumulative_runtime_ += delta;

    cout << "***Done " << colName << " rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime_ << "epoch " << Utilities::getEpoch() << endl;


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
    string db_name = argv[4];
    string year = argv[5];
    string year_selection = PilotUtilities::parseYearSelection(year);

    assert(party == 1 || party == 2);
    string party_name = (party == 1) ? "alice" : "bob";

    string partial_aggregate_query = DataUtilities::readTextFileToString( "pilot/queries/partial-count-no-dedupe.sql");
    partial_aggregate_query = PilotUtilities::replaceSelection(partial_aggregate_query, year_selection);
    string secret_share_file = argv[6];
    // otherwise output to stdout
    string logger_prefix = (argc == 8) ? string(argv[7]) + "-" + party_name : "";
    int study_length = PilotUtilities::getStudyLength(year);
    size_t cardinality_bound = 441*study_length;

    //Logger::setup(logger_prefix);
    //auto logger = vaultdb_logger::get();


    PilotUtilities::setupSystemConfiguration(party, host, port);

    uint64_t epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    cout<< "Starting epoch " << epoch << endl;

    SecureTable *alice, *bob, *chi;

    PlainTable *local_partial_counts = DataUtilities::getQueryResults(db_name, partial_aggregate_query, false);

    assert(local_partial_counts->getTupleCount() == cardinality_bound);
    PlainTable *empty = local_partial_counts->clone();
    empty->resize(0);

    // ship local, partial counts - alice, then bob
    if (party == 1) { // alice
        alice = local_partial_counts->secretShare();
//               alice =  RowTable<Bit>::secret_share_send_table(local_partial_counts, netio, 1);
      bob = empty->secretShare(); // secret share where we contribute no rows
      //  bob = RowTable<Bit>::secret_share_recv_table(local_partial_counts->getSchema(), SortDefinition(), netio,   2);

    } else { // bob
        alice = empty->secretShare();
//        alice = RowTable<Bit>::secret_share_recv_table(local_partial_counts->getSchema(), SortDefinition(),
//                                                     netio, 1);

            bob = local_partial_counts->secretShare();
//        bob = RowTable<Bit>::secret_share_send_table(local_partial_counts, netio, 2);
    }



    chi = UnionHybridData::readSecretSharedInput(secret_share_file, QuerySchema::toPlain(alice->getSchema()));

    assert(alice->getTupleCount() == bob->getTupleCount());
    assert(alice->getTupleCount() == chi->getTupleCount());
    assert(alice->getSchema() == bob->getSchema());
    assert(alice->getSchema() == chi->getSchema());

    SecureTable *data_cube = alice->clone();
    size_t tuple_cnt = data_cube->getTupleCount();

    // for each tuple
    for(size_t i = 0; i < tuple_cnt; ++i) {
        Bit dummy_tag = data_cube->getDummyTag(i) | bob->getDummyTag(i) | chi->getDummyTag(i);
        data_cube->setDummyTag(i, dummy_tag);

        data_cube->setField(i, 5,
                            data_cube->getField(i, 5) + bob->getField(i, 5) + chi->getField(i, 5));

        data_cube->setField(i, 6,
                            data_cube->getField(i, 6) + bob->getField(i, 6) + chi->getField(i, 6));

        //        SecureTuple dst = (*data_cube)[i];
//        SecureTuple b = (*bob)[i];
//        SecureTuple c = (*chi)[i];
//
//
//        dst.setDummyTag(dst.getDummyTag() | b.getDummyTag() | c.getDummyTag());
//        dst.setField(5, dst[5] + b[5] + c[5]);
//        dst.setField(6, dst[6] + b[6] + c[6]);

    }




    // verify data cube
    if(TESTBED) {
        PlainTable *revealed = data_cube->reveal();
        SortDefinition patientSortDef = DataUtilities::getDefaultSortDefinition(5);
        PilotUtilities::validateTable(PilotUtilities::unioned_db_name_, partial_aggregate_query, patientSortDef,
                                      revealed);
        delete revealed;
    }


    SecureTable *ageRollup = runRollup(1, "age_strata", party, data_cube, year_selection, output_path);
    SecureTable *genderRollup = runRollup(2, "sex", party, data_cube, year_selection, output_path);
    SecureTable *ethnicityRollup = runRollup(3, "ethnicity", party, data_cube, year_selection, output_path);
    SecureTable *raceRollup = runRollup(4, "race", party, data_cube, year_selection, output_path);

    double runtime = time_from(start_time_);
    cout <<  "Test completed on " << party << " in " <<    (runtime+0.0)*1e6*1e-9 << " ms." << endl;

    epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    cout  << "Ending epoch " << epoch << endl;
/*
    BOOST_LOG(logger) << "Age rollup: " << endl;
    BOOST_LOG(logger) << ageRollup->reveal()->toString() << endl;

    BOOST_LOG(logger) << "Sex rollup: " << endl;
    BOOST_LOG(logger) << genderRollup->reveal()->toString() << endl;


    BOOST_LOG(logger) << "Ethnicity rollup: " << endl;
    BOOST_LOG(logger) << ethnicityRollup->reveal()->toString() << endl;

    BOOST_LOG(logger) << "Race rollup: " << endl;
    BOOST_LOG(logger) << raceRollup->reveal()->toString() << endl;

*/

    delete ageRollup;
    delete raceRollup;
    delete genderRollup;
    delete ethnicityRollup;

}
