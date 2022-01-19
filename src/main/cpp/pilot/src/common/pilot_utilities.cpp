#include "pilot_utilities.h"

using namespace vaultdb;

const std::string PilotUtilities::data_cube_sql_ =  "WITH labeled as (\n"
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




std::string PilotUtilities::getRollupExpectedResultsSql(const std::string &groupByColName) {
    std::string expectedResultSql = "   WITH labeled as (\n"
                               "        SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl\n"
                               "        FROM patient\n"
                               "        ORDER BY pat_id),\n"
                               "  deduplicated AS (    SELECT p.pat_id,  age_strata, sex, ethnicity, race, MAX(p.numerator::INT) numerator, COUNT(*) cnt\n"
                               "    FROM labeled p\n"
                               "    GROUP BY p.pat_id, age_strata, sex, ethnicity, race\n"
                               "    HAVING MAX(denom_excl::INT) = 0\n"
                               "    ORDER BY p.pat_id, age_strata, sex, ethnicity, race ),\n"
                               "  data_cube AS (SELECT  age_strata, sex, ethnicity, race, SUM(numerator::INT) numerator, COUNT(*) denominator,\n"
                               "                       SUM(CASE WHEN (numerator > 0 AND cnt> 1) THEN 1 ELSE 0 END) numerator_multisite, SUM(CASE WHEN (cnt > 1) THEN 1 else 0 END)  denominator_multisite\n"
                               "  FROM deduplicated\n"
                               "  GROUP BY  age_strata, sex, ethnicity, race\n"
                               "  ORDER BY  age_strata, sex, ethnicity, race )\n";
    expectedResultSql += "SELECT " + groupByColName + ", SUM(numerator)::INT numerator, SUM(denominator)::INT denominator, SUM(numerator_multisite)::INT numerator_multisite, SUM(denominator_multisite)::INT denominator_multisite \n";
    expectedResultSql += " FROM data_cube \n"
                         " GROUP BY " + groupByColName + " \n"
                                                         " ORDER BY " + groupByColName;

    return expectedResultSql;

}