#ifndef GENERATE_N_PARTY_PHAME_DATA_H
#define GENERATE_N_PARTY_PHAME_DATA_H

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
#define DX_THRESHOLD 10 // expected likelihood of a patient having a given condition, generated for each patient, each condition, scaled to [0, 100) to avoid expensive floating point operations
#define DX_RATE 75 // expected likelihood of a patient being in the numerator, i.e., that they have an entry in the diagnosis table, scaled to [0, 100)


namespace domains {
  const vector<char> age_cat_ = {'0', '1', '2', '3', '4', '5', '6'};
  const vector<char> gender_  = {'M', 'F', 'N', '0'};
  const vector<char> ethnicity_ = {'0', '1', '2', '3', '4', '5', '6'};
  const vector<char> race_ = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  const vector<string> zip_codes_ = {"60004","60077","60163","60411","60463","60558","60625","60654","60005","60078","60164","60412","60464","60601","60626","60655","60006","60082","60165","60415","60465","60602","60628","60656","60007","60089","60168","60419","60466","60603","60629","60657","60008","60090","60169","60422","60467","60604","60630","60659","60009","60091","60171","60423","60469","60605","60631","60660","60010","60093","60172","60425","60471","60606","60632","60661","60015","60094","60173","60426","60472","60607","60633","60664","60016","60095","60176","60428","60473","60608","60634","60666","60017","60103","60192","60429","60475","60609","60636","60680","60018","60104","60193","60430","60476","60610","60637","60681","60022","60107","60194","60438","60477","60611","60638","60690","60025","60120","60195","60439","60478","60612","60639","60691","60026","60126","60201","60443","60480","60613","60640","60701","60029","60130","60202","60445","60482","60614","60641","60706","60043","60131","60203","60452","60487","60615","60642","60707","60053","60133","60204","60453","60499","60616","60643","60712","60056","60141","60301","60454","60501","60617","60644","60714","60062","60153","60302","60455","60513","60618","60645","60803","60065","60154","60303","60456","60521","60619","60646","60804","60067","60155","60304","60457","60525","60620","60647","60805","60068","60159","60305","60458","60526","60621","60649","60827","60070","60160","60402","60459","60527","60622","60651","60074", "60161","60406","60461","60534","60623","60652","60076","60162","60409","60462","60546","60624","60653"};
  const vector<char> payer_codes_ = {'0', '1', '2', '3', '4', '5', '6'};

  // diabetes, hypertension, breast cancer, lung cancer, colorectal cancer, cervical cancer
  // Flip a weighted coin for each condition to set this bool for each patient
  const vector<string> dx_labels_ = {"DM ", "HTN", "BC ", "LC ", "CLC", "CVC"};
}


// Table B1 in the protocol

struct PatientTuple {
    // (patid, site_id) is composite primary key
    int patid = -1;
    char age_cat;
  char gender;
  char ethnicity;
  char  race;
    std::string zip;
    char payer_primary;
    char payer_secondary;
   int site_id;

    string toString() const {
        std::stringstream s;
        s << patid << "," << age_cat << "," << gender << "," << ethnicity << "," << race << "," << zip << "," << payer_primary << "," << payer_secondary << "," << site_id;
        return s.str();
    }

    static string getSchema()  {
       return "(pat_id:int32, age_cat:char(1), gender:char(2),  ethnicity:char(1), race:char(1), zip_code:char(5), payer_primary char(1), payer_secondary char(1), site_id:int32)";
    }
};



// diabetes, hypertension, breast cancer, lung cancer, colorectal cancer, cervical cancer
struct DxTuple {
    int patid = -1;
    bool dx_diabetes;
    bool dx_hypertension;
    bool dx_breast_cancer;
    bool dx_lung_cancer;
    bool dx_colorectal_cancer;
    bool dx_cervical_cancer;
    int site_id = -1;

    string toString() const {
        std::stringstream s;
        s << patid << "," << dx_diabetes << "," << dx_hypertension << "," << dx_breast_cancer << "," << dx_lung_cancer << "," << dx_colorectal_cancer << "," << dx_cervical_cancer
<< ","        << site_id;
        return s.str();
    }

    static string getSchema()  {
        return "(patid:int32, dx_diabetes:bool, dx_hypertension:bool, dx_cervical_cancer:bool, dx_breast_cancer:bool, dx_lung_cancer:bool, dx_colorectal_cancer:bool, site_id:int32)";
    }

};

class GeneratePhameDataNParties {
public:
    static std::string getCurrentWorkingDirectory();
    // generate [min, max)
    static int generateRandomInt(int min, int max) {
        return min + (rand() % (max - min));
    }
    static float generateRandomFloat(float min, float max) {
        return min +  drand48() * (max - min); /* [0, 1.0] */
    }
    static PatientTuple generatePatientTuple(const int & a_patid, const int & site_id) {
        PatientTuple result;
        result.patid = a_patid;
        result.site_id = site_id;
        result.age_cat = generateRandomValue<char>(domains::age_cat_);
        result.gender =  generateRandomValue<char>(domains::gender_);
        result.ethnicity = generateRandomValue<char>(domains::ethnicity_);
        result.race = generateRandomValue<char>(domains::race_);
        result.zip = generateRandomValue<string>(domains::zip_codes_);
        result.payer_primary = generateRandomValue<char>(domains::payer_codes_);
        result.payer_secondary = generateRandomValue<char>(domains::payer_codes_);
        // make the two payer codes different
        while(result.payer_primary == result.payer_secondary) {
            result.payer_secondary = generateRandomValue<char>(domains::payer_codes_);
        }
        return result;

    }

    static DxTuple generateDxTuple(const PatientTuple & patient) {
        DxTuple dxTuple;
        dxTuple.patid = patient.patid;
        dxTuple.site_id = patient.site_id;
        dxTuple.dx_diabetes = generateDxValue();
        dxTuple.dx_hypertension = generateDxValue();
        dxTuple.dx_breast_cancer = generateDxValue();
        dxTuple.dx_lung_cancer = generateDxValue();
        dxTuple.dx_colorectal_cancer = generateDxValue();
        dxTuple.dx_cervical_cancer = generateDxValue();
        return dxTuple;
    }

    static vector<PatientTuple> generatePatientTuples(const int & site_id, const int & patient_cnt);

    static void writeSchemaFile(const string & dst_filename, const string & schema);

    static void runCommand(const string & cmd, const string & cwd = "") {
        string dir = (cwd == "") ?  getCurrentWorkingDirectory() : cwd;

        string to_run = "cd " + dir + " && " + cmd + " 2>&1 ";
        system(to_run.c_str());
    }

private:
  // draws a random value in domain
  static bool generateDxValue() {
      return (rand() % 100)< DX_THRESHOLD;
  }

  template<typename T>
  static T generateRandomValue(const std::vector<T> & domain) {
        int draw = generateRandomInt(0, domain.size());
        return domain[draw];
    }


};



#endif // GENERATE_N_PARTY_DATA_H

