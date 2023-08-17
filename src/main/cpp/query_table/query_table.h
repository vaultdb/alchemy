#ifndef QUERY_TABLE_H
#define QUERY_TABLE_H


#include "query_schema.h"
#include "query_tuple.h"
#include <memory>
#include <ostream>
#include "util/utilities.h"
#include "plain_tuple.h"
#include <common/defs.h>

namespace  vaultdb {



    // forward declarations
    template<typename B> class QueryTable;
    template<typename B> class RowStore;
    template<typename B> class ColumnStore;


    typedef QueryTable<bool> PlainTable;
    typedef QueryTable<emp::Bit> SecureTable;


    template<typename B>
    class QueryTable {


    protected:
        SortDefinition order_by_;
        QuerySchema schema_;


    public:
        // size of each tuple in bytes
        size_t tuple_size_bytes_;
        size_t tuple_cnt_;
        std::map<int, int> field_sizes_bytes_;
        std::map<int, int> field_offsets_bytes_;


        // empty sort definition for default case
        // called by children
        QueryTable(const size_t &tuple_cnt, const QuerySchema &schema,
                   const SortDefinition &sort_def = SortDefinition())
                : order_by_(sort_def), schema_(schema), tuple_cnt_(tuple_cnt) {
                setSchema(schema);
            }


        // deep copy
         QueryTable(const QueryTable &src)  : order_by_(src.getSortOrder()), tuple_size_bytes_(src.tuple_size_bytes_), tuple_cnt_(src.tuple_cnt_) {
            setSchema(src.schema_);
        }


        virtual ~QueryTable() {}

        virtual void resize(const size_t &tuple_cnt) = 0;

       inline  bool isEncrypted() const {     return std::is_same_v<B, emp::Bit>; }

        void setSchema(const QuerySchema &schema) {
            schema_ = schema;

            if(this->isEncrypted()) {
                // covers dummy tag as -1
                tuple_size_bytes_ = 0;
                QueryFieldDesc desc;
                EmpMode mode = SystemConfiguration::getInstance().emp_mode_;
                int field_size_bytes;
                for(auto pos : schema_.offsets_) {
                    desc = schema_.getField(pos.first);
                    field_size_bytes = (mode == EmpMode::OUTSOURCED) ? desc.packedWires() * TypeUtilities::getEmpBitSize() : desc.size() * TypeUtilities::getEmpBitSize();
                    tuple_size_bytes_ += field_size_bytes;
                    // offset units are packed wires for OMPC (Bits o.w.)
                    field_offsets_bytes_[pos.first] = pos.second * TypeUtilities::getEmpBitSize();
                    field_sizes_bytes_[pos.first] = field_size_bytes;
                }
                return;
            }


            // plaintext case
            tuple_size_bytes_ = schema_.size() / 8; // bytes for plaintext

            for(auto pos : schema_.offsets_) {
                field_offsets_bytes_[pos.first] = pos.second / 8;
                field_sizes_bytes_[pos.first] = schema_.getField(pos.first).size() / 8;
            }


        }

        inline const  QuerySchema getSchema() const  { return schema_; }


        inline size_t getTupleCount() const { return tuple_cnt_; }

        virtual string toString(const bool &show_dummies = false) const = 0;

        virtual string toString(const size_t &limit, const bool &show_dummies = false) const = 0;

        virtual void putTuple(const int &idx, const QueryTuple<B> &tuple) = 0;


        inline void setSortOrder(const SortDefinition &order_by) { order_by_ = order_by; }

        inline SortDefinition getSortOrder() const { return order_by_; }


       virtual std::vector<int8_t> serialize() const = 0;


        virtual  Field<B> getField(const int  & row, const int & col)  const  = 0;
        virtual  Field<B> getPackedField(const int  & row, const int & col)  const  = 0;
        virtual int8_t* getFieldPtr(const int  & row, const int & col)  const  = 0;

        virtual  void setField(const int  & row, const int & col, const Field<B> & f) = 0;

        virtual  void setPackedField(const int  & row, const int & col, const Field<B> & f) = 0;

        B getDummyTag(const int & row)  const {
            int8_t *dummy_tag =  getFieldPtr(row, -1);
            Field<B> res = Field<B>::deserialize(schema_.getField(-1), dummy_tag);
            return res.template getValue<B>();
        }

        void setDummyTag(const int & row, const B & val) {
            int8_t *write_ptr = getFieldPtr(row, -1);
            QueryFieldDesc desc = schema_.getField(-1);
            Field<B> f(desc.getType(), val);
            f.serialize(write_ptr, desc);
        }

        virtual SecureTable *secretShare() = 0;



        virtual SecretShares
        generateSecretShares() const = 0; // generate shares for alice and bob - for data sharing (non-computing) node

        virtual PlainTable *reveal(const int & party = emp::PUBLIC) = 0;
        // holds onto dummy rows
        virtual PlainTable *revealInsecure(const int & party = emp::PUBLIC) const = 0;

        virtual QueryTable<B> &operator=(const QueryTable<B> &src) = 0;

        virtual bool operator==(const QueryTable<B> &other) const = 0;

        B operator!=(const QueryTable<B> &other) const {
            return !(*this == other);
        }


        virtual string getOstringStream() const = 0;
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

        inline PlainTuple revealRow(const int & row) const {
            QuerySchema plain_schema = QuerySchema::toPlain(schema_);
            return revealRow(row, plain_schema);
        }

        inline PlainTuple revealRow(const int & row,  QuerySchema & dst_schema, const int & party = PUBLIC) const  {
            if(std::is_same_v<B, bool>) {
                return getPlainTuple(row);
            }

            PlainTuple plain(&dst_schema);

            for(int i = 0; i < this->schema_.getFieldCount(); i++) {
                PlainField p = this->getField(row, i).reveal(party);
                plain.setField(i, p);
            }

            PlainField dummy_tag = this->getField(row, -1).reveal(party);
            plain.setDummyTag(dummy_tag);

            return plain;
        }



        inline bool empty() const { return tuple_cnt_ == 0; }
         //intentionally giving these different names - not for used in generic (not bool/Bit-specific cases)
        virtual QueryTuple<bool> getPlainTuple(size_t idx) const = 0;
        // hoping to refactor to get rid of getSecureTuple soon.
        // need to set up expressions to support a "virtual" tuple in join
        // that concats from two sources
        // requires rejiggering InputReference
//        virtual QueryTuple<Bit> getSecureTuple(size_t idx) const = 0;

        // memcpy a field from one table to another
        virtual void assignField(const int & dst_row, const int & dst_col,const  QueryTable<B> *src, const int & src_row, const int & src_col) = 0;
        virtual void cloneFields(const int &dst_row, const int &dst_start_col, const QueryTable<B> *src, const int &src_row, const int & src_start_col,
                         const int &col_cnt) = 0;


        virtual void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) = 0;
        virtual void cloneRow(const bool & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) = 0;
        virtual void cloneRow(const emp::Bit & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) = 0;
        virtual void cloneTable(const int & dst_row, QueryTable<B> *src) = 0; // for unioning 2 or more tables

        virtual QueryTable *clone() = 0;
        virtual void compareSwap(const bool & swap, const int  & lhs_row, const int & rhs_row) = 0;
        virtual void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) = 0;

        virtual StorageModel storageModel() const = 0;



    };

    static std::ostream &operator<<(std::ostream &os, const QueryTable<bool> &table)   {
        os << table.getOstringStream();
        return os;
    }

    static std::ostream &operator<<(std::ostream &os, const QueryTable<Bit> &table)   {
        os << table.getOstringStream();
        return os;
    }




}
#endif // _QUERY_TABLE_H
