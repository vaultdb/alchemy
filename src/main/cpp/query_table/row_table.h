#ifndef _ROW_TABLE_H_
#define _ROW_TABLE_H_


#include "query_schema.h"
#include "query_tuple.h"
#include "query_table.h"
#include <memory>
#include <ostream>
#include "util/utilities.h"
#include "util/data_utilities.h"
#include "plain_tuple.h"
#include "query_table.h"


namespace  vaultdb {



    template<typename B> class QueryTable;
    typedef QueryTable<bool> PlainTable;
    typedef QueryTable<emp::Bit> SecureTable;



    template<typename B>
    class RowTable : public QueryTable<B> {



    public:

        std::vector<int8_t> tuple_data_;

        // empty sort definition for default case
        RowTable(const size_t &tuple_cnt, const QuerySchema &schema,
                   const SortDefinition &sort_def = SortDefinition());

        // deep copy
        explicit RowTable(const QueryTable<B> &src);

        virtual ~RowTable() {}

        void resize(const size_t &tuple_cnt) override;



        void putTuple(const int &idx, const QueryTuple<B> &tuple) override {
            assert(*tuple.getSchema() == this->schema_);

            size_t tuple_offset = idx * this->tuple_size_bytes_;
            memcpy(tuple_data_.data() + tuple_offset, tuple.getData(), this->tuple_size_bytes_);

        }


        std::vector<int8_t> serialize() const override;
        SecureTable *secretShare() override;



        SecretShares generateSecretShares() const override; // generate shares for alice and bob - for data sharing (non-computing) node

        QueryTable<B> &operator=(const QueryTable<B> &src) override;

        static RowTable<B> *deserialize(const QuerySchema &schema, const vector<int8_t> &table_bytes);
        static RowTable<B> *deserialize(const QuerySchema &schema, const vector<Bit> &table_bytes);


        QueryTuple<bool> getPlainTuple(size_t idx) const override;

        // memcpy a field from one table to another
        void assignField(const int & dst_row, const int & dst_col,const  QueryTable<B> *src, const int & src_row, const int & src_col) override;
        void cloneFields(const int &dst_row, const int &dst_start_col, const QueryTable<B> *src, const int &src_row, const int & src_start_col,
                         const int &col_cnt)  override;


        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row)  override;
        void cloneRow(const bool & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row)  override;
        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) override;
        void cloneTable(const int & dst_row, QueryTable<B> *src) override;

        vector<Bit> unpackRow(const int & row, const int & col_cnt, const int & selection_length_bits) const override {
            assert(this->isEncrypted());
            Bit *src = (Bit *) getFieldPtr(row, 0);
            Integer dst(selection_length_bits, 0, PUBLIC);
            memcpy(dst.bits.data(), src, selection_length_bits * TypeUtilities::getEmpBitSize());
            return dst.bits;
        }

        vector<int8_t> unpackRowBytes(const int & row, const int & col_cnt) const override {
           size_t read_len = 0;
           for(int i = 0; i < col_cnt; ++i) read_len += this->field_sizes_bytes_.at(i);

            int8_t *src = getFieldPtr(row, 0);
            vector<int8_t> dst(read_len);
            memcpy(dst.data(), src, read_len);
            return dst;
        }

        QueryTable<B> *clone() override {
            return new RowTable<B>(*this);
        }

        void compareSwap(const bool & swap, const int  & lhs_row, const int & rhs_row)  override;
        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row)  override;
        void compareSwapOmpc(const Bit &swap, const int & lhs_idx, const int & rhs_idx);

        inline virtual StorageModel storageModel() const override { return StorageModel::ROW_STORE; }

        bool operator==(const QueryTable<B> &other) const override;
        string toString(const bool &show_dummies = false) const override;
        string toString(const size_t &limit, const bool &show_dummies = false) const override;

        inline int8_t *getFieldPtr(const int & row, const int & col) const override {
           return  ((int8_t *) tuple_data_.data()) + this->tuple_size_bytes_ * row + this->field_offsets_bytes_.at(col);
        }

    protected:



        string getOstringStream() const override;


    };



}
#endif // _ROW_TABLE_H_
