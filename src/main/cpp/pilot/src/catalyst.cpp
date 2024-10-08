#include "catalyst.h"
#include "expression/generic_expression.h"
#include "util/emp_manager/sh2pc_manager.h"
#include "util/system_configuration.h"
#include "common/union_hybrid_data.h"
#include "operators/project.h"
#include "expression/expression_node.h"
#include "data/csv_reader.h"
#include "parser/plan_parser.h"

#define TESTBED 0
#define EXPECTED_RESULTS_PATH "pilot/study/phame/expected"

using namespace catalyst;

Catalyst::Catalyst(int party, const std::string json_config_filename)  {
    cout << "Parsing study configuration from " << json_config_filename << endl;
    StudyParser parser(json_config_filename);
    cout << "Done parsing study configuration." << endl;

    assert(parser.protocol_ == "sh2pc");
    auto emp_manager = new SH2PCManager(parser.alice_host_, party, parser.port_);
    study_ = parser.study_;
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.emp_mode_ = EmpMode::SH2PC;
    s.emp_manager_ = emp_manager;
    BitPackingMetadata md; // empty set
    s.initialize("", md, StorageModel::COLUMN_STORE);
    cout << "Parsed study: "  << parser.study_.toString() << endl;

}

void Catalyst::loadStudyData() {
    // load secret shares into TableManager
    for(auto &input_table : study_.input_tables_) {
        for(auto &contributor : input_table.data_contributors_) {
            importSecretShares(input_table.name_, contributor);
            cout << "Loaded secret shares for " << input_table.name_ << " from party " << contributor << endl;
        }
    }
    data_loaded_ = true;
}

void Catalyst::parseAndRunQueries() {
    // parse and run queries
    // write out results to secret share files
    assert(data_loaded_);
    string dst_dir = study_.dst_path_;
    SystemConfiguration & c = SystemConfiguration::getInstance();

    for(auto q : study_.queries_) {
        QuerySpec s = q.second;
        cout << "Parsing query plan in " << s.plan_file_ << endl;
        auto root = PlanParser<Bit>::parse("", s.plan_file_);
        string dst_table_name = s.name_ + "_res";

        cout << "Running " << s.name_ << " with execution plan: \n" << root->printTree() << endl;
            QueryTable<Bit> *output = root->run();
            TableManager::getInstance().insertTable(dst_table_name, output);

            string fq_filename = dst_dir + "/" + s.name_ + (c.party_ == 1 ? ".alice" : ".bob");

            if (!TESTBED) {
                // redact small cell counts
                for (auto &col: s.count_cols_) {
                    redactCellCounts(output, col, study_.min_cell_cnt_);
                }
            }

            std::vector<int8_t> results = output->reveal(emp::XOR)->serialize();
            cout << "Writing output of " << s.name_ << " to " << fq_filename << endl;
            DataUtilities::writeFile(fq_filename, results);
        }

}


void Catalyst::importSecretShares(const string & table_name, const int & src_party) {
    // read secret shares from file
    // import into table_manager
    string fq_table_file = study_.secret_shares_path_ + "/" + std::to_string(src_party) + "/" + table_name;
    cout << "Ingesting table: " << fq_table_file << endl;
    Utilities::checkMemoryUtilization(true);
    SystemConfiguration & s = SystemConfiguration::getInstance();
    TableManager & table_manager = TableManager::getInstance();

    string suffix = (s.party_ == 1) ? "alice" : "bob";
    string secret_shares_file = fq_table_file + "." + suffix;
    QuerySchema schema = QuerySchema::toPlain(table_manager.getSchema(table_name));

    // this is a hack for PHAME workflow
    bool drop_site_id = false;
    if(table_name == "phame_diagnosis" || table_name == "phame_demographic") {
        drop_site_id = true;
        schema.dropField("site_id");
    }

    // add site_id using src_party
    // this schema should NOT have site_id
    SecureTable *secret_shares = UnionHybridData::readSecretSharedInput(secret_shares_file, schema);
    cout << "Read " << secret_shares->tuple_cnt_ << " tuples from " << secret_shares_file << endl;
    cout << "Memory footprint of secret shares: " << secret_shares->tuple_cnt_ * schema.size() * sizeof(emp::Bit) << " bytes" << endl;

    // add site_id using src_party
    if(drop_site_id) {

        QueryFieldDesc site_id_desc(schema.getFieldCount(), "site_id", "",  FieldType::SECURE_INT);
        secret_shares->appendColumn(site_id_desc);
        auto dst = Field<Bit>(FieldType::SECURE_INT, Integer(32, src_party));
        for(int i = 0; i < secret_shares->tuple_cnt_; ++i) {
            secret_shares->setField(i, site_id_desc.getOrdinal(), dst);
        }

//        ExpressionMapBuilder<Bit> builder(schema);
//        int field_cnt = schema.getFieldCount();
//        for (int i = 0; i < field_cnt; ++i) {
//            builder.addMapping(i, i);
//        }
//
//        LiteralNode<Bit> site_id(Field<Bit>(FieldType::SECURE_INT, Integer(32, src_party)));
//        Expression<Bit> *site_id_expr = new GenericExpression<Bit>(&site_id, "site_id", FieldType::SECURE_INT);
//
//        builder.addExpression(site_id_expr, field_cnt);
//
//        Project<Bit> *projection = new Project<Bit>(secret_shares, builder.getExprs());
//        secret_shares = projection->run();
//        delete projection; // freeing input before we insertTable to reduce memory usage

        // need to call insertTable within this scope for the project output to be available for TableManager copy
        table_manager.insertTable(table_name, secret_shares);
        delete secret_shares;
        return;
    }

    table_manager.insertTable(table_name, secret_shares);
    delete secret_shares;
}

int main(int argc, char **argv) {
    // usage: ./bin/catalyst <party> <json config file>
    // e.g.,
    // Alice: ./bin/catalyst 1 pilot/study/phame/study.json
    // Bob: ./bin/catalyst 2 pilot/study/phame/study.json

    // for testing depends on generating synthetic data.  Example call for this:
    // bash pilot/test/generate-and-load-phame-test-data.sh 4 100
    if(argc < 3) {
        std::cout << "usage: ./bin/catalyst <party> <json config file>" << std::endl;
        exit(-1);
    }

    // Alice = 1, Bob = 2
    int party = atoi(argv[1]);
    assert(party == 1 || party == 2);

    Catalyst catalyst(party, argv[2]);
    catalyst.loadStudyData();
    catalyst.parseAndRunQueries();


    if(TESTBED) {
        CatalystStudy<Bit> study = catalyst.getStudy();
        for(auto &query : study.queries_) {
                string query_name = query.first;
                string dst_table_name = query_name + "_res";
                SecureTable *output = TableManager::getInstance().getSecureTable(dst_table_name);
                PlainTable *revealed = output->reveal();

                string expected_results_file = string(EXPECTED_RESULTS_PATH) + "/" + query_name + ".csv";
                // the test generator (pilot/test/generate-and-load-phame-test-data.sh) will write out the results to a file
                // we generate expected results in pilot/test/phame/load-generated-data.sql
                cout << "Revealing and verifying results of " << query_name << " against " << expected_results_file << endl;
                PlainTable *expected = CsvReader::readCsv(expected_results_file, revealed->getSchema());
                expected->order_by_ = revealed->order_by_;

                assert(*revealed == *expected);
        }

    }

    cout << "Finished MPC evaluation of " << catalyst.getStudy().study_name_ << " study." << endl;

}