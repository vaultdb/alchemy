#include "generate_enrich_data_three_parties.h"
#include <cassert>

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif


std::string GenerateEnrichDataThreeParties::getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    char * ret = getcwd(cwd, sizeof(cwd));
    ++ret; // to address warnings

    std::string  currentWorkingDirectory = std::string(cwd);
    std::string suffix = currentWorkingDirectory.substr(currentWorkingDirectory.length() - 4, 4);
    if(suffix == std::string("/bin")) {
        currentWorkingDirectory = currentWorkingDirectory.substr(0, currentWorkingDirectory.length() - 4);
    }

    return currentWorkingDirectory;
}

// generate [min, max)
int GenerateEnrichDataThreeParties::generateRandomInt(int min, int max)  {
    return rand() % (max - min) + min;
}






PatientTuple GenerateEnrichDataThreeParties::generatePatientTuple(const int & aPatientId)  {
    PatientTuple result;
    result.patid = aPatientId;
    result.study_year = generateRandomInt(2018, 2021);
    result.age_strata = generateRandomInt(1, 8);
    result.gender =  generateRandomValue(domains::gender_);  
    result.ethnicity = generateRandomValue(domains::ethnicity_);
    result.race = generateRandomValue(domains::race_);
    result.numerator =  (generateRandomInt(0, 4) < 3) ? 0 : 1; // 25% in numerator
    result.denominator =  (generateRandomInt(0, 10) < 9) ? 1 : 0; // 90% in denominator

    result.denom_excl = generateRandomInt(0,10) < 9 ? 0 : 1; // 1-in-10 are excluded
    result.site_id = generateRandomInt(0, 3);
    return result;


}




TupleSet GenerateEnrichDataThreeParties::generateTuples(int alicePatientId)  {
    TupleSet output;

    output.alicePatient = generatePatientTuple(alicePatientId);
    output.alicePatient.site_id = 1;

    // flip a 50/50 coin to decide if Bob's tuple in overlap set
    bool bobOverlap = (rand() % 2) == 1;

    // flip a 1/3 coin to decide if Chi's tuple in overlap set

    bool chiOverlap = (rand() % 3) == 1;


    if(bobOverlap) {
        output.bobPatient = output.alicePatient;
        // re-roll the dice on bob's patient's exclusion from study, 50/50 chance to ID corner cases more easily
        bool flipExclusionBit = (rand() % 2) == 1;
        if(flipExclusionBit)
            output.bobPatient.denom_excl =  abs(output.alicePatient.denom_excl-1);
        output.bobPatient.site_id = 2;


    }
    else {
        output.bobPatient.patid = -1;

    }

    if(chiOverlap) {
        output.chiPatient = output.alicePatient;

        // re-roll the dice on chi's patient's exclusion from study, 1/3rd chance to ID corner cases
        bool flipExclusionBit = (rand() % 3) == 1;
        if(flipExclusionBit)
            output.chiPatient.denom_excl =  abs(output.alicePatient.denom_excl-1);
        output.chiPatient.site_id = 3;

    }
    else {
        output.chiPatient.patid = -1;
    }

    return output;
}


int main(int argc, char **argv) {
    // usage: generate_enrich_data <target dir> <tuple count>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g.,  ./bin/generate_enrich_data_three_parties pilot/test/input/ 100

    // each party provides <tuple count> tuples
    if(argc < 3) {
        std::cout << "usage: generate_enrich_data_three_parties <dst path> <tuple count>" << std::endl;
        exit(-1);
    }


    string dst_path = argv[1];
    int tuple_cnt = atoi(argv[2]);
    srand (time(nullptr));
    // maintain a running offset to bob's patient ID with respect to Alice's
    // increments when we generate a non-overlapping patient
    int bob_pat_id = tuple_cnt;
    // do the same thing for chiPatient
    int chi_pat_id = 0;
    string schema_str = "(pat_id:int32, study_year:int32, age_strata:varchar(1), sex:varchar(1),  ethnicity:varchar(1), race:varchar(1), numerator:bool, denominator:bool, denom_excl:bool, site_id:bool)";


    dst_path = GenerateEnrichDataThreeParties::getCurrentWorkingDirectory() + "/" + dst_path;
    std::string alice_pat_filename = dst_path + "/alice-patient.csv";
    std::string bob_pat_filename = dst_path + "/bob-patient.csv";
    std::string chi_pat_filename = dst_path + "/chi-patient.csv";




    std::ofstream alice_pat_file, bob_pat_file, chi_pat_file;
    alice_pat_file.open(alice_pat_filename.c_str(), ios::out);

    if(!alice_pat_file.is_open() ) {
        std::cout << "Failed to open alice files: " << alice_pat_filename << std::endl;
        exit(-1);
    }
    else {
        std::ofstream schema_file;
        schema_file.open(dst_path + "/alice-patient.schema", ios::out);
        schema_file << schema_str << std::endl;
        schema_file.close();
    }


    bob_pat_file.open(bob_pat_filename, ios::out);

    if(!bob_pat_file.is_open()) {
        std::cout << "Failed to open bob files: " << bob_pat_filename << std::endl;
        exit(-1);
    }
    else {
        std::ofstream schema_file;
        schema_file.open(dst_path + "/bob-patient.schema", ios::out);
        schema_file << schema_str << std::endl;
        schema_file.close();
    }


    chi_pat_file.open(chi_pat_filename, ios::out);

    if(!chi_pat_file.is_open()) {
        std::cout << "Failed to open chi files: " << chi_pat_filename << std::endl;
        exit(-1);
    }
    else {
        std::ofstream schema_file;
        schema_file.open(dst_path + "/chi-patient.schema", ios::out);
        schema_file << schema_str << std::endl;
        schema_file.close();
    }


    int bob_tuple_cnt = 0;
    int chi_tuple_cnt = 0;

    // end setup, generate Alice tuples
    for(int i = 0; i < tuple_cnt; ++i) {
        TupleSet tupleSet = GenerateEnrichDataThreeParties::generateTuples(i);
        alice_pat_file << tupleSet.alicePatient.toString() << std::endl;
        if(tupleSet.bobPatient.patid != -1) { // overlapping pid
            assert(tupleSet.bobPatient.site_id == 2);
            bob_pat_file << tupleSet.bobPatient.toString() << std::endl;
            ++bob_tuple_cnt;
        }
        if(tupleSet.chiPatient.patid != -1) { // overlapping pid
            assert(tupleSet.chiPatient.site_id  == 3);
            chi_pat_file << tupleSet.chiPatient.toString() << std::endl;
            ++chi_tuple_cnt;
        }
    }

    alice_pat_file.close();


    while(bob_tuple_cnt < tuple_cnt) {
        TupleSet tupleSet = GenerateEnrichDataThreeParties::generateTuples(bob_pat_id);

        PatientTuple bob_pat = tupleSet.alicePatient; // reusing earlier logic to guarantee we get a tuple, Alice is correct here.
        bob_pat.site_id = 2;
        bob_pat_file << bob_pat.toString() << std::endl;

        if(tupleSet.chiPatient.patid != -1) { // overlapping pid
            assert(tupleSet.chiPatient.site_id  == 3);
            chi_pat_file << tupleSet.chiPatient.toString() << std::endl;
            ++chi_tuple_cnt;
        }

        ++bob_tuple_cnt;
        ++bob_pat_id;
    }

    bob_pat_file.close();

    chi_pat_id = bob_pat_id;

    while(chi_tuple_cnt < tuple_cnt) {
        PatientTuple chiPatient = GenerateEnrichDataThreeParties::generatePatientTuple(chi_pat_id);
        chiPatient.site_id = 3;
        chi_pat_file << chiPatient.toString() << std::endl;
        ++chi_tuple_cnt;
        ++chi_pat_id;
    }

    chi_pat_file.close();
    std::cout << "Finished generating " << tuple_cnt << " patients per host!" << std::endl;

}


std::string GenerateEnrichDataThreeParties::generateRandomValue(const std::vector<std::string>  & domain) {
  int draw = generateRandomInt(0, domain.size());
  return domain[draw];
			       
}
