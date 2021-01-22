#include "generate_enrich_data_three_parties.h"


int main(int argc, char **argv) {
    // usage: generate_enrich_data <target dir> <tuple count>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g.,  ./bin/generate_enrich_data_three_parties pilot/secret_shares/input/ 100

    // each party provides <tuple count> tuples
    if(argc < 3) {
        std::cout << "usage: generate_enrich_data_three_parties <dst path> <tuple count>" << std::endl;
        exit(-1);
    }


    string targetPath = argv[1];
    int tupleCount = atoi(argv[2]);
    srand (time(NULL));
    // maintain a running offset to bob's patient ID with respect to Alice's
    // increments when we generate a non-overlapping patient
    int bobPatientId = tupleCount;
    // do the same thing for chiPatient
    int chiPatientId = 0;

    targetPath = getCurrentWorkingDirectory() + "/" + targetPath;
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
        TupleSet tupleSet = generateTuples(i);
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
        TupleSet tupleSet = generateTuples(bobPatientId);

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
        PatientTuple chiPatient = generatePatientTuple(chiPatientId);
        chiPatientFile << chiPatient.toString() << std::endl;
        ++chiTupleCount;
        ++chiPatientId;
    }

    chiPatientFile.close();
    std::cout << "finished generating " << tupleCount << " patients per host!" << std::endl;

}
