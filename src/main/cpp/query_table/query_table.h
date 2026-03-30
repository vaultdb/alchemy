#ifndef _QUERY_TABLE_H_
#define _QUERY_TABLE_H_


#include "query_schema.h"
#include "query_tuple.h"
#include <memory>
#include <ostream>
#include "util/utilities.h"
#include "plain_tuple.h"
#include "util/data_utilities.h"
#include "util/system_configuration.h"
#include "util/buffer_pool/buffer_pool_manager.h"


namespace  vaultdb {

    template<typename B> class QueryTable;
    typedef QueryTable<bool> PlainTable;
    typedef QueryTable<emp::Bit> SecureTable;

    template<typename B>
    class QueryTable {

        public:
            size_t tuple_size_bytes_;
            size_t tuple_cnt_;
            std::map<int, int> field_sizes_bytes_;
            std::map<int, int> fields_per_page_; // how many fields are in each page
            std::map<int, long> ordinal_offsets_; // offsets for each column in bytes in the file

            bool pinned_ = false; // if set to true, do not delete table automatically.  For use in operators with CTEs
            SortDefinition order_by_;
            int table_id_ = SystemConfiguration::getInstance().table_cnt_++;

            QueryTable<B>(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition & sort_def) : order_by_(sort_def), tuple_cnt_(tuple_cnt) {
                this->ordinal_offsets_ = getOrdinalOffsets(schema, tuple_cnt);
                setSchema(schema);
                SystemConfiguration::getInstance().bpm_.registerTable<B>(this);
            }

            QueryTable<B>(const QueryTable<B> &src) : order_by_(src.order_by_), tuple_cnt_(src.tuple_cnt_), ordinal_offsets_(src.ordinal_offsets_) {
                pinned_ = false;
                setSchema(src.schema_);
                SystemConfiguration::getInstance().bpm_.registerTable<B>(this);
                // delegate data copying to child class
            }

            virtual ~QueryTable() = default;

            // pure virtual  methods to implement in child classes
            // copy page from disk to buffer pool
            virtual void getPage(const PageId &pid, int8_t *dst) = 0;
            // write page to disk if applicable
            virtual void flushPage(const PageId &pid, int8_t *src) = 0;
            virtual void resize(const size_t &tuple_cnt) = 0;
            virtual StorageModel storageModel() const = 0;
            virtual void appendColumn(const QueryFieldDesc & desc) = 0;
            virtual SecureTable *secretShare() = 0;
            virtual  int8_t *getFieldPtr(const int & row, const int & col) const = 0;
            // includes dummy tag for cloneRow
            virtual void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) = 0;

            // conditional writes
            virtual void cloneRow(const B & write, const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row) = 0;

            // includes dummy tag for cloneRow
            // copy row src_row from src to self at dst_row and fill the next copies rows with the same data
            virtual void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row, const int & copies) = 0;
            virtual QueryTuple<B> getRow(const int & idx) = 0;
            virtual void setRow(const int & idx, const QueryTuple<B> &tuple) = 0;
            virtual void setDummyTag(const int & row, const B & val) = 0;
            virtual B getDummyTag(const int & row) const = 0;
            virtual void setField(const int &row, const int &col, const Field<B> &f) = 0;
            virtual Field<B> getField(const int &row, const int &col) const = 0;

            virtual QueryTable<B> *clone() = 0;
            virtual void compareSwap(const B & swap, const int  & lhs_row, const int & rhs_row) = 0;

            virtual void cloneColumn(const int & dst_col, const QueryTable<B> *src, const int & src_col) = 0;
            virtual void putTuple(const int &idx, const QueryTuple<B> &tuple) = 0;
            virtual void writeToFile(const string & fq_filename) const = 0;

        // copy all src_col entries from src_row until end (or until we run out of slots)
        // to self at dst_col starting at dst_row.
        virtual void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<B> *src, const int & src_col, const int & src_row = 0) = 0;

        virtual void cloneTable(const int & dst_row, const int & dst_col, QueryTable<B> *src) = 0;

        virtual vector<int8_t> serialize() = 0;

        inline  bool isEncrypted() const {     return std::is_same_v<B, emp::Bit>; }



            inline const  QuerySchema & getSchema() const  { return schema_; }




        // virtual void revealAndWrite(const int & party, const string & filename);

        SecretShares generateSecretShares(); // generate shares for alice and bob - for data sharing (non-computing) node
        vector<vector<int8_t> > generateSecretShares(const int & party_count); // generate shares for all parties


        QueryTable<B> &operator=(const QueryTable<B> &src);

        // needs to be here (not virtual) because it is a static function
        // only produces ColumnTable<B> objects
        static QueryTable<B> *deserialize(const QuerySchema &schema, const vector<Bit> &table_bits, const int & limit = -1) {
            return deserialize(schema, reinterpret_cast<const int8_t *>(table_bits.data()), table_bits.size() * sizeof(Bit), limit);
        }
        static QueryTable<B> *deserialize(const QuerySchema &schema, const int8_t *table_bytes, const int &byte_cnt, const int & limit = -1);
        static QueryTable<B> *deserialize(const TableMetadata & md, const int & limit = -1);
        static QueryTable<B> *deserialize(const TableMetadata & md, const vector<int> & col_ordinals, const int & limit = -1);
        static QueryTable<B> *deserialize(const TableMetadata & md, const string & col_names_csv = "", const int & limit = -1);

        static void revealAndWrite(int party, QueryTable<B> *table, const string &fq_filename) {
            assert(table->isEncrypted());
            auto plain_table = table->reveal(party);
            plain_table->writeToFile(fq_filename);
            delete plain_table;
        }

        int getTrueTupleCount() const {
            assert(!isEncrypted());
            size_t count = 0;

            for(size_t i = 0; i < tuple_cnt_; ++i) {
                B dummy_tag = getDummyTag(i); // always bool because !isEncrypted
                bool d = *((bool *) &dummy_tag);
                if(!d) {
                    ++count;
                }
            }

            return count;

        }

        PlainTable *reveal(const int & party = emp::PUBLIC);

        // holds onto dummy rows
        virtual PlainTable *revealInsecure(const int & party = emp::PUBLIC) const;

        inline PlainTuple revealRow(const int &row, const int &party = PUBLIC)    {
            if(std::is_same_v<B, bool>) {
                return getPlainTuple(row);
            }

            // this needs to be plain_schema_ or some other object that will exist beyond this method in order to work.
            PlainTuple plain(&plain_schema_);
            int field_cnt = schema_.getFieldCount();

            for(int i = 0; i < field_cnt; i++) {
                auto s = getField(row, i);
                auto p = s.reveal(schema_.getField(i), party);
                plain.setField(i, p);
            }

            PlainField dummy_tag = getField(row, -1).reveal(schema_.getField(-1), party);
            plain.setDummyTag(dummy_tag);

            return plain;
        }



        inline bool empty() const { return tuple_cnt_ == 0; }


        QueryTuple<bool> getPlainTuple(size_t idx) const;


        virtual void cloneTable(const int & dst_row, QueryTable<B> *src) {
            this->cloneTable(dst_row, 0, src);
        }


        // for serializing a row
       Integer unpackRow(const int & row, const int & col_cnt=-1) const {
           assert(isEncrypted());
           auto src = (SecureTable *) this;
            int to_unpack = (col_cnt < 1) ? schema_.getFieldCount() : col_cnt;

            Integer dst(schema_.size(), 0, PUBLIC);
            Bit *cursor = dst.bits.data();

            for(int i = 0; i < to_unpack; ++i) {
                SecureField f = src->getField(row, i);
                QueryFieldDesc desc = schema_.getField(i);

                switch(f.getType()) {
                    case FieldType::SECURE_BOOL: {
                        Bit b = f.getValue<emp::Bit>();
                        *cursor = b;
                        break;
                    }
                    case FieldType::SECURE_INT:
                    case FieldType::SECURE_LONG:
                    case FieldType::SECURE_STRING: {
                        Integer i_field = f.getInt();
                        memcpy(cursor, i_field.bits.data(), desc.size() * sizeof(Bit));
                        break;
                    }
                    case FieldType::SECURE_FLOAT: {
                        Float f_field = f.getFloat();
                        memcpy(cursor, f_field.value.data(), desc.size() * sizeof(Bit));
                        break;
                    }
                    default:
                        throw;

                }
                cursor += desc.size();
            }

            // do dummy tag last
            SecureField b_field = src->getDummyTag(row);
            Bit b = b_field.getValue<emp::Bit>();
            *cursor = b;
            return dst.bits;
        }

        // does not include dummy tag
        Integer unpackRow(const int & row, const int & col_cnt, const int & selection_length_bits) const {
            assert(isEncrypted());
            auto src = (SecureTable *) this;
            Integer dst(selection_length_bits, 0, PUBLIC);
            Bit *write_cursor = dst.bits.data();

            for(int i = 0; i < col_cnt; ++i) {
                SecureField f = src->getField(row, i);
                QueryFieldDesc desc = schema_.fields_.at(i);

                switch(f.getType()) {
                    case FieldType::SECURE_BOOL: {
                        Bit b = f.getValue<emp::Bit>();
                        *write_cursor = b;
                        break;
                    }
                    case FieldType::SECURE_INT:
                    case FieldType::SECURE_LONG:
                    case FieldType::SECURE_STRING: {
                        Integer i_field = f.getInt();
                        memcpy(write_cursor, i_field.bits.data(), desc.size() * sizeof(Bit));
                        break;
                    }
                    case FieldType::SECURE_FLOAT: {
                        Float f_field = f.getFloat();
                        memcpy(write_cursor, f_field.value.data(), desc.size() * sizeof(Bit));
                        break;
                    }
                    default:
                        throw;

                }
                write_cursor += desc.size();
                if((write_cursor - dst.bits.data()) >= selection_length_bits)
                    break;
            }
            return dst;
        }


         inline void packRow(const int & row, Integer src) {
           assert(isEncrypted());
           auto dst = (SecureTable *) this;
            Bit *cursor = src.bits.data();
            Bit *end = src.bits.data() + src.size() - 1; // -1 for dummy tag, do this separately
            int dst_idx = 0;
            while(cursor < end) {
                QueryFieldDesc desc = schema_.getField(dst_idx);
                switch(desc.getType()) {
                    case FieldType::SECURE_BOOL: {
                        Bit b = *cursor;
                        SecureField f(FieldType::SECURE_BOOL, b);
                        dst->setField(row, dst_idx, f);
                        break;
                    }
                    case FieldType::SECURE_INT:
                    case FieldType::SECURE_LONG:
                    case FieldType::SECURE_STRING: {
                        Integer i_field(desc.size() + desc.bitPacked(), 0, PUBLIC);
                        memcpy(i_field.bits.data(), cursor, desc.size() * sizeof(Bit));
                        SecureField f(desc.getType(), i_field);
                        dst->setField(row, dst_idx, f);
                        break;
                    }
                    case FieldType::SECURE_FLOAT: {
                        Float f_field;
                        memcpy(f_field.value.data(), cursor, desc.size() * sizeof(Bit));
                        SecureField f(FieldType::SECURE_FLOAT, f_field);
                        dst->setField(row, dst_idx, f);
                        break;
                    }
                    default:
                        throw;

                }
                cursor += desc.size();
                ++dst_idx;
            }

            // do dummy tag last
            Bit dummy_tag = *end;
            dst->setDummyTag(row, dummy_tag);
        }


        bool operator==(const QueryTable<B> &other) const;
        bool operator!=(const QueryTable<B> &other) const { return !(*this == other); }
        string toString(const bool &show_dummies = false) const;
        string toString(const size_t &limit, const bool &show_dummies = false) const;




        void setSchema(const QuerySchema &schema)  {
            assert(!(schema.getFieldCount() == 0));  // must have at least one field

            this->schema_ = schema;
            this->plain_schema_ = QuerySchema::toPlain(schema);
            int bp_page_size_bytes = SystemConfiguration::getInstance().bpm_.page_size_bytes_;


            int field_size_bytes;
            QueryFieldDesc desc;

            tuple_size_bytes_ = 0;
            for (int i = -1; i < schema.getFieldCount(); ++i) {
                desc = schema_.getField(i);
                field_size_bytes = desc.sizeBytes();
                field_sizes_bytes_[i] = field_size_bytes;
                fields_per_page_[i] = bp_page_size_bytes / field_size_bytes; // how many fields fit in a page

                tuple_size_bytes_ += field_size_bytes;
            }

        }

        static QueryTable<B> *getTable(const size_t &tuple_cnt, const QuerySchema &schema, const StorageModel & model, const SortDefinition &sort_def = SortDefinition());
        // uses default storage model from SystemConfiguration
        static QueryTable<B> *getTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition());


        // if no bpm enabled, bpm does nothing
        void pinRow(const int & row) {
           for (int i = -1; i < this->schema_.getFieldCount(); ++i) {
                PageId pid(this->table_id_, i, row / this->fields_per_page_.at(i));
                SystemConfiguration::getInstance().bpm_.pinPage(pid);
            }
        }

        void unpinRow(const int & row) {
            for (int i = -1; i < this->schema_.getFieldCount(); ++i) {
                PageId pid(this->table_id_, i, row / this->fields_per_page_.at(i));
                SystemConfiguration::getInstance().bpm_.unpinPage(pid);
            }
        }

        // includes dummy tag
        // write out row contents to a byte array and return
        // length is in bytes, includes dummy tag
    virtual vector<int8_t> serializeRow(const int & row, const int & length = -1) const  {
            int alloc_length = (length < 0) ? this->tuple_size_bytes_ : length;
            vector<int8_t> dst(alloc_length);
            int8_t *write_ptr = dst.data();
            memset(dst.data(), 0, dst.size()); // zero out the array

            for (int i = 0; i < this->schema_.getFieldCount(); ++i) {
                Field<B> to_write = getField(row, i);
                Field<B>::serialize(write_ptr, to_write, this->schema_.getField(i));
                write_ptr += this->field_sizes_bytes_.at(i);
            }

            // dummy tag goes at the end unconditionally
           write_ptr = dst.data() + dst.size() - sizeof(B);
           B dummy_tag = getDummyTag(row);
           *((B *) write_ptr) = dummy_tag;
           return dst;
        }

        virtual void deserializeRow(const int & row, vector<int8_t> & src) {
        assert(src.size() == this->tuple_size_bytes_); // must be same size as tuple size, padding done in serialize()
        int8_t *cursor = src.data();


        // re-pack row
        for (int i = 0; i < this->schema_.getFieldCount(); ++i) {
            Field<B> deser = Field<B>::deserialize(this->schema_.getField(i), cursor);
            setField(row, i, deser); // need setField to maintain dirty bit in BPM
            cursor += this->field_sizes_bytes_.at(i);
        }

        B dummy_tag = *((B *) cursor);
        this->setDummyTag(row, dummy_tag);

    }



        static map<int, long>  getOrdinalOffsets(const QuerySchema &schema, const int & tuple_cnt) {
        long array_byte_cnt = 0;
        map<int, long> ordinal_offsets;
        SystemConfiguration & s = SystemConfiguration::getInstance();
        int page_size_bytes = s.bpm_.page_size_bytes_;

        for(int i = 0; i < schema.getFieldCount(); ++i) {
            ordinal_offsets[i] = array_byte_cnt;
            int field_size_bytes = schema.getField(i).sizeBytes();

            auto col_byte_cnt = Utilities::pageAlignedBytes(field_size_bytes, tuple_cnt, page_size_bytes);
            auto col_page_cnt = col_byte_cnt / page_size_bytes + (col_byte_cnt % page_size_bytes != 0);
            array_byte_cnt +=  col_page_cnt * page_size_bytes;
        }

        ordinal_offsets[-1] = array_byte_cnt;
        return ordinal_offsets;
    }

    protected:
        QuerySchema schema_;
        QuerySchema plain_schema_;


    private:
       static map<int, long> getSerializedOrdinalOffsets(const QuerySchema &schema, const int & tuple_cnt)  {
            long array_byte_cnt = 0;
            map<int, long> ordinal_offsets;
            SystemConfiguration & s = SystemConfiguration::getInstance();
            int page_size_bytes = s.bpm_.page_size_bytes_;

            for(int i = 0; i < schema.getFieldCount(); ++i) {
                ordinal_offsets[i] = array_byte_cnt;
                int field_size_bytes = schema.getField(i).size();
                if (schema.isSecure()) {
                    field_size_bytes *= sizeof(Bit);
                }
                else {
                    field_size_bytes /= 8;
                }
                long col_byte_cnt = tuple_cnt * field_size_bytes;
                array_byte_cnt +=  col_byte_cnt;
            }

            ordinal_offsets[-1] = array_byte_cnt;
            return ordinal_offsets;
        }



    };


    static std::ostream &operator<<(std::ostream &os, const QueryTable<bool> &table)   {
        os << "(table id: " << table.table_id_ << "): " << table.getSchema() << " isEncrypted? false, order by: " << DataUtilities::printSortDefinition(table.order_by_) << std::endl;

        for(int i = 0; i < table.tuple_cnt_; ++i) {


            os <<  table.getPlainTuple(i);

            if(!table.getDummyTag(i))
                os << std::endl;
        }

        return os;
    }

    static std::ostream &operator<<(std::ostream &os, const QueryTable<Bit> &table)   {
        throw std::runtime_error("secret shared table!");
    }




}

#endif //  _QUERY_TABLE_H_