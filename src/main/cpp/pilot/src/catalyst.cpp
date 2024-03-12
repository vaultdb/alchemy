#include "catalyst.h"
#include "expression/generic_expression.h"
#include "util/emp_manager/sh2pc_manager.h"
#include "util/system_configuration.h"
#include "common/union_hybrid_data.h"
#include "operators/project.h"
#include "expression/expression_node.h"

using namespace catalyst;

Catalyst::Catalyst(int party, const std::string json_config_filename)  {
    StudyParser study_parser(json_config_filename);
    study_ = study_parser.study_;
    assert(study_parser.protocol_ == "sh2pc");
    auto emp_manager = new SH2PCManager(study_parser.alice_host_, party, study_parser.port_);
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.emp_mode_ = EmpMode::SH2PC;
    s.emp_manager_ = emp_manager;
    BitPackingMetadata md; // empty set
    s.initialize("", md, StorageModel::COLUMN_STORE);

}

void Catalyst::loadStudyData() {
    // load secret shares into TableManager
    for(auto &input_table : study_.input_tables_) {
        for(auto &contributor : input_table.data_contributors_) {
            importSecretShares(input_table.name_, contributor);
        }
    }
    data_loaded_ = true;

}

void Catalyst::runQueries() {
    // run queries
    // write out results to secret share files
    assert(data_loaded_);
    string dst_dir = study_.dst_path_;
    if(dst_dir[0] != '/') {
        dst_dir = Utilities::getCurrentWorkingDirectory() + "/" + dst_dir;
    }
    Utilities::mkdir(dst_dir);

    SystemConfiguration & s = SystemConfiguration::getInstance();

    for(auto &query : study_.queries_) {
        string query_name = query.first;
        Operator<Bit> *root = query.second;
        cout << "Running query with execution plan: \n" << root->toString() << endl;
        QueryTable<Bit> *output = root->run();
        string fq_filename = dst_dir + "/" + query_name + (s.party_ == 1 ? ".alice" : ".bob");
        std::vector<int8_t> results = output->reveal(emp::XOR)->serialize();
        DataUtilities::writeFile(fq_filename, results);
    }

    }


void Catalyst::importSecretShares(const string & table_name, const int & src_party) {
    // read secret shares from file
    // import into table_manager
    string fq_table_file = study_.secret_shares_root_ + "/" + std::to_string(src_party) + "/" + table_name;
    SystemConfiguration & s = SystemConfiguration::getInstance();
    string suffix = (s.party_ == 1) ? "alice" : "bob";
    string secret_shares_file = fq_table_file + "." + suffix;
    QuerySchema schema = QuerySchema::toPlain(table_manager_.getSchema(table_name));
    // temporarily drop the site_id and project it in from src_party
    bool drop_site_id = false;
    // JMR: this is a hack to support the format from CAPriCORN queries, not for long-term use
    if(table_name == "phame_diagnosis" || table_name == "phame_demographic") {
        schema.dropField("site_id");
        drop_site_id = true;
    }
    SecureTable *secret_shares = UnionHybridData::readSecretSharedInput(secret_shares_file, schema);

    if(drop_site_id) {
        ExpressionMapBuilder<Bit> builder(schema);
        int field_cnt = schema.getFieldCount();
        for (int i = 0; i < field_cnt; ++i) {
            builder.addMapping(i, i);
        }

        LiteralNode<Bit> site_id(Field<Bit>(FieldType::SECURE_INT, Integer(32, src_party)));
        Expression<Bit> *site_id_expr = new GenericExpression<Bit>(&site_id, "site_id", FieldType::SECURE_INT);

        builder.addExpression(site_id_expr, field_cnt);

        Project<Bit> projection(secret_shares, builder.getExprs());
        secret_shares = projection.run();
        // need to call insertTable within this scope for the project output to be available for TableManager copy
        table_manager_.insertTable(table_name, secret_shares);
        return;
    }

    table_manager_.insertTable(table_name, secret_shares);
}

int main(int argc, char **argv) {
    // usage: ./bin/catalyst <party> <json config file>
    // e.g.,
    // Alice: ./bin/catalyst 1 pilot/study/phame/study.json
    // Bob: ./bin/catalyst 2 pilot/study/phame/study.json
    if(argc < 3) {
        std::cout << "usage: ./bin/catalyst <party> <json config file>" << std::endl;
        exit(-1);
    }

    // Alice = 1, Bob = 2
    int party = atoi(argv[1]);
    assert(party == 1 || party == 2);

    Catalyst catalyst(party, argv[2]);
    catalyst.loadStudyData();
    catalyst.runQueries();

}