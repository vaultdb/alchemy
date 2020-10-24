//
// Created by Jennie Rogers on 10/7/20.
//

#include <util/data_utilities.h>
#include "QueryTableTestQueries.h"

std::string QueryTableTestQueries::getInputQuery()  {
    // selecting one of each type:
    // int
    // varchar
    // fixed char
    // numeric
    // date

    return  "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
            "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
            "FROM lineitem "
            "ORDER BY l_orderkey "
            "LIMIT 5";

}

std::unique_ptr<QueryTable> QueryTableTestQueries::getExpectedSecureOutput()  {

    return DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", getInputQuery(), false);

}

std::string QueryTableTestQueries::getInputQueryDummyTag() {
        // selecting one of each type:
        // int
        // varchar
        // fixed char
        // numeric
        // date
        // dummy tag

        return "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
               "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate, "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
               "l_returnflag <> 'N' AS dummy "  // simulate a filter for l_returnflag = 'N' -- all of the ones that dont match are dummies
               "FROM lineitem "
               "ORDER BY l_orderkey "
               "LIMIT 5";


}

std::unique_ptr<QueryTable> QueryTableTestQueries::getExpectedSecureOutputDummyTag() {

        return DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", getInputQueryDummyTag(), true);


}

