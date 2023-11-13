#ifndef _PACKED_COLUMN_TABLE_H_
#define _PACKED_COLUMN_TABLE_H_
#include "query_table.h"

// only supports Bit for now
namespace vaultdb {
    class PackedColumnTable : public QueryTable<Bit> {

    public:


        // choosing to branch instead of storing this in a float for now, need to analyze trade-off on this one
        // maps ordinal to wire count.  floor(128/field_size_bits)
        map<int, int> fields_per_wire_;
        // if a field spans more than one wire per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple wires
        map<int, int> wires_per_field_;

        // cache of last wire we unpacked for each column
        typedef struct unpacked_t {
            emp::Bit unpacked_wire[128];
            int page_idx_;
            bool dirty_bit_ = false; // TODO: if dirty, flush this to column_data_ when we evict an entry from unpacked_wires_
        }  Unpacked;

        map<int, Unpacked> unpacked_wires_;


        PackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
            setSchema(schema);

            if(tuple_cnt == 0)
                return;


            for(int i = 0; i < schema_.fields_.size(); ++i) {
                auto desc = schema_.fields_.at(i);
                int packed_wires = desc.packedWires();

                // solve for col_len_bytes
                int wire_cnt =  (packed_wires == 1) ? (tuple_cnt_ / fields_per_wire_[i] + (tuple_cnt_ % fields_per_wire_[i] != 0))
                                                   : (tuple_cnt_ * packed_wires);
                 column_data_[i] = std::vector<int8_t>(wire_cnt * sizeof(OMPCPackedWire));

            }

            int dummy_tag_wire_cnt = tuple_cnt / 128 + (tuple_cnt % 128 != 0);
            column_data_[-1] = std::vector<int8_t>(dummy_tag_wire_cnt * sizeof(OMPCPackedWire));

            emp::Bit d(true);
            emp::Bit dummies[128];
            for(int i = 0; i < 128; ++i) {
                dummies[i] = d;
            }
            OMPCBackend<N> *protocol = (OMPCBackend<N> *) emp::backend;
            OMPCPackedWire w;
            protocol->pack(dummies, &w, 128);

            OMPCPackedWire *cursor = (OMPCPackedWire *) column_data_[-1].data();
            for(int i = 0; i < dummy_tag_wire_cnt; ++i) {
                memcpy(cursor, &w, sizeof(OMPCPackedWire));
                ++cursor;
            }

        }

        PackedColumnTable(const PackedColumnTable &src) : QueryTable<Bit>(src) {
            setSchema(src.schema_);
        }

        Field<Bit> getField(const int  & row, const int & col)  const override {
            throw;
//            int8_t *read_ptr = getFieldPtr(row, col);
            // TODO: cache latest packed wire
//            return Field<Bit>::deserialize(this->schema_.fields_.at(col), read_ptr );
            return Field<Bit>();
        }


        void setField(const int  & row, const int & col, const Field<Bit> & f)  override {
//            int8_t *write_ptr = getFieldPtr(row, col);
            // TODO: update column cache and write as needed
//            f.serialize(write_ptr, this->schema_.fields_.at(col));

        }

        void appendColumn(const QueryFieldDesc & desc) override {
            // NYI
            throw;
        }

        void resize(const size_t &tuple_cnt) override {
            // NYI
            throw;
        }

        Bit getDummyTag(const int & row)  const override {
            // NYI
            return Bit();
        }

        void setDummyTag(const int & row, const Bit & val) override {
            // NYI
        }


        QueryTable<Bit> *clone()  override {
            throw;
            return new PackedColumnTable(*this);
        }

        void setSchema(const QuerySchema &schema) override {
            schema_ = schema;

            tuple_size_bytes_ = 0;
            QueryFieldDesc desc;
            int field_size_bytes;
            int running_count = 0;

            // covers dummy tag as -1
            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                auto desc = schema_.fields_.at(i);
                int packed_wires = desc.packedWires();
                field_size_bytes = desc.packedWires() * sizeof(emp::OMPCPackedWire);
                tuple_size_bytes_ += field_size_bytes;
                field_sizes_bytes_[i] = field_size_bytes;
                running_count += field_size_bytes;
                if(packed_wires == 1) {
                    fields_per_wire_[i] = 128 / desc.size();
                    wires_per_field_[i] = 0;
                    int wire_cnt = tuple_cnt_ / fields_per_wire_[i] + (tuple_cnt_ % fields_per_wire_[i] != 0);
                    column_data_[i] = std::vector<int8_t>(wire_cnt * sizeof(OMPCPackedWire));
                }
                else {
                    wires_per_field_[i] = packed_wires;
                    fields_per_wire_[i] = 0;
                    int wire_cnt = tuple_cnt_ * packed_wires;
                    column_data_[i] = std::vector<int8_t>(wire_cnt * sizeof(OMPCPackedWire));
                }
            }

        }

        QueryTuple<Bit> getRow(const int & idx) override {
            throw;
            // NYI
            return QueryTuple<Bit>();
        }
        void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {
            throw;
            // NYI
        }


        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) override {
            throw;
            // NYI
        }

        void cloneTable(const int & dst_row, QueryTable<Bit> *src) override {
            throw;
            // NYI
        }

        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<Bit> * src, const int & src_row) override {
            throw;
            // NYI, make sure to include dummy tag in cloning
        }

        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row) override {
            throw;
            // NYI, make sure to include dummy tag in cloning
        }

        void cloneColumn(const int & dst_col, const int & dst_idx, const QueryTable<Bit> *s, const int & src_col, const int & src_offset = 0) override {
            throw;
            // NYI
        }

        StorageModel storageModel() const override { return StorageModel::PACKED_COLUMN_STORE; }

    private:
        // index of the wire in the column, might move this up
        int wireOffset(int row, int col) const {
            if(fields_per_wire_.at(col) > 0) {
                return row / fields_per_wire_.at(col);
            }
            else
                return row * wires_per_field_.at(col);
        }

        };
}
#endif
