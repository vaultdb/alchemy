#ifndef _COLUMN_TABLE_H_
#define _COLUMN_TABLE_H_


#include "query_schema.h"
#include "query_tuple.h"
#include "query_table.h"
#include <memory>
#include <ostream>
#include "util/utilities.h"
#include "plain_tuple.h"
#include "query_table.h"


namespace  vaultdb {



    template<typename B> class QueryTable;
    typedef QueryTable<bool> PlainTable;
    typedef QueryTable<emp::Bit> SecureTable;



    template<typename B>
    class ColumnTable : public QueryTable<B> {



    public:
        std::map<int, std::vector<int8_t> > column_data_;

        // empty sort definition for default case
        ColumnTable(const size_t &tuple_cnt, const QuerySchema &schema,
                 const SortDefinition &sort_def = SortDefinition());

        // deep copy
        explicit ColumnTable(const QueryTable<B> &src);

        virtual ~ColumnTable() {}

        void resize(const size_t &tuple_cnt) override;



        void putTuple(const int &idx, const QueryTuple<B> &tuple) override {
            assert(*tuple.getSchema() == this->schema_);

            int8_t *write_ptr;
            int write_size;
            int8_t *read_ptr = (int8_t *) tuple.getData();

            for(int i = 0; i < this->schema_.getFieldCount(); ++i) {
                write_ptr = column_data_[i].data() + idx * this->field_sizes_bytes_[i];
                write_size = this->field_sizes_bytes_[i];
                memcpy(write_ptr, read_ptr, write_size);
                read_ptr += write_size;
            }
            write_size =  this->field_sizes_bytes_[-1];
            write_ptr = column_data_[-1].data() +  write_size * idx;
            memcpy(write_ptr, read_ptr, write_size);

        }


        std::vector<int8_t> serialize() const override;


        inline Field<B> getField(const int  & row, const int & col)  const override {
            int8_t *read_ptr = getFieldPtr(row, col);
            return Field<B> ::deserialize(this->schema_.getField(col), read_ptr);
        }

        inline Field<B> getPackedField(const int & row, const int & col) const override {
            int8_t *read_ptr = getFieldPtr(row, col);
            if(SystemConfiguration::getInstance().bitPackingEnabled())
                return Field<B> ::deserializePacked(this->schema_.getField(col), read_ptr );

            return Field<B> ::deserialize(this->schema_.getField(col), read_ptr);
        }

        inline void setField(const int  & row, const int & col, const Field<B> & f)  override {
            int8_t *write_ptr = getFieldPtr(row, col);
            f.serialize(write_ptr, this->schema_.getField(col));
        }

        inline void setPackedField(const int  & row, const int & col, const Field<B> & f)  override {
            int8_t *write_ptr = getFieldPtr(row, col);
            if(SystemConfiguration::getInstance().bitPackingEnabled()) {
                f.serializePacked(write_ptr, this->schema_.getField(col));
                return;
            }

            f.serialize(write_ptr, this->schema_.getField(col));
        }



        inline int8_t *getFieldPtr(const int & row, const int & col) const override {
            return const_cast<int8_t *>(column_data_.at(col).data() + row * this->field_sizes_bytes_.at(col));
        }

        SecureTable *secretShare() override;


        SecretShares
        generateSecretShares() const override; // generate shares for alice and bob - for data sharing (non-computing) node

        PlainTable *reveal(const int & party = emp::PUBLIC) override;
        PlainTable *revealInsecure(const int & party = emp::PUBLIC) const override;


        QueryTable<B> &operator=(const QueryTable<B> &src) override;


        static ColumnTable<B> *deserialize(const QuerySchema &schema, const vector<int8_t> &table_bytes);


        QueryTuple<bool> getPlainTuple(size_t idx) const override;
        //QueryTuple<Bit> getSecureTuple(size_t idx) const override;

        // memcpy a field from one table to another
        void assignField(const int & dst_row, const int & dst_col,const  QueryTable<B> *src, const int & src_row, const int & src_col) override;
        // conditional write
        void assignField(const emp::Bit & write, const int & dst_row, const int & dst_col,const  QueryTable<B> *src, const int & src_row, const int & src_col);

        void cloneFields(const int &dst_row, const int &dst_start_col, const QueryTable<B> *src, const int &src_row, const int & src_start_col,
                         const int &col_cnt)  override;



        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row)  override;
        void cloneRow(const bool & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row)  override;
        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) override;
        void cloneTable(const int & dst_row, QueryTable<B> *src) override;

        QueryTable<B> *clone() override {
            return new ColumnTable<B>(*this);
        }

        void compareSwap(const bool & swap, const int  & lhs_row, const int & rhs_row)  override;
        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row)  override;

        inline virtual StorageModel storageModel() const override { return StorageModel::COLUMN_STORE; }

        bool operator==(const QueryTable<B> &other) const override;
        string toString(const bool &show_dummies = false) const override;
        string toString(const size_t &limit, const bool &show_dummies = false) const override;

    protected:



        string getOstringStream() const override;


    };



}

#endif // _COLUMN_TABLE_H_
