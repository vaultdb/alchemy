#ifndef _COMPUTING_PARTY_PACKED_COLUMN_TABLE_
#define _COMPUTING_PARTY_PACKED_COLUMN_TABLE_
#include "query_table/packed_column_table.h"
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_set>

#include "util/emp_manager/outsourced_mpc_manager.h"

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")
namespace vaultdb {
    class ComputingPartyPackedColumnTable : public PackedColumnTable  {
    public:
        ComputingPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : PackedColumnTable(tuple_cnt, schema, sort_def) { }

        ComputingPartyPackedColumnTable(const ComputingPartyPackedColumnTable &src) : PackedColumnTable(src) {}

        QueryTable<Bit> *clone()  override {
            return new ComputingPartyPackedColumnTable(*this);
        }

        void writeToFile(const string &fq_filename) const override {
            throw runtime_error("Not yet implemented!");
        }


    };
} // namespace vaultdb


#else

namespace vaultdb {
class ComputingPartyPackedColumnTable : public PackedColumnTable  {
public:
    ComputingPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : PackedColumnTable(tuple_cnt, schema, sort_def) {
        // create table file
        SystemConfiguration & s = SystemConfiguration::getInstance();
        table_file_name_ = s.temp_db_path_ + "/table_" + std::to_string(table_id_) + "_wires.p" + std::to_string(s.party_);

        if(tuple_cnt == 0)
            return;


        OMPCPackedWire zero(block_cnt_);
        auto mgr = (OutsourcedMpcManager *) s.emp_manager_;
        vector<int8_t> zero_block = mgr->serializePackedWire(zero);
        vector<int8_t> one_block = mgr->serializePackedWire(one_wire_);

        assert(zero_block.size() == packed_wire_size_bytes_);

        packed_pages_file_ = openPackedTableFile(table_file_name_);

        // initialize packed wires
        auto zero_page_cnt = this->ordinal_offsets_.at(-1) / packed_wire_size_bytes_;
        for (int i = 0; i < zero_page_cnt; ++i) {
            packed_pages_file_.write(reinterpret_cast<char *>(zero_block.data()), zero_block.size());
        }

        int dummy_page_cnt = tuple_cnt_ / page_size_bits_ + ((tuple_cnt_ % page_size_bits_) != 0);

        for (int i = 0; i < dummy_page_cnt; ++i) {
            packed_pages_file_.write(reinterpret_cast<char*>(one_block.data()), one_block.size());
        }

        packed_pages_file_.flush();
    }

    // copies src table into a temp table
    ComputingPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const string & src_filename, const int & src_tuple_cnt, const SortDefinition &sort_def = SortDefinition())
    : PackedColumnTable(tuple_cnt, schema, sort_def) {
        // create table file
        table_file_name_ = SystemConfiguration::getInstance().temp_db_path_ + "/table_" + std::to_string(table_id_) + "_wires.p" + std::to_string(SystemConfiguration::getInstance().party_);
        if(tuple_cnt == 0)
            return;

        if(tuple_cnt == src_tuple_cnt) {
            std::filesystem::copy_file(src_filename.c_str(), table_file_name_.c_str(), std::filesystem::copy_options::overwrite_existing);
            packed_pages_file_ = openPackedTableFile(table_file_name_);
            return;
        }

        auto src_ordinal_offsets = getOrdinalOffsets(schema, src_tuple_cnt);

        fstream src_file = openPackedTableFile(src_filename);
        packed_pages_file_ = openPackedTableFile(table_file_name_);


        cout << "Src file size " << std::filesystem::file_size(src_filename) << " dest file size " << std::filesystem::file_size(table_file_name_) << endl;
        // initialize packed wires
        for(int i = 0; i < schema.getFieldCount(); ++i) {
            int read_size = bytesPerColumn(schema.getField(i), tuple_cnt);
            cout << "Src file is open? " << src_file.is_open() << " good? " <<  src_file.good() << endl;
            cout << "Reading from " << src_ordinal_offsets[i] << " for " << read_size << " bytes\n";
            copyAndWriteWires(src_file, src_ordinal_offsets.at(i), this->packed_pages_file_, this->ordinal_offsets_[i], read_size);
        }

        int read_offset = src_ordinal_offsets.at(-1);
        int read_size = bytesPerColumn(schema.getField(-1), tuple_cnt);
        copyAndWriteWires(src_file, read_offset, this->packed_pages_file_, this->ordinal_offsets_[-1], read_size);

        src_file.close();
    }



    ComputingPartyPackedColumnTable(const ComputingPartyPackedColumnTable &src) : PackedColumnTable(src){
        ComputingPartyPackedColumnTable *src_table = const_cast<ComputingPartyPackedColumnTable *>(&src);
        bpm_.flushTable<Bit>(src_table->table_id_);
        src_table->packed_pages_file_.flush();
        src_table->packed_pages_file_.close();

        table_file_name_ = SystemConfiguration::getInstance().temp_db_path_ + "/table_" + std::to_string(table_id_) + "_wires.p" + std::to_string(SystemConfiguration::getInstance().party_);

        string src_table_file = src.table_file_name_;
        assert(!src_table_file.empty());

        std::filesystem::copy_file(src_table_file.c_str(), table_file_name_.c_str(), std::filesystem::copy_options::overwrite_existing);

        src_table->packed_pages_file_.open(src_table_file.c_str(), std::ios::in | std::ios::out | std::ios::binary);

        packed_pages_file_ = openPackedTableFile(table_file_name_);

    }

    ~ComputingPartyPackedColumnTable() {
        // Flush pages in buffer pool
        bpm_.removeTable<Bit>(this->table_id_);

        // close the file
        if (packed_pages_file_.is_open() && packed_pages_file_.good()) {
            packed_pages_file_.close();
        }

        // remove the file
        string temp_dir = SystemConfiguration::getInstance().temp_db_path_;
        if (table_file_name_.substr(0, temp_dir.length()) == temp_dir) {
            filesystem::remove(table_file_name_.c_str());
        }

    }

    QueryTable<Bit> *clone()  override {
        return new ComputingPartyPackedColumnTable(*this);
    }



    void putTuple(const int &idx, const QueryTuple<Bit> &tuple) override {
        throw std::runtime_error("not yet implemented");
    }

    vector<int8_t> serialize() override {
        throw std::runtime_error("not yet implemented");
    }

    vector<int8_t> serializePackedWire(const PageId & pid) const override {
        vector<int8_t> dst = vector<int8_t>(packed_wire_size_bytes_, 0);

        int read_offset = getPageOffset(pid);
        packed_pages_file_.seekg(read_offset, ios::beg);
        packed_pages_file_.read(reinterpret_cast<char*>(dst.data()), packed_wire_size_bytes_);

        return dst;
    }

    vector<int8_t> serializePackedWire(OMPCPackedWire & wire) const override {
        return ((OutsourcedMpcManager *) SystemConfiguration::getInstance().emp_manager_)->serializePackedWire(wire);
    }

    emp::OMPCPackedWire deserializePackedWire(int8_t *serialized_packed_wire) const override {
        return ((OutsourcedMpcManager *) SystemConfiguration::getInstance().emp_manager_)->deserializePackedWire(serialized_packed_wire);

    }

     void writePackedWire(const PageId & pid, OMPCPackedWire & wire) override {
        int write_offset = getPageOffset(pid);
        vector<int8_t> ser = serializePackedWire(wire);

        packed_pages_file_.seekp(write_offset, ios::beg);
        if (ser.size() != packed_wire_size_bytes_) {
            throw runtime_error("Serialized packed wire size does not match expected size.");
        }
        packed_pages_file_.write(reinterpret_cast<char *>(ser.data()), packed_wire_size_bytes_);
        packed_pages_file_.flush();
    }

     OMPCPackedWire readPackedWire(const PageId & pid) const override {
        vector<int8_t> dst = serializePackedWire(pid);
        return deserializePackedWire(dst.data());
    }

    void getPage(const PageId &pid, int8_t *dst) override {
       cout << "Reading packed wire for " << pid.toString() << " from file." << endl;
       OMPCPackedWire wire = readPackedWire(pid);
       manager_->unpack((Bit *) &wire,  (Bit *) dst, page_size_bits_);
    }


    void flushPage(const PageId &pid, int8_t *src) override {
        cout << "Packing " << DataUtilities::printByteArray(src, 16) << " for " << pid.toString() << '\n';

        OMPCPackedWire wire(block_cnt_);
        cout << "Packing page " << pid.toString()  << endl;
        manager_->pack((Bit *) src, (Bit *) &wire, page_size_bits_);
        cout << "Writing packed wire for " << pid.toString() << endl;
        writePackedWire(pid, wire);
    }

    void appendColumn(const QueryFieldDesc & desc) override {
        appendColumnSetup(desc);

        OMPCPackedWire zero(block_cnt_);
        vector<int8_t> zero_block = serializePackedWire(zero);

        int ordinal = desc.getOrdinal();
        int col_packed_wires =  tuple_cnt_ / fields_per_page_.at(ordinal) + (tuple_cnt_ % fields_per_page_.at(ordinal) != 0);

        packed_pages_file_.seekp(this->ordinal_offsets_[ordinal], ios::beg);
        for(int j = 0; j < col_packed_wires; ++j) {
            packed_pages_file_.write(reinterpret_cast<char *>(zero_block.data()), zero_block.size());
        }

        packed_pages_file_.flush();
    }


    void writeToFile(const string &fq_filename) const override {
        throw runtime_error("Not yet implemented!");

    }


};
} // namespace vaultdb

#endif

#endif
