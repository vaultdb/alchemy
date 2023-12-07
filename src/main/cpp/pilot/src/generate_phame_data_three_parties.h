#ifndef GENERATE_3_PARTY_PHAME_DATA_H
#define GENERATE_3_PARTY_PHAME_DATA_H

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

#define PHAME_STUDY_START_YEAR 2015
#define PHAME_STUDY_END_YEAR 2023

namespace domains {
  const vector<string> gender_  = {"M ", "F ", "NB"};
  const vector<string> ethnicity_ = {"Y ", "N ", "R ", "NI", "UN", "OT"};
  const vector<string> race_ = {"01", "02", "03", "04", "05", "06", "07", "NI", "UN", "OT"};
  const vector<string> zip_codes_ = {"60004","60077","60163","60411","60463","60558","60625","60654","60005","60078","60164","60412","60464","60601","60626","60655","60006","60082","60165","60415","60465","60602","60628","60656","60007","60089","60168","60419","60466","60603","60629","60657","60008","60090","60169","60422","60467","60604","60630","60659","60009","60091","60171","60423","60469","60605","60631","60660","60010","60093","60172","60425","60471","60606","60632","60661","60015","60094","60173","60426","60472","60607","60633","60664","60016","60095","60176","60428","60473","60608","60634","60666","60017","60103","60192","60429","60475","60609","60636","60680","60018","60104","60193","60430","60476","60610","60637","60681","60022","60107","60194","60438","60477","60611","60638","60690","60025","60120","60195","60439","60478","60612","60639","60691","60026","60126","60201","60443","60480","60613","60640","60701","60029","60130","60202","60445","60482","60614","60641","60706","60043","60131","60203","60452","60487","60615","60642","60707","60053","60133","60204","60453","60499","60616","60643","60712","60056","60141","60301","60454","60501","60617","60644","60714","60062","60153","60302","60455","60513","60618","60645","60803","60065","60154","60303","60456","60521","60619","60646","60804","60067","60155","60304","60457","60525","60620","60647","60805","60068","60159","60305","60458","60526","60621","60649","60827","60070","60160","60402","60459","60527","60622","60651","60074", "60161","60406","60461","60534","60623","60652","60076","60162","60409","60462","60546","60624","60653"};
  const vector<string> payer_codes_ = {"1%", "2%", "5%", "6%", "NI", "OT", "UN"};
  // diabetes, hypertension, breast cancer, lung cancer, colorectal cancer, cervical cancer
  const vector<string> dx_labels_ = {"DM ", "HTN", "BC ", "LC ", "CLC", "CVC"};
  const vector<string> diabetes_dx_codes_ = {"E11.00","E11.01","E11.10","E11.11","E11.21","E11.22","E11.29","E11.311","E11.319","E11.3211","E11.3212","E11.3213","E11.3219","E11.3291","E11.3292","E11.3293","E11.3299","E11.3311","E11.3312","E11.3313","E11.3319","E11.3391","E11.3392","E11.3393","E11.3399","E11.3411","E11.3412","E11.3413","E11.3419","E11.3491","E11.3492","E11.3493","E11.3499","E11.3511","E11.3512","E11.3513","E11.3519","E11.3521","E11.3522","E11.3523","E11.3529","E11.3531","E11.3532","E11.3533","E11.3539","E11.3541","E11.3542","E11.3543","E11.3549","E11.3551","E11.3552","E11.3553","E11.3559","E11.3591","E11.3592","E11.3593","E11.3599","E11.36","E11.37X1","E11.37X2","E11.37X3","E11.37X9","E11.39","E11.40","E11.41","E11.42","E11.43","E11.44","E11.49","E11.51","E11.52","E11.59","E11.610","E11.618","E11.620","E11.621","E11.622","E11.628","E11.630","E11.638","E11.641","E11.649","E11.65","E11.69","E11.8","E11.9"};
  const vector<string> htn_dx_codes_ = {"I10.", "I11.0", "I11.9", "I12.0", "I12.9", "I13.0", "I13.10", "I13.11", "I13.2", "I15.0", "I15.1", "I15.2", "I15.8", "I15.9", "I16.0", "I16.1", "I16.9", "R03.0", "R03.1", "Y93.I1"};
  const vector<string> breast_cancer_dx_codes_ = {"I10.", "I11.0", "I11.9", "I12.0", "I12.9", "I13.0", "I13.10", "I13.11", "I13.2", "I15.0", "I15.1", "I15.2", "I15.8", "I15.9", "I16.0", "I16.1", "I16.9", "R03.0", "R03.1", "Y93.I1"};
  const vector<string> lung_cancer_dx_codes_ = {"C34", "C34.1", "C34.10", "C34.11", "C34.12", "C34.2", "C34.3", "C34.30", "C34.31", "C34.32", "C34.8", "C34.80", "C34.81", "C34.82", "C34.9", "C34.90", "C34.91", "C34.92", "C46.5", "C46.50", "C46.51", "C46.52", "C78.0", "C78.00", "C78.01", "C78.02", "C7A.090", "D02.2", "D02.20", "D02.21", "D02.22", "D3A.090"};
  const vector<string> colorectal_cancer_dx_codes_ = {"C18", "C18.2", "C18.4", "C18.6", "C18.7", "C18.8", "C18.9", "C7A.022", "C7A.023", "C7A.024", "C7A.025", "D01.0", "D3A.022", "D3A.023", "D3A.024", "D3A.025"};
  const vector<string> cervical_cancer_dx_codes_ = {"C53", "C53.0", "C53.1", "C53.8", "C53.9", "R87.614", "D06", "D06.0", "D06.1", "D06.7", "D06.9"};
}

// Table B1 in the protocol
// CREATE TABLE phame_demo (
//    patid int,
//    age float, // [18, 100]
//    gender char(2),
// storing race code - ('01', '02', '03', '04', '05', '06', '07', 'NI', 'UN', 'OT') - NOTE: protocol requests long names, but these are going to be low performance
// same with primary_payer and payer_secondary - just remove the extra formatting in the code
// TODO: udate SQL input code for race encoding.
//    race char(2),
//    ethnicity char(2),
//    zip char(5),
//  payer_primary char(2),
// payer_secondary char(2),
//    site_id int -- alice = 1, bob = 2, chi = 3

struct PatientTuple {
    int patid = -1;
    float age_years;
  string gender;
  string ethnicity;
  string  race;
    string zip;
    string payer_primary;
    string payer_secondary;
   int site_id;

    string toString() const {
        return std::to_string(patid) + ","
               + std::to_string(age_years) + ","
               + gender + ","
               + ethnicity + ","
               + race + ","
               + zip + ","
               + payer_primary + ","
               + payer_secondary + ","
      + std::to_string(site_id);
    }

    static string getSchema()  {
       return "(pat_id:int32, age_years:float, gender:char(2),  ethnicity:varchar(2), race:varchar(2), zip_code:char(5), payer_primary char(2), payer_secondary char(2), site_id:int32)";
    }
};



// table B3 in protocol
// CREATE TABLE phame_diag_ins (
//   patid int,
//  dx_date date, -- date of diagnosis, in range of PHAME_STUDY_START_YEAR to PHAME_STUDY_END_YEAR, stored in a string in C++
//  dx_code char(7), -- ICD-10 code
// dx_label char(3), -- DM, HTN, BC, LC, CLC, CVC
//  payer_primary char(2), -- get these from joining with the patient tuples later
// payer_secondary char(2));


struct DxTuple {
    int patid;
    string dx_date;
    string dx_code;
    string dx_label;
    string payer_primary;
    string payer_secondary;


    string toString() const {
        return std::to_string(patid) + ","
               + dx_date + ","
               + dx_code + ","
               + dx_label + ","
               + payer_primary + ","
                + payer_secondary;
    }

    static string getSchema()  {
        return "(pat_id:int32, dx_date date, dx_code char(7), dx_label char(3), payer_primary char(2), payer_secondary char(2))";
    }

};

// JMR: I think everything else is derived from these two tables


struct TupleSet {
    PatientTuple alice_patient;
    PatientTuple bob_patient;
    PatientTuple chi_patient;

};

class GeneratePhameDataThreeParties {
public:
    static std::string getCurrentWorkingDirectory();
    // generate [min, max)
    static int generateRandomInt(int min, int max);
    static float generateRandomFloat(float min, float max);
    static PatientTuple generatePatientTuple(const int & a_patid);
    static TupleSet generateTuples(int alice_patid);
    static vector<DxTuple> generateDxTuples(const PatientTuple & patient);

    static void writeSchemaFile(const string & dst_filename, const string & schema);

private:
  // draws a random value in domain
  static std::string generateRandomValue(const std::vector<std::string> & domain);
  static std::string generateRandomDxDate();
  static std::string generateRandomDxCode(const std::string & dx_label);

};



#endif // GENERATE_3_PARTY_DATA_H

