#ifndef GENERATE_3_PARTY_DATA_H
#define GENERATE_3_PARTY_DATA_H

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
//    numerator int default 0, -- denotes 0 = false, 1 = true
//    denom_excl int default 0 -- denotes 0 = false, 1 = true
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
    int denom_excl; // actually bool

    string toString() const {
        return std::to_string(patid) + ","
               + zip_marker + ","
               + std::to_string(age_days) + ","
               + gender + ","
               + std::to_string(ethnicity) + ","
               + std::to_string(race) + ","
               + std::to_string(numerator);
    }
};


// for denom_excl table, each site flips a coin on this separately
struct TupleSet {
    PatientTuple alicePatient;
    PatientTuple bobPatient;
    PatientTuple chiPatient;

};


PatientTuple generatePatientTuple(const int & aPatientId) {
    PatientTuple result;
    result.patid = aPatientId;

    std::stringstream ss;
    ss << std::setw(3) << std::setfill('0') << generateRandomInt(0, 50);
    result.zip_marker =  ss.str();
    result.age_days = generateRandomInt(18*365, 100*365);
    result.gender =  generateRandomInt(0, 2) == 0 ? 'M' : 'F';
    result.ethnicity = generateRandomInt(0, 2);
    result.race = generateRandomInt(0, 6);
    result.numerator =  (generateRandomInt(0, 4) < 3) ? 0 : 1; // 25% in numerator
    result.denom_excl = generateRandomInt(0,10) < 9 ? 0 : 1; // 1-in-20 are excluded
    return result;


}




TupleSet generateTuples(int alicePatientId) {
    TupleSet output;

    output.alicePatient = generatePatientTuple(alicePatientId);

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

    }
    else {
        output.chiPatient.patid = -1;
    }

    return output;
}



#endif // GENERATE_3_PARTY_DATA_H

