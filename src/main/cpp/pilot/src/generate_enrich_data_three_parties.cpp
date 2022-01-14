#include "generate_enrich_data_three_parties.h"

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
    result.denom_excl = generateRandomInt(0,10) < 9 ? 0 : 1; // 1-in-20 are excluded
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


    string targetPath = argv[1];
    int tupleCount = atoi(argv[2]);
    srand (time(nullptr));
    // maintain a running offset to bob's patient ID with respect to Alice's
    // increments when we generate a non-overlapping patient
    int bobPatientId = tupleCount;
    // do the same thing for chiPatient
    int chiPatientId = 0;

    targetPath = GenerateEnrichDataThreeParties::getCurrentWorkingDirectory() + "/" + targetPath;
    std::string alicePatientFilename = targetPath + "/alice-patient.csv";
    std::string bobPatientFilename = targetPath + "/bob-patient.csv";
    std::string chiPatientFilename = targetPath + "/chi-patient.csv";


    std::ofstream alicePatientFile, bobPatientFile, chiPatientFile;
    alicePatientFile.open(alicePatientFilename.c_str(), ios::out);

    if(!alicePatientFile.is_open() ) {
        std::cout << "Failed to open alice files: " << alicePatientFilename  << std::endl;
        exit(-1);
    }


    bobPatientFile.open(bobPatientFilename, ios::out);

    if(!bobPatientFile.is_open()) {
        std::cout << "Failed to open bob files: " << bobPatientFilename << std::endl;
        exit(-1);
    }


    chiPatientFile.open(chiPatientFilename, ios::out);

    if(!bobPatientFile.is_open()) {
        std::cout << "Failed to open chi files: " << chiPatientFilename << std::endl;
        exit(-1);
    }


    int bobTupleCount = 0;
    int chiTupleCount = 0;

    // end setup, generate Alice tuples
    for(int i = 0; i < tupleCount; ++i) {
        TupleSet tupleSet = GenerateEnrichDataThreeParties::generateTuples(i);
        alicePatientFile << tupleSet.alicePatient.toString() << std::endl;
        if(tupleSet.bobPatient.patid != -1) { // overlapping pid
            bobPatientFile << tupleSet.bobPatient.toString() << std::endl;
            ++bobTupleCount;
        }
        if(tupleSet.chiPatient.patid != -1) { // overlapping pid
            chiPatientFile << tupleSet.chiPatient.toString() << std::endl;
            ++chiTupleCount;
        }
    }

    alicePatientFile.close();


    while(bobTupleCount < tupleCount) {
        TupleSet tupleSet = GenerateEnrichDataThreeParties::generateTuples(bobPatientId);

        PatientTuple bobPatient = tupleSet.alicePatient; // reusing earlier logic to guarantee we get a tuple, Alice is correct here.
        bobPatientFile << bobPatient.toString() << std::endl;

        if(tupleSet.chiPatient.patid != -1) { // overlapping pid
            chiPatientFile << tupleSet.chiPatient.toString() << std::endl;
            ++chiTupleCount;
        }

        ++bobTupleCount;
        ++bobPatientId;
    }

    bobPatientFile.close();

    chiPatientId = bobPatientId;

    while(chiTupleCount < tupleCount) {
        PatientTuple chiPatient = GenerateEnrichDataThreeParties::generatePatientTuple(chiPatientId);
        chiPatient.site_id = 3;
        chiPatientFile << chiPatient.toString() << std::endl;
        ++chiTupleCount;
        ++chiPatientId;
    }

    chiPatientFile.close();
    std::cout << "Finished generating " << tupleCount << " patients per host!" << std::endl;

}


std::string GenerateEnrichDataThreeParties::generateRandomValue(const std::vector<std::string>  & domain) {
  int draw = generateRandomInt(0, domain.size());
  return domain[draw];
			       
}
