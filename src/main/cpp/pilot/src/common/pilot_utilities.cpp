#include "pilot_utilities.h"
#include <operators/sort.h>
#include <operators/group_by_aggregate.h>
#include <util/data_utilities.h>
#include <data/csv_reader.h>

using namespace vaultdb;

const std::string PilotUtilities::unioned_db_name_ = "enrich_htn_unioned_3pc";
const std::string PilotUtilities::data_cube_sql_no_dummies_ =  DataUtilities::readTextFileToString("pilot/queries/data-cube-no-dummies.sql");


const std::string PilotUtilities::data_cube_sql_ =  DataUtilities::readTextFileToString("pilot/queries/data-cube.sql");




std::string PilotUtilities::getRollupExpectedResultsSql(const std::string &groupByColName) {
    std::string expectedResultSql = "   WITH labeled as (\n"
                               "        SELECT study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl\n"
                               "        FROM (SELECT DISTINCT study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl, site_id FROM patient) p \n"
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

    return expectedResultSql;

}

// roll up one group-by col at a time
// input schema:
// study_year (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
std::shared_ptr<SecureTable> PilotUtilities::rollUpAggregate(const std::shared_ptr<SecureTable> & input, const int &ordinal)  {

    SortDefinition sortDefinition{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(ordinal, SortDirection::ASCENDING)};
    Sort sort(input, sortDefinition);
    shared_ptr<SecureTable> sorted = sort.run();

    std::vector<int32_t> groupByCols{0, ordinal}; // 0 for study_year
    // ordinals 0...4 are group-by cols in input schema
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };

    GroupByAggregate rollupStrata(sorted, groupByCols, aggregators );

    return rollupStrata.run();


}


void PilotUtilities::validateInputTable(const std::string & dbName, const std::string & sql, const SortDefinition  & expectedSortDefinition, const std::shared_ptr<PlainTable> & testTable)  {


    shared_ptr<PlainTable> expectedTable = DataUtilities::getQueryResults(dbName, sql, false);
    expectedTable->setSortOrder(expectedSortDefinition);

    // sort the inputs
    // ops deleted later using Operator framework
    Sort sort(testTable, expectedSortDefinition);
    shared_ptr<PlainTable> observedTable = sort.run();
    observedTable = DataUtilities::removeDummies(observedTable);


    bool res = (*expectedTable == *observedTable);
    if(!res) {
        BOOST_LOG(vaultdb_logger::get()) << "Failed to match at " << Utilities::getStackTrace() << endl;
    }
    assert(res);


}

void
PilotUtilities::secretShareFromCsv(const string &src_csv, const QuerySchema &plain_schema, const string &dst_root) {
    std::unique_ptr<PlainTable> inputTable = CsvReader::readCsv(src_csv, plain_schema);
    SecretShares shares = inputTable->generateSecretShares();

    DataUtilities::writeFile(dst_root + ".alice", shares.first);
    DataUtilities::writeFile(dst_root + ".bob", shares.second);

}

void PilotUtilities::secretShareFromQuery(const string &db_name, const string &query, const string &dst_root) {
    std::shared_ptr<PlainTable> table = DataUtilities::getQueryResults(db_name, query, false);
    SecretShares shares = table->generateSecretShares();

    BOOST_LOG(vaultdb_logger::get())  << "Secret sharing " << table->getTupleCount() << " rows."; //" tuples from query " << query << " on " << db_name << endl;

    DataUtilities::writeFile(dst_root + ".alice", shares.first);
    DataUtilities::writeFile(dst_root + ".bob", shares.second);

    string schema_filename = dst_root + ".schema";
    std::stringstream schema_str;
    schema_str << *(table->getSchema()) << endl;
    string schema_desc = schema_str.str();
    DataUtilities::writeFile(schema_filename, schema_desc);
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
