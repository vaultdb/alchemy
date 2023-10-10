#include "pilot_utilities.h"
#include <operators/sort.h>
#include <operators/sort_merge_aggregate.h>
#include <util/data_utilities.h>
#include <data/csv_reader.h>
#include "util/emp_manager/sh2pc_manager.h"
#include "operators/filter.h"

using namespace vaultdb;

const std::string PilotUtilities::unioned_db_name_ = "enrich_htn_unioned_3pc";
const std::string PilotUtilities::data_cube_sql_no_dummies_ =  DataUtilities::readTextFileToString("pilot/queries/data-cube-no-dummies.sql");


const std::string PilotUtilities::data_cube_sql_ =  DataUtilities::readTextFileToString("pilot/queries/data-cube.sql");




std::string PilotUtilities::getRollupExpectedResultsSql(const std::string &groupByColName, const std::string & selection) {
    std::string expectedResultSql = "   WITH labeled as (\n"
                               "        SELECT study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl\n"
                               "        FROM (SELECT DISTINCT study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl, site_id, multisite FROM patient) p \n"
                               "        WHERE :selection\n"
                               "        ORDER BY pat_id),\n"
                               "  deduplicated AS (    SELECT study_year, p.pat_id,  age_strata, sex, ethnicity, race, MAX(p.numerator::INT) numerator, MAX(p.denominator::INT) denominator, COUNT(*) cnt\n"
                               "    FROM labeled p\n"
                               "    GROUP BY study_year, p.pat_id, age_strata, sex, ethnicity, race\n"
                               "    HAVING MAX(denom_excl::INT) = 0\n"
                               "    ORDER BY study_year, p.pat_id, age_strata, sex, ethnicity, race ),\n"
                               "  data_cube AS (SELECT  study_year, age_strata, sex, ethnicity, race, SUM(numerator) numerator, SUM(denominator) denominator,\n"
                               "                       SUM(CASE WHEN (numerator > 0 AND cnt> 1) THEN 1 ELSE 0 END) numerator_multisite, SUM(CASE WHEN (denominator > 0 AND cnt > 1) THEN 1 else 0 END)  denominator_multisite\n"
                               "  FROM deduplicated\n"
                               "  GROUP BY  study_year, age_strata, sex, ethnicity, race\n"
                               "  ORDER BY  study_year, age_strata, sex, ethnicity, race )\n";
    expectedResultSql += "SELECT study_year, " + groupByColName + ", SUM(numerator)::INT numerator, SUM(denominator)::INT denominator, SUM(numerator_multisite)::INT numerator_multisite, SUM(denominator_multisite)::INT denominator_multisite \n";
    expectedResultSql += " FROM data_cube \n"
                         " GROUP BY study_year, " + groupByColName + " \n"
                                                         " ORDER BY study_year, " + groupByColName;

    expectedResultSql = PilotUtilities::replaceSelection(expectedResultSql, selection);
    return expectedResultSql;

}

// roll up one group-by col at a time
// input schema:
// study_year (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
SecureTable *PilotUtilities::rollUpAggregate(SecureTable *input, const int &ordinal)  {

    SortDefinition sortDefinition{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(ordinal, SortDirection::ASCENDING)};
    auto sort = new Sort(input->clone(), sortDefinition);


    std::vector<int32_t> groupByCols{0, ordinal}; // 0 for study_year
    // ordinals 0...4 are group-by cols in input schema
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };

    SortMergeAggregate rollupStrata(sort, groupByCols, aggregators, SortDefinition() );

    return rollupStrata.run()->clone();


}


void PilotUtilities::validateTable(const std::string & db_name, const std::string & sql, const SortDefinition  & expected_sort, PlainTable *test_table)  {


    PlainTable *expected = DataUtilities::getQueryResults(db_name, sql, false);
    expected->setSortOrder(expected_sort);

    // sort the inputs
    // ops deleted later using Operator framework
    Sort sort(test_table->clone(), expected_sort);
    PlainTable *observed = sort.run();
    DataUtilities::removeDummies(observed);

    bool res = (*expected == *observed);
    if(!res) {
        cout << "Failed to match at " << Utilities::getStackTrace() << endl;
    }
    assert(res);

    delete expected;

}

void PilotUtilities::secretShareFromCsv(const string &src_csv, const QuerySchema &plain_schema, const string &dst_root) {
    PlainTable *inputTable = CsvReader::readCsv(src_csv, plain_schema);
    SecretShares shares = inputTable->generateSecretShares();
    QuerySchema dst_schema = QuerySchema::toSecure(plain_schema);

    DataUtilities::writeFile(dst_root + ".alice", shares.first);
    DataUtilities::writeFile(dst_root + ".bob", shares.second);
    DataUtilities::writeFile(dst_root + ".schema",  dst_schema.prettyPrint());
    delete inputTable;

}

void PilotUtilities::secretShareFromQuery(const string &db_name, const string &query, const string &dst_root) {
    PlainTable *table = DataUtilities::getQueryResults(db_name, query, false);
    SecretShares shares = table->generateSecretShares();

    DataUtilities::writeFile(dst_root + ".alice", shares.first);
    DataUtilities::writeFile(dst_root + ".bob", shares.second);

    string schema_filename = dst_root + ".schema";
    DataUtilities::writeFile(schema_filename, table->getSchema().prettyPrint());

    delete table;
}


string PilotUtilities::appendToConjunctivePredicate(const string & base, const string & to_append) {
    return base.empty() ? " " + to_append + " "
        : base + " AND " + to_append + " ";

}


// replaces all instance of to_find
std::string PilotUtilities::replaceSubstring(const std::string & base, const std::string & to_find, const std::string replacement){
    size_t to_replace = base.find(to_find);
    assert(to_replace != string::npos);
    string dupe = base;


    while(to_replace != string::npos) {
        dupe = dupe.replace(to_replace, to_find.length(), replacement);
        to_replace = dupe.find(to_find);
    }

    return dupe;

}

std::string PilotUtilities::replaceSelection(const string &base, const std::string replacement) {
    string to_insert = (replacement.empty()) ? "1=1" : replacement;
    return replaceSubstring(base, ":selection", to_insert);
}


 std::pair<std::string, std::string>  PilotUtilities::parseRange(const std::string & range) {
     size_t dash_pos = range.find('-');
     assert(dash_pos != string::npos);

     string l = range.substr(0, dash_pos);
     string u = range.substr(dash_pos+1, range.length() - (dash_pos +1 ) );

     return pair<string, string>(l, u);
}

std::string PilotUtilities::getRangePredicate(const string &range) {
    pair<string, string> bounds = parseRange(range);
    return "study_year >= " + bounds.first + " AND study_year <= " + bounds.second;
}

std::string PilotUtilities::parseYearSelection(const std::string & study_year) {

    if(study_year != "all" && (study_year.find('-') == string::npos)) { // single year
        return "study_year = " + study_year;
    }
    else if(study_year.find('-') != string::npos) {
       return  PilotUtilities::getRangePredicate(study_year);
    }
    else {
        assert(study_year == "all");
    }
    return ""; // empty selection

}


// study length in years
 int PilotUtilities::getStudyLength(const std::string study_year) {
    if(study_year == "all") return 3;

    if(study_year.find('-') != string::npos) {
        pair<string, string> bounds = PilotUtilities::parseRange(study_year);
        int lhs = std::atoi(bounds.first.c_str());
        int rhs = std::atoi(bounds.second.c_str());
        return rhs - lhs + 1;
    }
       bool is_digit =  !study_year.empty() && std::find_if(study_year.begin(),
                                                            study_year.end(), [](unsigned char c) { return !std::isdigit(c); }) == study_year.end();
       assert(is_digit);
       return 1; // 1 years

}

void PilotUtilities::redactCellCounts(SecureTable *input, const int &min_cell_cnt) {
    emp::Integer cutoff(32, min_cell_cnt);
    SecureField cutoff_field(FieldType::SECURE_INT, cutoff);

    // null out the ones with cell count below threshold
    for(int i = 0; i < input->getTupleCount(); ++i) {
        // numerator_cnt and denom_cnt
        Bit b = (input->getField(i, 2) < cutoff_field) | (input->getField(i, 3) < cutoff_field);
        Bit dummy_tag = (input->getDummyTag(i) | b);
        input->setDummyTag(i, dummy_tag);

    }

}

void PilotUtilities::setupSystemConfiguration(int & party, string & host, int & port) {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    conf.emp_mode_ = EmpMode::SH2PC;
    conf.setStorageModel(StorageModel::ROW_STORE);
    SH2PCManager *manager = new SH2PCManager(party == ALICE ? "" : host, party, port);
    conf.emp_manager_ = manager;
}

void PilotUtilities::setupSystemConfiguration() {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    conf.emp_mode_ = EmpMode::SH2PC;
    conf.setStorageModel(StorageModel::ROW_STORE);

    SH2PCManager *manager = new SH2PCManager();
    conf.emp_manager_ = manager;
}

// Schema:
QueryTable<Bit> *PilotUtilities::filterRollup(QueryTable<Bit> * input) {
    InputReference<Bit> field_2(2, input->getSchema());
    InputReference<Bit> field_3(3, input->getSchema());
    InputReference<Bit> field_4(4, input->getSchema());
    InputReference<Bit> field_5(5, input->getSchema());

    Integer zero(32, 0, PUBLIC);
    LiteralNode<Bit> zero_2(Field<Bit>(FieldType::SECURE_INT, zero));
    LiteralNode<Bit> zero_3(Field<Bit>(FieldType::SECURE_INT, zero));
    LiteralNode<Bit> zero_4(Field<Bit>(FieldType::SECURE_INT, zero));
    LiteralNode<Bit> zero_5(Field<Bit>(FieldType::SECURE_INT, zero));

    NotEqualNode<Bit> neq_2(&field_2, &zero_2);
    NotEqualNode<Bit> neq_3(&field_3, &zero_3);
    NotEqualNode<Bit> neq_4(&field_4, &zero_4);
    NotEqualNode<Bit> neq_5(&field_5, &zero_5);

    OrNode<Bit> or_0(&neq_2, &neq_3);
    OrNode<Bit> or_1(&neq_4, &neq_5);
    OrNode<Bit> or_2(&or_0, &or_1);

    GenericExpression<Bit> *expression = new GenericExpression<Bit>(&or_2, "predicate", FieldType::SECURE_BOOL);
    Filter<Bit> filter(input, expression);

    return filter.run()->clone();
}
