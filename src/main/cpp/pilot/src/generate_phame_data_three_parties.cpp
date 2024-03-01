#include "generate_phame_data_three_parties.h"
#include <cassert>

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif


std::string GeneratePhameDataThreeParties::getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    char * ret = getcwd(cwd, sizeof(cwd));
    ++ret; // to address warnings

    std::string  current_working_directory = std::string(cwd);
    std::string suffix = current_working_directory.substr(current_working_directory.length() - 4, 4);
    if(suffix == std::string("/bin")) {
        current_working_directory = current_working_directory.substr(0, current_working_directory.length() - 4);
    }

    return current_working_directory;
}

// generate [min, max)
int GeneratePhameDataThreeParties::generateRandomInt(int min, int max)  {
    return rand() % (max - min) + min;
}




PatientTuple GeneratePhameDataThreeParties::generatePatientTuple(const int & a_patid)  {
    PatientTuple result;
    result.patid = a_patid;
    result.age_years = generateRandomFloat(18.0, 101.0);
    result.gender =  generateRandomValue(domains::gender_);
    result.ethnicity = generateRandomValue(domains::ethnicity_);
    result.race = generateRandomValue(domains::race_);
    result.zip = generateRandomValue(domains::zip_codes_);
    result.payer_secondary = result.payer_primary = generateRandomValue(domains::payer_codes_);
    // make the two payer codes different
    while(result.payer_primary == result.payer_secondary) {
        result.payer_secondary = generateRandomValue(domains::payer_codes_);
    }
    result.site_id = generateRandomInt(0, 3);
    return result;


}




TupleSet GeneratePhameDataThreeParties::generateTuples(int alice_patid)  {
    TupleSet output;

    output.alice_patient = generatePatientTuple(alice_patid);
    output.alice_patient.site_id = 1;

    // 5% chance it is an overlapping row
    if(rand() % 20 == 1) {
        // 50/50 chance of being bob or chi
        bool bob = (rand() % 2) == 1;
        bool chi = !bob;
        if(bob) {
            output.bob_patient = output.alice_patient;
            output.bob_patient.site_id = 2;
            if((rand() % 2) == 1)  // flip a coin to decide if they appear in chi as well
                chi = true;
        }

        if(chi) {
            output.chi_patient = output.alice_patient;
            output.chi_patient.site_id = 3;
        }

    }
    return output;
}


int main(int argc, char **argv) {
    // usage: generate_phame_data_three_parties <target dir> <tuple count>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g.,  ./bin/generate_phame_data_three_parties pilot/test/input/phame 500

    // each party provides <tuple count> tuples
    if(argc < 3) {
        std::cout << "usage: generate_phame_data_three_parties <dst path> <tuple count per party>" << std::endl;
        exit(-1);
    }


    string dst_path = argv[1];
    int tuple_cnt = atoi(argv[2]);
    srand (time(nullptr));
    // maintain a running offset to bob's patient ID with respect to Alice's
    // increments when we generate a non-overlapping patient
    int bob_pat_id = tuple_cnt;
    // do the same thing for chi_patient
    int chi_pat_id = 0;


    dst_path = GeneratePhameDataThreeParties::getCurrentWorkingDirectory() + "/" + dst_path;
    std::string alice_pat_filename = dst_path + "/alice-patient.csv";
    std::string bob_pat_filename = dst_path + "/bob-patient.csv";
    std::string chi_pat_filename = dst_path + "/chi-patient.csv";

    std::string alice_dx_filename = dst_path + "/alice-dx.csv";
    std::string bob_dx_filename = dst_path + "/bob-dx.csv";
    std::string chi_dx_filename = dst_path + "/chi-dx.csv";


    std::ofstream alice_pat_file, bob_pat_file, chi_pat_file;
    std::ofstream alice_dx_file, bob_dx_file, chi_dx_file;

    alice_pat_file.open(alice_pat_filename, ios::out);
    bob_pat_file.open(bob_pat_filename, ios::out);
    chi_pat_file.open(chi_pat_filename, ios::out);

    alice_dx_file.open(alice_dx_filename, ios::out);
    bob_dx_file.open(bob_dx_filename, ios::out);
    chi_dx_file.open(chi_dx_filename, ios::out);

    // not bothering to check rest since errors are usually at the directory level
    if(!alice_pat_file.is_open() ) {
        std::cout << "Failed to open file: " << alice_pat_filename << std::endl;
        exit(-1);
    }

    GeneratePhameDataThreeParties::writeSchemaFile(dst_path + "/alice-patient.schema", PatientTuple::getSchema());
    GeneratePhameDataThreeParties::writeSchemaFile(dst_path + "/bob-patient.schema", PatientTuple::getSchema());
    GeneratePhameDataThreeParties::writeSchemaFile(dst_path + "/chi-patient.schema", PatientTuple::getSchema());

    GeneratePhameDataThreeParties::writeSchemaFile(dst_path + "/alice-dx.schema", DxTuple::getSchema());
    GeneratePhameDataThreeParties::writeSchemaFile(dst_path + "/bob-dx.schema", DxTuple::getSchema());
    GeneratePhameDataThreeParties::writeSchemaFile(dst_path + "/chi-dx.schema", DxTuple::getSchema());


    int bob_tuple_cnt = 0;
    int chi_tuple_cnt = 0;

    // end setup, generate Alice tuples
    for(int i = 0; i < tuple_cnt; ++i) {
        TupleSet tupleSet = GeneratePhameDataThreeParties::generateTuples(i);
        alice_pat_file << tupleSet.alice_patient.toString() << std::endl;
        if(tupleSet.bob_patient.patid != -1) { // overlapping pid
            assert(tupleSet.bob_patient.site_id == 2);
            bob_pat_file << tupleSet.bob_patient.toString() << std::endl;
            ++bob_tuple_cnt;
        }
        if(tupleSet.chi_patient.patid != -1) { // overlapping pid
            assert(tupleSet.chi_patient.site_id == 3);
            chi_pat_file << tupleSet.chi_patient.toString() << std::endl;
            ++chi_tuple_cnt;
        }

        auto dx_tuples = GeneratePhameDataThreeParties::generateDxTuples(tupleSet.alice_patient);
        // for overlapping ones, just store diagnoses in Alice
        // dupes unlikely to matter because we take the min diagnosis date over all later and this does not use the site ID
        for(auto dx_tuple : dx_tuples) {
            alice_dx_file << dx_tuple.toString() << std::endl;
        }
    }

    alice_pat_file.close();
    alice_dx_file.close();


    while(bob_tuple_cnt < tuple_cnt) {
        TupleSet tupleSet = GeneratePhameDataThreeParties::generateTuples(bob_pat_id);

        PatientTuple bob_pat = tupleSet.alice_patient; // reusing earlier logic to guarantee we get a tuple, Alice is correct here.
        bob_pat.site_id = 2;
        bob_pat_file << bob_pat.toString() << std::endl;

        if(tupleSet.chi_patient.patid != -1) { // overlapping pid
            assert(tupleSet.chi_patient.site_id == 3);
            chi_pat_file << tupleSet.chi_patient.toString() << std::endl;
            ++chi_tuple_cnt;
        }

        auto dx_tuples = GeneratePhameDataThreeParties::generateDxTuples(bob_pat);
        // for overlapping ones, just store diagnoses in Bob
        // dupes unlikely to matter because we take the min diagnosis date over all later and this does not use the site ID
        for(auto dx_tuple : dx_tuples) {
            bob_dx_file << dx_tuple.toString() << std::endl;
        }

        ++bob_tuple_cnt;
        ++bob_pat_id;
    }

    bob_pat_file.close();
    bob_dx_file.close();

    chi_pat_id = bob_pat_id;

    while(chi_tuple_cnt < tuple_cnt) {
        PatientTuple chi_pat = GeneratePhameDataThreeParties::generatePatientTuple(chi_pat_id);
        chi_pat.site_id = 3;
        chi_pat_file << chi_pat.toString() << std::endl;

        auto dx_tuples = GeneratePhameDataThreeParties::generateDxTuples(chi_pat);
        // for overlapping ones, just store diagnoses in Bob
        // dupes unlikely to matter because we take the min diagnosis date over all later and this does not use the site ID
        for(auto dx_tuple : dx_tuples) {
            chi_dx_file << dx_tuple.toString() << std::endl;
        }

        ++chi_tuple_cnt;
        ++chi_pat_id;
    }

    chi_pat_file.close();
    chi_dx_file.close();
    std::cout << "Finished generating " << tuple_cnt << " patients per host!" << std::endl;

}


std::string GeneratePhameDataThreeParties::generateRandomValue(const std::vector<std::string>  & domain) {
  int draw = generateRandomInt(0, domain.size());
  return domain[draw];
			       
}

float GeneratePhameDataThreeParties::generateRandomFloat(float min, float max) {
    return min +  drand48() * (max - min); /* [0, 1.0] */

}

vector<DxTuple> GeneratePhameDataThreeParties::generateDxTuples(const PatientTuple & patient) {
    // first decide how many diagnoses we have
    // ranging from 1.. 10 to stress test how we handle dupes and picking first date of diagnosis
    int row_cnt = generateRandomInt(1, 11);
    vector<DxTuple> output;
    // for each diagnosis row, generate date
    for(int i = 0; i < row_cnt; ++i) {
        DxTuple dx_tuple;
        dx_tuple.patid = patient.patid;
        dx_tuple.dx_date = generateRandomDxDate();
        dx_tuple.dx_label = generateRandomValue(domains::dx_labels_);
        dx_tuple.dx_code = generateRandomDxCode(dx_tuple.dx_label);

        dx_tuple.payer_primary = patient.payer_primary;
        dx_tuple.payer_secondary = patient.payer_secondary;
        output.push_back(dx_tuple);
    }

    return output;
}

// based on: https://stackoverflow.com/questions/20271163/generating-a-random-date
std::string GeneratePhameDataThreeParties::generateRandomDxDate() {
    int study_years = PHAME_STUDY_END_YEAR - PHAME_STUDY_START_YEAR;
    int year = (rand()%study_years)+1;
    int month = (rand()%12)+1;
    int day = (rand()%30)+1;

    return std::to_string(year) + "/" + std::to_string(month) + "/" + std::to_string(day);
}

// DM, HTN, BC, LC, CLC, CVC
std::string GeneratePhameDataThreeParties::generateRandomDxCode(const string &dx_label) {
    if(dx_label == "DM ")
        return generateRandomValue(domains::diabetes_dx_codes_);
    if(dx_label == "HTN")
        return generateRandomValue(domains::htn_dx_codes_);
    if(dx_label == "BC ")
        return generateRandomValue(domains::breast_cancer_dx_codes_);
    if(dx_label == "LC ")
        return generateRandomValue(domains::lung_cancer_dx_codes_);
    if(dx_label == "CLC")
        return generateRandomValue(domains::colorectal_cancer_dx_codes_);
    if(dx_label == "CVC")
        return generateRandomValue(domains::cervical_cancer_dx_codes_);

    throw "Invalid dx label: " + dx_label;

}

void GeneratePhameDataThreeParties::writeSchemaFile(const string &dst_filename, const string &schema) {
    std::ofstream schema_file;
    schema_file.open(dst_filename, ios::out);
    schema_file << schema << std::endl;
    schema_file.close();
}

