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

            size_t tuple_offset = idx * this->tuple_size_;
            memcpy(tuple_data_.data() + tuple_offset, tuple.getData(), this->tuple_size_);

        }


        std::vector<int8_t> serialize() const override;


        inline Field<B> getField(const int  & row, const int & col)  const override {
            int8_t *read_ptr = getFieldPtr(row, col);
            return Field<B> ::deserialize(this->schema_.getField(col), read_ptr );
        }

        inline Field<B> getPackedField(const int & row, const int & col) const override {
            int8_t *read_ptr = getFieldPtr(row, col);
            return Field<B> ::deserializePacked(this->schema_.getField(col), read_ptr );
        }

        inline void setField(const int  & row, const int & col, const Field<B> & f)  override {
            int8_t *write_ptr = getFieldPtr(row, col);
            f.serialize(write_ptr, this->schema_.getField(col));
        }

        inline void setField(const int  & row, const int & col, const Field<B> & f, const bool & packed)  override {
            int8_t *write_ptr = getFieldPtr(row, col);
            if(packed)
                f.serializePacked(write_ptr, this->schema_.getField(col));
            else
                f.serialize(write_ptr, this->schema_.getField(col));
        }

        inline B getDummyTag(const int & row)  const  override{
            B *tag = (B *) getFieldPtr(row, -1);
            return *tag;
        }

        inline void setDummyTag(const int & row, const B & val)  override {
            B *tag = (B *) getFieldPtr(row, -1);
            *tag = val;
        }

        SecureTable *secretShare() override;



        SecretShares
        generateSecretShares() const override; // generate shares for alice and bob - for data sharing (non-computing) node

        PlainTable *reveal(const int & party = emp::PUBLIC) override;
        PlainTable *revealInsecure(const int & party = emp::PUBLIC) override;

        QueryTable<B> &operator=(const QueryTable<B> &src) override;


        static RowTable<B> *deserialize(const QuerySchema &schema, const vector<int8_t> &table_bytes);


        QueryTuple<bool> getPlainTuple(size_t idx) const override;
//        QueryTuple<Bit> getSecureTuple(size_t idx) const override;

        // memcpy a field from one table to another
        void assignField(const int & dst_row, const int & dst_col,const  QueryTable<B> *src, const int & src_row, const int & src_col) override;
        void cloneFields(const int &dst_row, const int &dst_start_col, const QueryTable<B> *src, const int &src_row, const int & src_start_col,
                         const int &col_cnt)  override;


        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row)  override;
        void cloneRow(const bool & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row)  override;
        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) override;
        void cloneTable(const int & dst_row, QueryTable<B> *src) override;

        QueryTable<B> *clone() override {
            return new RowTable<B>(*this);
        }

        void compareSwap(const bool & swap, const int  & lhs_row, const int & rhs_row)  override;
        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row)  override;

        inline virtual StorageModel storageModel() const override { return StorageModel::ROW_STORE; }

        bool operator==(const QueryTable<B> &other) const override;
        string toString(const bool &show_dummies = false) const override;
        string toString(const size_t &limit, const bool &show_dummies = false) const override;


    protected:




        string getOstringStream() const override;

        inline int8_t *getFieldPtr(const int & row, const int & col) const {
            return (int8_t *) (tuple_data_.data() + this->tuple_size_ * row +  this->field_offsets_bytes_.at(col));
        }

    };



}
#endif // _ROW_TABLE_H_
