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
#include <vector>

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
//    site_id int // alice = 1, bob = 2, chi = 3
//);
//
// new schema:
// //pat_id (0), study_year (1), zip_marker (2), age_days (3), sex (4), ethnicity (5), race (6), numerator (7), denom (8), denom_excl (9), site_id (10)
// changes:
// * add study_year (int)
// * sex --> varchar(2)
// * ethnicity --> varchar(2)
// * race --> varchar(2)
// * numerator --> bool
// * denominator --> bool (added, might be unneeded)
// * denom_excl --> bool

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


namespace domains {
  const std::vector<std::string> gender_  = {"M", "F", "UN"};
  const std::vector<std::string> ethnicity_ = {"Y", "N", "OT", "NI", "R"};
  const std::vector<std::string> race_ = {"01", "02", "03", "04", "05", "06", "07", "NI", "OT", "UN"};
  const std::vector<std::string> zip_marker_ = {"000", "600", "601", "602", "606", "607", "608"};
}

struct PatientTuple {
    int patid;
    int study_year;
    string zip_marker; // varchar(3)
    int age_days;
  string gender; // varchar(2)
  string ethnicity; //varchar(2)
  string  race; // varchar(2)
   bool numerator;  
   bool denom_excl; 
   int site_id;

    string toString() const {
        return std::to_string(patid) + ","
      	       + std::to_string(study_year) + ","
	       + zip_marker + ","
               + std::to_string(age_days) + ","
               + gender + ","
               + ethnicity + ","
               + race + ","
               + std::to_string(numerator) + ","
	       + std::to_string(!denom_excl) + ","
               + std::to_string(denom_excl) + ","
               + std::to_string(site_id);
    }
};


// for denom_excl table, each site flips a coin on this separately
struct TupleSet {
    PatientTuple alicePatient;
    PatientTuple bobPatient;
    PatientTuple chiPatient;

};

class GenerateEnrichDataThreeParties {
public:
    static std::string getCurrentWorkingDirectory();
    static int generateRandomInt(int min, int max);
    static PatientTuple generatePatientTuple(const int & aPatientId);
    static TupleSet generateTuples(int alicePatientId);

private:
  // draws a random value in domain
  static std::string generateRandomValue(const std::vector<std::string> & domain);


  
};



#endif // GENERATE_3_PARTY_DATA_H

