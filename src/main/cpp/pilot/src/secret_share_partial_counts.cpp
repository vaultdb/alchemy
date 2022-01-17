#include <data/csv_reader.h>
#include <util/data_utilities.h>

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
        cout << "usage: secret_share_partial_counts <target_db> <destination root> <optional site_id>" << endl;
        exit(-1);
    }

    setup_plain_prot(false, "");
    string db_name = argv[1];
    string dst_root = argv[2];

    string partial_count_query = "WITH single_site AS (\n"
                                 "    SELECT *\n"
                                 "    FROM patient\n"
                                 "    WHERE NOT multisite ";
    if(argc == 4) {
        int site_id = atoi(argv[3]); // verify it is an int
        assert(1 <= site_id && site_id <= 3);
        partial_count_query += ("AND site_id = " + std::string(argv[3]));
    }
        partial_count_query += "),\n"
                                 "     full_domain AS (\n"
                                 "        SELECT d.*, p.pat_id\n"
                                 "        FROM demographics_domain d LEFT JOIN single_site p on d.age_strata = p.age_strata  AND d.sex = p.sex  AND d.ethnicity = p.ethnicity AND d.race = p.race)\n"
                                 "SELECT age_strata, sex, ethnicity, race, COUNT(pat_id)\n"
                                 "FROM full_domain\n"
                                 "GROUP BY age_strata, sex, ethnicity, race\n"
                                 "ORDER BY age_strata, sex, ethnicity, race";

    std::shared_ptr<PlainTable> table = DataUtilities::getQueryResults(db_name, partial_count_query, false);

    SecretShares shares = table->generateSecretShares();

    DataUtilities::writeFile(dst_root + ".alice", shares.first);
    DataUtilities::writeFile(dst_root + ".bob", shares.second);

    finalize_plain_prot();

    std::cout << "Success." << std::endl;

}

