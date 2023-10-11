#include "sh2pc_manager.h"

#if __has_include("emp-sh2pc/emp-sh2pc.h")
#include <emp-sh2pc/emp-sh2pc.h>
#include <query_table/query_table.h>
#include <query_table/table_factory.h>
#include <util/field_utilities.h>
#include <operators/sort.h>

using namespace vaultdb;

size_t SH2PCManager::getTableCardinality(const int &local_cardinality) {
    int alice_tuple_cnt = local_cardinality;
    int bob_tuple_cnt = local_cardinality;
    if (party_ == ALICE) {
        netio_->send_data(&alice_tuple_cnt, 4);
        netio_->flush();
        netio_->recv_data(&bob_tuple_cnt, 4);
        netio_->flush();
    } else if (party_ == BOB) {
        netio_->recv_data(&alice_tuple_cnt, 4);
        netio_->flush();
        netio_->send_data(&bob_tuple_cnt, 4);
        netio_->flush();
    }

    return alice_tuple_cnt + bob_tuple_cnt;

}


QueryTable<Bit> *SH2PCManager::secretShare(const QueryTable<bool> *src) {
    size_t alice_tuple_cnt =  src->getTupleCount();
    size_t bob_tuple_cnt = alice_tuple_cnt;

    if (party_ == ALICE) {
        netio_->send_data(&alice_tuple_cnt, 4);
        netio_->flush();
        netio_->recv_data(&bob_tuple_cnt, 4);
        netio_->flush();
    } else if (party_ == BOB) {
        netio_->recv_data(&alice_tuple_cnt, 4);
        netio_->flush();
        netio_->send_data(&bob_tuple_cnt, 4);
        netio_->flush();
    }


    QuerySchema dst_schema = QuerySchema::toSecure(src->getSchema());

    auto dst_table =
           TableFactory<Bit>::getTable(alice_tuple_cnt + bob_tuple_cnt, dst_schema, src->getSortOrder());

    if(!src->getSortOrder().empty()) {
        if (party_ == emp::ALICE) {
            if(alice_tuple_cnt > 0) secret_share_send(emp::ALICE, src, dst_table, 0, (bob_tuple_cnt > 0));
            if(bob_tuple_cnt > 0) secret_share_recv(bob_tuple_cnt, emp::BOB, dst_table, alice_tuple_cnt, false);

        } else { // bob
            if(alice_tuple_cnt > 0) secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table, 0, (bob_tuple_cnt > 0));
            if(bob_tuple_cnt > 0)  secret_share_send(emp::BOB, src, dst_table, alice_tuple_cnt, false);
        }


        int counter = 0;
        // if one is empty, then we are already sorted
        if(alice_tuple_cnt > 0 && bob_tuple_cnt > 0) {

            auto dst_sort = dst_table->getSortOrder();
            Sort<Bit> sorter(dst_table, dst_sort);
            auto normalized = sorter.normalizeTable(dst_table);

            sorter.bitonicMerge(normalized, sorter.getSortOrder(), 0, normalized->getTupleCount(), true, counter);
            dst_table = sorter.denormalizeTable(normalized);
            delete normalized;
            dst_table->setSortOrder(dst_sort);

//            float n = dst_table->getTupleCount();
//            float rounds = log2(dst_table->getTupleCount());
//
//            float comparisons_per_stage = n / 2;
//            float total_comparisons = rounds * comparisons_per_stage;
//            float relative_error = fabs(total_comparisons - counter) / counter;
//            auto gate_cnt = this->andGateCount();
//            cout << "Estimated comparisons: " << total_comparisons << ", observed=" << counter << ", relative error="
//                 << relative_error << endl;
//            cout << "Gate count: " << gate_cnt << ", avg cost per comparison: " << gate_cnt / total_comparisons << endl;
        }
    }
    else { // concatenate Alice and Bob
        if (party_ == emp::ALICE) {
            if(alice_tuple_cnt > 0) secret_share_send(emp::ALICE, src, dst_table, 0, false);
            if(bob_tuple_cnt > 0)  secret_share_recv(bob_tuple_cnt, emp::BOB, dst_table, alice_tuple_cnt, false);
        } else { // bob
            if(alice_tuple_cnt > 0) secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table, 0, false);
            if(bob_tuple_cnt > 0)  secret_share_send(emp::BOB, src, dst_table, alice_tuple_cnt, false);
        }
    }
    netio_->flush();

    return dst_table;


}

void SH2PCManager::secret_share_recv(const size_t &tuple_count, const int &party,
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
                dst_table->setPackedField(cursor, j, dst_field);
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
            dst_table->setPackedField(cursor, j, dst_field);
        }

        emp::Bit b(0, party);
        dst_table->setDummyTag(cursor, b);
        ++cursor;
    }

}






void
SH2PCManager::secret_share_send(const int &party,const PlainTable *src_table, SecureTable *dst_table, const int &write_offset,
                               const bool &reverse_read_order)  {

    int cursor = (int) write_offset;
    auto src_schema = src_table->getSchema();
    auto dst_schema = dst_table->getSchema();


    if(reverse_read_order) {
        for(int i = src_table->getTupleCount() - 1; i >= 0; --i) {
            for(int j = 0; j < src_table->getSchema().getFieldCount(); ++j) {
                auto src_field = src_table->getField(i, j);
                auto field_desc = dst_schema.getField(j);

                auto dst_field = SecureField::secretShareHelper(src_field, field_desc, party, true);
                dst_table->setPackedField(cursor, j, dst_field);
            }
            emp::Bit b(src_table->getDummyTag(i), party);
            dst_table->setDummyTag(cursor, b);

            ++cursor;
        }
        return; // end reverse order
    }

    // else
    for(size_t i = 0; i < src_table->getTupleCount(); ++i) {
        for(int j = 0; j < src_table->getSchema().getFieldCount(); ++j) {
            auto src_field = src_table->getField(i, j);
            auto field_desc = dst_schema.getField(j);
            auto dst_field = SecureField::secretShareHelper(src_field, field_desc, party, true);
            dst_table->setPackedField(cursor, j, dst_field);
        }

        emp::Bit b(src_table->getDummyTag(i), party);
        dst_table->setDummyTag(cursor, b);
        ++cursor;
    }

}



#endif
