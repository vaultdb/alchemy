#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

// this code is designed to be a stub - i.e., it is self-contained

using namespace std;

// CREATE TABLE patient (
//    patid int,
//    zip_marker varchar(3),
//    age_days integer,
//    sex varchar(1),
//    ethnicity bool,
//    race int,
//    numerator int default null -- denotes null = false, 1 = true
//);
//
//
//-- populate this with the following distributions for 200 tuples:
//-- patid: monotonically increasing key on Alice with Bob partially overlapping (~50% overlap in current config)
//-- zip_marker: randomly generated fixed-length string based on int.  For now constrain it to 1...50
//-- age_days: uniform draw from 0...6, then map back to age_days in range
//-- sex: random 50/50 draw
//-- ethnicity: random 50/50 draw
//-- numerator: random draw with 1/4 chance of true


// patient_excl table:
// patid and numerator as before
// denom_excl is when a relevant patient has a condition that disqualifies him or her, 1/10 chance de novo for patient.  Flip a 50/50 coin for overlapping patients to detect corner cases.

std::string getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    std::string  currentWorkingDirectory = std::string(cwd);
    std::string suffix = currentWorkingDirectory.substr(currentWorkingDirectory.length() - 4, 4);
    if(suffix == std::string("/bin")) {
        currentWorkingDirectory = currentWorkingDirectory.substr(0, currentWorkingDirectory.length() - 4);
    }

    return currentWorkingDirectory;
}

// generate [min, max)
int generateRandomInt(int min, int max) {
  return rand() % (max - min) + min;
}


struct PatientTuple {
    int patid;
    string zip_marker;
    int age_days;
    char gender;
    int ethnicity; // actually bool
    int race;
    int numerator;  // actually bool

    string toString() const {
        return std::to_string(patid) + ", "
               + zip_marker + ", "
               + std::to_string(age_days) + ", "
               + std::to_string(gender) + ", "
               + std::to_string(ethnicity) + ", "
               + std::to_string(race) + ", "
               + std::to_string(numerator);
    }
};

struct PatientExclusionTuple {
    int patid;
    int numerator; // actually bool
    int denom_excl; // actually bool

    string toString() const {
        return std::to_string(patid) + ", " + std::to_string(numerator) + ", " + std::to_string(denom_excl);
    }
};

// for denom_excl table, each site flips a coin on this separately
 struct TupleSet {
    PatientTuple alicePatient;
    PatientExclusionTuple alicePatientExcl;
    PatientTuple bobPatient;
    PatientExclusionTuple bobPatientExcl;

};

// maintain a running offset to bob's patient ID with respect to Alice's
// increments when we generate a non-overlapping patient
int bobPatientId = 0;

PatientTuple generatePatientTuple(const int & aPatientId) {
    PatientTuple result;
    result.patid = aPatientId;

    std::stringstream ss;
    ss << std::setw(3) << std::setfill('0') << generateRandomInt(0, 50);
    result.zip_marker =  ss.str();
    result.age_days = generateRandomInt(18*365, 100*365);
    result.gender =  generateRandomInt(0, 2);
    result.ethnicity = generateRandomInt(0, 2);
    result.race = generateRandomInt(0, 6);
    result.numerator =  (generateRandomInt(0, 4) < 3) ? 0 : 1; // 25% in numerator
    return result;


}

PatientExclusionTuple generatePatientExclusionTuple(const PatientTuple & srcTuple) {

    PatientExclusionTuple result;
    result.patid = srcTuple.patid;
    result.numerator = srcTuple.numerator;
    result.denom_excl = generateRandomInt(0,10) < 9 ? 0 : 1; // 1-in-20 are excluded
    return result;
}



TupleSet generateTuples(int alicePatientId) {
    TupleSet output;

    output.alicePatient = generatePatientTuple(alicePatientId);
    output.alicePatientExcl = generatePatientExclusionTuple(output.alicePatient);

    // flip a 50/50 coin to decide if Bob's tuple in overlap set
    bool bobOverlap = (rand() % 2) == 1;

    if(bobOverlap) {
        output.bobPatient = output.alicePatient;
        output.bobPatientExcl = output.alicePatientExcl;
        // re-roll the dice on bob's patient's exclusion from study, 50/50 chance to ID corner cases more easily
        bool flipExclusionBit = (rand() % 2) == 1;
        if(flipExclusionBit)
            output.bobPatientExcl.denom_excl =  abs(output.alicePatientExcl.denom_excl-1);

    }
    else {
        output.bobPatient.patid = -1;
        output.bobPatientExcl.patid = -1;

    }

    return output;
}


int main(int argc, char **argv) {
   // usage: generate_enrich_data <target dir> <tuple count>
   // target path is relative to $VAULTDB_ROOT/src/main/cpp
   // e.g., ./generate_enrich_data test/support/csv/enrich 100

   if(argc < 3) {
       std::cout << "usage: generate_enrich_data <dst path> <tuple count>" << std::endl;
       exit(-1);
   }

   string targetPath = argv[1];
   int tupleCount = atoi(argv[2]);
    srand (time(NULL));
    bobPatientId = tupleCount;

    targetPath = getCurrentWorkingDirectory() + "/" + targetPath;
    std::string alicePatientFilename = targetPath + "/alice-patient.csv";
    std::string alicePatientExclusionFilename = targetPath + "/alice-patient-exclusion.csv";
    std::string bobPatientFilename = targetPath + "/bob-patient.csv";
    std::string bobPatientExclusionFilename = targetPath + "/bob-patient-exclusion.csv";


    std::ofstream alicePatientFile, alicePatientExclusionFile, bobPatientFile, bobPatientExclusionFile;
    alicePatientFile.open(alicePatientFilename.c_str(), ios::out);
    alicePatientExclusionFile.open(alicePatientExclusionFilename.c_str(), ios::out);

    if(!alicePatientFile.is_open() || !alicePatientExclusionFile.is_open()) {
        std::cout << "Failed to open alice files: " << alicePatientFilename << ", " << alicePatientExclusionFilename << std::endl;
        exit(-1);
    }


    bobPatientFile.open(bobPatientFilename, ios::out);
    bobPatientExclusionFile.open(bobPatientExclusionFilename, ios::out);

    if(!bobPatientFile.is_open() || !bobPatientExclusionFile.is_open()) {
        std::cout << "Failed to open bob files: " << bobPatientFilename << ", " << bobPatientExclusionFilename << std::endl;
        exit(-1);
    }


   int bobTupleCount = 0;
    for(int i = 0; i < tupleCount; ++i) {
        TupleSet tupleSet = generateTuples(i);
        alicePatientFile << tupleSet.alicePatient.toString() << std::endl;
        alicePatientExclusionFile << tupleSet.alicePatientExcl.toString() << std::endl;
        if(tupleSet.bobPatient.patid != -1) { // overlapping pid
            bobPatientFile << tupleSet.bobPatient.toString() << std::endl;
            bobPatientExclusionFile << tupleSet.bobPatientExcl.toString() << std::endl;
            ++bobTupleCount;
        }
    }

    alicePatientFile.close();
    alicePatientExclusionFile.close();


    while(bobTupleCount < tupleCount) {
        PatientTuple bobPatient = generatePatientTuple(bobPatientId);
        PatientExclusionTuple bobPatientExclusion = generatePatientExclusionTuple(bobPatient);
        bobPatientFile << bobPatient.toString() << std::endl;
        bobPatientExclusionFile << bobPatientExclusion.toString() << std::endl;
        ++bobTupleCount;
        ++bobPatientId;
    }

    bobPatientFile.close();
    bobPatientExclusionFile.close();

    std::cout << "finished generating " << tupleCount << " patients per host!" << std::endl;

}