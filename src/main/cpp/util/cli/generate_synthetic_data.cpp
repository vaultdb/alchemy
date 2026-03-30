#include "generate_synthetic_data.h"
#include <cassert>
#include <filesystem>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <util/field_utilities.h>

#include "util/data_utilities.h"




#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif


void GenerateSyntheticData::generateOverlapping() {
    for (auto t : table_defs_) {
        int overlapping_rows = ((float) t.cardinality_) * t.overlap_;
        int overlapping_rows_per_party = overlapping_rows / party_cnt_;
        int rows_per_party = t.cardinality_ / party_cnt_ - overlapping_rows_per_party;

        int remaining_rows = overlapping_rows - overlapping_rows_per_party * party_cnt_;
        int cursor = 0;
        for (int i = 0; i < party_cnt_; ++i) {
            string dst_filename = dst_path_ + "/" + std::to_string(i) + "/" + t.name_ + ".csv";
            string schema_filename = dst_path_ + "/" + std::to_string(i) + "/" + t.name_ + ".schema";
            std::ofstream dst_file;
            dst_file.open(dst_filename, ios::out);

            if(!dst_file.is_open() ) {
                std::cout << "Failed to open table file: " << dst_filename << std::endl;
                exit(-1);
            }

            for (int j = 0; j < rows_per_party; ++j) {
                dst_file << t.generateRow(cursor) << std::endl;
                ++cursor;
            }

            // for overlapping rows
            for (int j = 0; j < overlapping_rows_per_party; ++j) {
                auto row_id = generateOverlappingRowId(i, rows_per_party);
                dst_file << t.generateRow(row_id) << std::endl;
            }

            // draw one more row if there's a modulus
            if (remaining_rows > i) {
                auto row_id = generateOverlappingRowId(i, rows_per_party);
                dst_file << t.generateRow(row_id) << std::endl;
            }
            dst_file.close();
            writeSchemaFile(schema_filename, t.schema_);
        }
    }


}

void GenerateSyntheticData::generateOneHost() {
    for (auto t : table_defs_) {
        int cursor = 0;
        string dst_filename = dst_path_ + "/" + t.name_ + ".csv";
        string schema_filename = dst_path_ + "/" + t.name_ + ".schema";
        std::ofstream dst_file;
        dst_file.open(dst_filename, ios::out);

        if(!dst_file.is_open() ) {
            std::cout << "Failed to open table file: " << dst_filename << std::endl;
            exit(-1);
        }

        for (int j = 0; j < t.cardinality_; ++j) {
            dst_file << t.generateRow(cursor) << std::endl;
            ++cursor;
        }

        dst_file.close();
        writeSchemaFile(schema_filename, t.schema_);
    } // end for table_defs_
}

void GenerateSyntheticData::parseJson(const string &json_file) {


    stringstream ss;
    std::vector<std::string> json_lines = DataUtilities::readTextFile(json_file);
    for(vector<string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
        ss << *pos << endl;

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);


    party_cnt_ = parseField<int>(pt, "parties");
    if (party_cnt_ <= 0) {
        party_cnt_ = 1; // single host config like RESCU
    }
    dst_path_ = parseField<string>(pt, "dst_path");

    if (dst_path_[0] != '/') // give it an absolute path
       dst_path_= Utilities::getCurrentWorkingDirectory() + "/" + dst_path_;

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("tables."))
    {
        assert(v.first.empty()); // array elements have no names
        parseTable(v.second);
    }
}






void GenerateSyntheticData::parseTable(boost::property_tree::ptree & table) {

    TableDefinition table_def;
    table_def.name_ = parseField<string>(table, "name");
    auto schema_str = parseField<string>(table, "schema");
    table_def.schema_ = QuerySchema(schema_str);

    string key_str = parseField<string>(table, "key");
    // check if ordinal
    if (key_str.find_first_not_of("0123456789") == std::string::npos) {
        table_def.key_idx_ = atoi(key_str.c_str());
    }
    else {
        table_def.key_idx_ = table_def.schema_.getField(key_str).getOrdinal();
    }

    table_def.cardinality_ = parseField<int>(table, "cardinality");
    if (party_cnt_ > 1) {
        table_def.overlap_ = parseField<float>(table, "overlap");
        if (table_def.overlap_ < 0.0 || table_def.overlap_ > 1.0) {
            throw runtime_error("Invalid overlap: " + to_string(table_def.overlap_));
        }
    }
    // for each domain
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, table.get_child("domains."))
    {
        assert(v.first.empty()); // array elements have no names
        auto domain_entry = v.second;
        // a hack to get the domain name
        auto itr = domain_entry.begin();
        string domain_name = itr->first;
        ColumnDefinition domain_def;
        auto field_desc = table_def.schema_.getField(domain_name);
        domain_def.def_ = field_desc;

        if (itr->second.get_value<string>() != "") {
            // list domain
            domain_def.type_ = DomainType::LIST;
            std::stringstream ss(itr->second.get_value<string>());
            while( ss.good() )
            {
                string substr;
                getline( ss, substr, ',' );
                // remove leading and trailing whitespace
                substr.erase(0, substr.find_first_not_of(" \t"));
                substr.erase(substr.find_last_not_of(" \t") + 1);
                Field<bool> f = FieldUtilities::parseField(field_desc.getType(), substr);
                domain_def.domain_.push_back(f);
            }

        }
        else if (itr->second.count("p") > 0 && field_desc.getType() == FieldType::BOOL) {
            // weighted bool
            domain_def.type_ = DomainType::WEIGHT;
            auto p = parseField<float>(itr->second, "p");
            if (p < 0.0 || p > 1.0) {
                throw runtime_error("Invalid probability: " + to_string(p));
            }
            domain_def.weight_ = p;
        }
        else {
            domain_def.type_ = DomainType::RANGE;
            assert(field_desc.getType() == FieldType::INT || field_desc.getType() == FieldType::LONG);
            // range domain
            domain_def.min_ = parseField<long>(itr->second, "min");
            domain_def.max_ = parseField<long>(itr->second, "max");
        }
        table_def.col_defs_[field_desc.getOrdinal()] = domain_def;



    }

    // for each bool column default to generating random values
    for (int i = 0; i < table_def.schema_.getFieldCount(); ++i) {
        if (table_def.col_defs_.find(i) == table_def.col_defs_.end() && table_def.schema_.getField(i).getType() == FieldType::BOOL) {
            ColumnDefinition col_def;
            col_def.def_ = table_def.schema_.getField(i);
            col_def.type_ = DomainType::LIST;
            col_def.domain_ = {
                Field<bool>(FieldType::BOOL, false),
                Field<bool>(FieldType::BOOL, true)
            };
            table_def.col_defs_[i] = col_def;
        }
    }
    table_defs_.push_back(table_def);
}

void GenerateSyntheticData::writeSchemaFile(const string & dst_file, QuerySchema & schema) {
    std::ofstream schema_file;
    schema_file.open(dst_file, ios::out);
    schema_file << schema << std::endl;
    schema_file.close();
}





int main(int argc, char **argv) {
    if(argc < 2) {
        std::cout << "usage: generate_synthetic_data <json config> [--seed <seed>]" << std::endl;
        exit(-1);
    }

    string json_file = argv[1];
    int seed = time(nullptr);  // default seed: current time

    // check for optional --seed flag
    for (int i = 2; i < argc - 1; ++i) {
        if (string(argv[i]) == "--seed") {
            seed = std::stoi(argv[i + 1]);
        }
    }

    srand(seed);  // SET THE SEED HERE
    std::cout << "Using seed: " << seed << std::endl;

    GenerateSyntheticData gen(json_file);
    gen.generate();
    std::cout << "Generated data from " << json_file << " to " << gen.dst_path_ << std::endl;
    std::cout << "Done." << std::endl;
}


