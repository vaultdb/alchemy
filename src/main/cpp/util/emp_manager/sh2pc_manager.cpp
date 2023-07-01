#include "sh2pc_manager.h"

#if __has_include("emp-sh2pc/emp-sh2pc.h")
#include <emp-sh2pc/emp-sh2pc.h>
#include <query_table/query_table.h>
#include <query_table/table_factory.h>
#include <util/field_utilities.h>
#include <operators/sort.h>

using namespace vaultdb;

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
           TableFactory<Bit>::getTable(alice_tuple_cnt + bob_tuple_cnt, dst_schema, src->storageModel(), src->getSortOrder());

    if(!src->getSortOrder().empty()) {
        if (party_ == emp::ALICE) {
            if(alice_tuple_cnt > 0) secret_share_send(emp::ALICE, src, dst_table, 0, true);
            if(bob_tuple_cnt > 0) secret_share_recv(bob_tuple_cnt, emp::BOB, dst_table, alice_tuple_cnt, false);

        } else { // bob
            if(alice_tuple_cnt > 0) secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table, 0, true);
            if(bob_tuple_cnt > 0)  secret_share_send(emp::BOB, src, dst_table, alice_tuple_cnt, false);
        }
        int counter = 0;
        Sort<emp::Bit>::bitonicMerge(dst_table, dst_table->getSortOrder(), 0, dst_table->getTupleCount(), true, counter);
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

void SH2PCManager::secret_share_recv(const size_t &tuple_count, const int &dst_party,
                                    SecureTable *dst_table, const size_t &write_offset,
                                    const bool &reverse_read_order)  {

    int32_t cursor = (int32_t) write_offset;

    if(reverse_read_order) {

        for(int32_t i = tuple_count - 1; i >= 0; --i) {
            FieldUtilities::secret_share_recv(dst_table, cursor, dst_party);
            ++cursor;
        }



        return;
    }

    // else
    for(size_t i = 0; i < tuple_count; ++i) {
        FieldUtilities::secret_share_recv(dst_table, cursor, dst_party);
        ++cursor;
    }



}



void
SH2PCManager::secret_share_send(const int &party,const PlainTable *src_table, SecureTable *dst_table, const int &write_offset,
                               const bool &reverse_read_order)  {

    int cursor = (int) write_offset;

    if(reverse_read_order) {
        for(int i = src_table->getTupleCount() - 1; i >= 0; --i) {
            FieldUtilities::secret_share_send(src_table, i, dst_table,  cursor, party);
            ++cursor;
        }

        return;

    }

    // else
    for(size_t i = 0; i < src_table->getTupleCount(); ++i) {
        FieldUtilities::secret_share_send(src_table, i, dst_table,  cursor, party);
        ++cursor;
    }

}



#endif
