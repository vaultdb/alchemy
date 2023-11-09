#include "zk_manager.h"
#include "query_table/query_table.h"
#include "util/field_utilities.h"

#if __has_include("emp-zk/emp-zk.h")
using namespace vaultdb;

size_t ZKManager::getTableCardinality(const int &local_cardinality) {
    size_t alice_tuple_cnt = local_cardinality;
    SystemConfiguration &s = SystemConfiguration::getInstance();
    int party = s.party_;


   this->flush();

    NetIO *netio = ios_[0]->io;

    if (party == ALICE) {
        netio->send_data(&alice_tuple_cnt, 4);
        netio->flush();
    } else if (party == BOB) {
        netio->recv_data(&alice_tuple_cnt, 4);
        netio->flush();
    }

    return alice_tuple_cnt;

}

QueryTable<Bit> *ZKManager::secretShare(const QueryTable<bool> *src) {

    size_t alice_tuple_cnt = src->tuple_cnt_;
    SystemConfiguration &s = SystemConfiguration::getInstance();
    int party = s.party_;


    // reset before we send the counts
    for(size_t i = 0; i < threads_; ++i) {
        ios_[i]->flush();
    }

    NetIO *netio = ios_[0]->io;

    if (party == ALICE) {
        netio->send_data(&alice_tuple_cnt, 4);
        netio->flush();
    } else if (party == BOB) {
        netio->recv_data(&alice_tuple_cnt, 4);
        netio->flush();
    }

    // zero rows is needed for MergeJoin
    //assert(alice_tuple_cnt > 0);

    QuerySchema dst_schema = QuerySchema::toSecure(src->getSchema());
    SecureTable *dst_table = new SecureTable(alice_tuple_cnt, dst_schema, src->order_by_);

    if (party == emp::ALICE) {
        secret_share_send(emp::ALICE,  (PlainTable *) src, dst_table);
    } else { // bob
        secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table);
    }

   flush();
    return dst_table;

}

void ZKManager::secret_share_recv(const size_t &tuple_count, const int &dst_party,
                                     SecureTable *dst_table)  {

    int32_t cursor = 0;

    for(size_t i = 0; i < tuple_count; ++i) {
        FieldUtilities::secret_share_recv(dst_table, cursor, dst_party);
        ++cursor;
    }



}



void
ZKManager::secret_share_send(const int &party,const PlainTable *src_table, SecureTable *dst_table)  {

    int cursor = 0;

    for(size_t i = 0; i < src_table->tuple_cnt_; ++i) {
        FieldUtilities::secret_share_send(src_table, i, dst_table,  cursor, party);
        ++cursor;
    }

}



#endif