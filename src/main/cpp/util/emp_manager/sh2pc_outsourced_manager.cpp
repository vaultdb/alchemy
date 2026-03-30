#include "sh2pc_outsourced_manager.h"

#if __has_include("emp-sh2pc/emp-sh2pc.h")
#include <emp-sh2pc/emp-sh2pc.h>
#include <query_table/query_table.h>
#include <util/field_utilities.h>
#include <operators/sort.h>

using namespace vaultdb;

size_t SH2PCOutsourcedManager::getTableCardinality(const int &local_cardinality) {
    int tuple_cnt = local_cardinality;
    if (party_ == ALICE) {
        netio_->send_data(&tuple_cnt, 4);
        netio_->flush();
    } else if (party_ == BOB) {
        netio_->recv_data(&tuple_cnt, 4);
        netio_->flush();
    }

    return tuple_cnt;

}


QueryTable<Bit> *SH2PCOutsourcedManager::secretShare(const QueryTable<bool> *src) {
//    size_t local_tuple_cnt =  src->tuple_cnt_;
//
//    if (party_ == ALICE) {
//        netio_->send_data(&alice_tuple_cnt, 4);
//        netio_->flush();
//        netio_->recv_data(&bob_tuple_cnt, 4);
//        netio_->flush();
//    } else if (party_ == BOB) {
//        netio_->recv_data(&alice_tuple_cnt, 4);
//        netio_->flush();
//        netio_->send_data(&bob_tuple_cnt, 4);
//        netio_->flush();
//    }

    int tuple_cnt = getTableCardinality(src->tuple_cnt_);
    QuerySchema dst_schema = QuerySchema::toSecure(src->getSchema());

    auto dst_table = QueryTable<Bit>::getTable(tuple_cnt, dst_schema, src->order_by_);

    if(tuple_cnt > 0) {
        if (party_ == emp::ALICE) {
            secret_share_send(emp::ALICE, src, dst_table, 0, false);
        } else { // bob
            secret_share_recv(tuple_cnt, emp::ALICE, dst_table, 0, false);
        }
    }
    netio_->flush();

    return dst_table;


}

void SH2PCOutsourcedManager::secret_share_recv(const size_t &tuple_count, const int &party,
                                    SecureTable *dst_table, const size_t &write_offset,
                                    const bool &reverse_read_order)  {

    int32_t cursor = (int32_t) write_offset;
    auto dst_schema = dst_table->getSchema();
    auto src_schema = QuerySchema::toPlain(dst_schema);

    PlainTuple src_tuple(&src_schema);

    if(reverse_read_order) {

        for(int32_t i = tuple_count - 1; i >= 0; --i) {
            for(int j = 0; j < dst_table->getSchema().getFieldCount(); ++j) {
                PlainField placeholder = src_tuple.getField(j);
                auto field_desc = dst_schema.getField(j);
                auto dst_field = SecureField::secretShareHelper(placeholder, field_desc, party, false);
                dst_table->setField(cursor, j, dst_field);
            }

            emp::Bit b(0, party);
            dst_table->setDummyTag(cursor, b);
            ++cursor;
        }
        return;
    }

    // else
    for(size_t i = 0; i < tuple_count; ++i) {
        for(int j = 0; j < dst_table->getSchema().getFieldCount(); ++j) {
            PlainField placeholder = src_tuple.getField(j);
            auto field_desc = dst_schema.getField(j);
            auto dst_field = SecureField::secretShareHelper(placeholder, field_desc, party, false);
            dst_table->setField(cursor, j, dst_field);
        }

        emp::Bit b(0, party);
        dst_table->setDummyTag(cursor, b);
        ++cursor;
    }

}






void
SH2PCOutsourcedManager::secret_share_send(const int &party,const PlainTable *src_table, SecureTable *dst_table, const int &write_offset,
                               const bool &reverse_read_order)  {

    int cursor = (int) write_offset;
    auto src_schema = src_table->getSchema();
    auto dst_schema = dst_table->getSchema();


    if(reverse_read_order) {
        for(int i = src_table->tuple_cnt_ - 1; i >= 0; --i) {
            for(int j = 0; j < src_table->getSchema().getFieldCount(); ++j) {
                auto src_field = src_table->getField(i, j);
                auto field_desc = dst_schema.getField(j);

                auto dst_field = SecureField::secretShareHelper(src_field, field_desc, party, true);
                dst_table->setField(cursor, j, dst_field);
            }
            emp::Bit b(src_table->getDummyTag(i), party);
            dst_table->setDummyTag(cursor, b);

            ++cursor;
        }
        return; // end reverse order
    }

    // else
    for(size_t i = 0; i < src_table->tuple_cnt_; ++i) {
        for(int j = 0; j < src_table->getSchema().getFieldCount(); ++j) {
            auto src_field = src_table->getField(i, j);
            auto field_desc = dst_schema.getField(j);
            auto dst_field = SecureField::secretShareHelper(src_field, field_desc, party, true);
            dst_table->setField(cursor, j, dst_field);
        }

        emp::Bit b(src_table->getDummyTag(i), party);
        dst_table->setDummyTag(cursor, b);
        ++cursor;
    }

}



#endif
