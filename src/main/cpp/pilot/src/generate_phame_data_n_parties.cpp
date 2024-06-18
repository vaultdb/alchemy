#include "generate_phame_data_n_parties.h"
#include <cassert>
#include <filesystem>

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif


std::string GeneratePhameDataNParties::getCurrentWorkingDirectory() {
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








int main(int argc, char **argv) {
    // usage: generate_phame_data_n_parties <target dir> <party count> <tuple count>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g.,  ./bin/generate_phame_data_n_parties pilot/test/input/phame 4 100

    // each party has <tuple count> tuples
    if(argc < 4) {
        std::cout << "usage: generate_phame_data_n_parties <dst path> <party count> <tuple count per party>" << std::endl;
        exit(-1);
    }


    string dst_path = argv[1];
    int party_cnt = atoi(argv[2]);
    int tuple_cnt = atoi(argv[3]);
    srand (time(nullptr));

    dst_path = GeneratePhameDataNParties::getCurrentWorkingDirectory() + "/" + dst_path;

    for(int i = 0; i < party_cnt; ++i) {
        string party_path = dst_path + "/" + std::to_string(i);
        GeneratePhameDataNParties::runCommand("mkdir -p " + party_path);

        GeneratePhameDataNParties::writeSchemaFile(party_path + "/phame_demographic.schema", PatientTuple::getSchema());
        GeneratePhameDataNParties::writeSchemaFile(party_path + "/phame_diagnosis.schema", DxTuple::getSchema());


        std::ofstream pat_file, dx_file;
        string pat_filename = party_path + "/phame_demographic.csv";
        string dx_filename = party_path + "/phame_diagnosis.csv";
        cout << "Writing to " << pat_filename << "\n   and " << dx_filename << "\n";
        pat_file.open(pat_filename, ios::out);
        dx_file.open(dx_filename, ios::out);

        if(!pat_file.is_open() ) {
            std::cout << "Failed to open patient file: " << pat_filename << std::endl;
            exit(-1);
        }

        for(int j = 0; j < tuple_cnt; ++j) {
              PatientTuple p = GeneratePhameDataNParties::generatePatientTuple(j);
              pat_file << p.toString() << std::endl;
              if(rand() % 100 >= DX_RATE) {
                    DxTuple dx = GeneratePhameDataNParties::generateDxTuple(p);
                    dx_file << dx.toString() << std::endl;
              }
        }
        pat_file.close();
        dx_file.close();
    } // end per-party loop
}

void GeneratePhameDataNParties::writeSchemaFile(const string &dst_filename, const string &schema) {
    std::ofstream schema_file;
    schema_file.open(dst_filename, ios::out);
    schema_file << schema << std::endl;
    schema_file.close();
}

vector<PatientTuple> GeneratePhameDataNParties::generatePatientTuples(const int &patient_cnt) {
    vector<PatientTuple> output;
    for(int i = 0; i < patient_cnt; ++i) {
        PatientTuple patient = generatePatientTuple(i);
        output.push_back(patient);
    }
    return output;
}

