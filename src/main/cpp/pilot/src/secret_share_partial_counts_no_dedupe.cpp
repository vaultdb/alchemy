#include <pilot/src/common/shared_schema.h>
#include <pilot/src/common/pilot_utilities.h>

using namespace vaultdb;
using namespace std;

// input generated with:
// ./bin/generate_enrich_data_three_parties pilot/secret_shares/input/ 100
int main(int argc, char **argv) {
    // usage: secret_share_partial_counts <src file> <destination root>
    // paths are relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/secret_share_csv pilot/test/input/chi-patient.csv pilot/test/output/chi-patient
    // writes to pilot/test/output/chi-patient.alice pilot/secret_shares/output/chi-patient.bob

    if (argc < 3) {
        cout << "usage: secret_share_partial_counts_no_dedupe <target_db> <destination root>" << endl;
        exit(-1);
    }

    setup_plain_prot(false, "");
    string db_name = argv[1];
    string dst_root = argv[2];

    // output schema:
    // age_strata (0), sex (1), ethnicity (2) , race (3), numerator_cnt (4), denominator_cnt (5)
    string partial_count_query =  "WITH aggs AS (SELECT age_strata, sex, ethnicity, race, SUM(CASE WHEN numerator AND NOT denom_excl THEN 1 ELSE 0 END)::INT numerator_cnt, "
                                                                  "SUM(CASE WHEN NOT denom_excl THEN 1 ELSE 0 END)::INT denominator_cnt \n"
                                                                  " FROM patient \n"
                                                                  " GROUP BY age_strata, sex, ethnicity, race) \n"
                                                                  "SELECT d.*, COALESCE(numerator_cnt, 0) numerator_cnt, COALESCE(denominator_cnt, 0) denominator_cnt \n"
                                                                  "FROM demographics_domain d LEFT JOIN aggs a on d.age_strata = a.age_strata  AND d.sex = a.sex  AND d.ethnicity = a.ethnicity AND d.race = a.race "
                                                                  "ORDER BY age_strata, sex, ethnicity, race ";


    PilotUtilities::secretShareFromQuery(db_name, partial_count_query, dst_root);

    finalize_plain_prot();

    std::cout << "Successfully generated partial counts  to " << dst_root << "(.alice|.bob)" << std::endl;

}

