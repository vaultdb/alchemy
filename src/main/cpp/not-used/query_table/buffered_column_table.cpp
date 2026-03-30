#include "buffered_column_table.h"

using namespace vaultdb;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

BufferedColumnTable *BufferedColumnTable::deserialize(const vaultdb::TableMetadata &md, const vector<int> &ordinals, const int &limit) {
    return nullptr;
}

#else
#include "util/emp_manager/outsourced_mpc_manager.h"

BufferedColumnTable *BufferedColumnTable::deserialize(const vaultdb::TableMetadata &md, const vector<int> &ordinals, const int &limit) {
    bool not_limit = (limit == -1 || limit > md.tuple_cnt_);
    int tuple_cnt = not_limit ? md.tuple_cnt_ : limit;

    QuerySchema secure_schema = ordinals.empty() ? QuerySchema::toSecure(md.schema_) : QuerySchema::toSecure(OperatorUtilities::deriveSchema(md.schema_, ordinals));
    auto buffered_table = (BufferedColumnTable *) QueryTable<Bit>::getTable(tuple_cnt, secure_schema, md.collation_);

    string src_data_path = Utilities::getFilenameForTable(md.name_);
    if(!ordinals.empty() || !not_limit) {
        fstream src_data_file;
        src_data_file.open(src_data_path.c_str(), std::ios::in | std::ios::out | std::ios::binary);
        for(int i = 0; i < secure_schema.getFieldCount(); ++i) {
            int max_page = tuple_cnt / buffered_table->fields_per_page_[i] + (tuple_cnt % buffered_table->fields_per_page_[i] != 0);

            buffered_table->table_shares_file_.seekp(buffered_table->ordinal_offsets_[i], ios::beg);
            for(int j = 0; j < max_page; ++j) {
                PageId pid(buffered_table->table_id_, i, j);
                std::vector<emp::Bit> secret_shares = buffered_table->readPage(pid, md.tuple_cnt_, md.schema_,
                                                                               ordinals.empty() ? i : ordinals[i],
                                                                               src_data_file);
                std::vector<int8_t> write_buffer = buffered_table->serializeEMPBits(secret_shares);
                buffered_table->table_shares_file_.write(reinterpret_cast<char*>(write_buffer.data()), write_buffer.size());
            }
        }

        int max_dummy_page = tuple_cnt / buffered_table->fields_per_page_.at(-1) + ((tuple_cnt % buffered_table->fields_per_page_.at(-1)) != 0);

        buffered_table->table_shares_file_.seekp(buffered_table->ordinal_offsets_[-1], ios::beg);
        for(int i = 0; i < max_dummy_page; ++i) {
            PageId pid(buffered_table->table_id_, -1, i);
            std::vector<emp::Bit> secret_shares = buffered_table->readPage(pid, md.tuple_cnt_, md.schema_, -1,
                                                                           src_data_file);

            std::vector<int8_t> write_buffer = buffered_table->serializeEMPBits(secret_shares);
            buffered_table->table_shares_file_.write(reinterpret_cast<char*>(write_buffer.data()), write_buffer.size());
        }
        src_data_file.close();
    }
    else {
        buffered_table->table_shares_file_.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::filesystem::copy_file(src_data_path.c_str(), buffered_table->table_file_name_.c_str(), std::filesystem::copy_options::overwrite_existing);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        buffered_table->table_shares_file_.open(buffered_table->table_file_name_, std::ios::in | std::ios::out | std::ios::binary);

        if (!buffered_table->table_shares_file_.is_open()) {
            throw std::runtime_error(
                    std::string("In deserialize: Failed to open file '") + buffered_table->table_file_name_ + "': " + strerror(errno)
            );
        }
    }

    return buffered_table;
}

#endif