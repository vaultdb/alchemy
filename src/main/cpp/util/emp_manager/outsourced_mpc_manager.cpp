#include "outsourced_mpc_manager.h"

#ifdef __OMPC_BACKEND__
#include "emp-rescu/emp-rescu.h"
#include <query_table/query_table.h>
#include <query_table/table_factory.h>
#include <util/field_utilities.h>
#include <operators/sort.h>

using namespace vaultdb;

size_t OutsourcedMpcManager::getTableCardinality(const int & local_cardinality)  {
    int tuple_cnt = local_cardinality;
    if(party_ == emp::TP) {
        for(int i = 0; i < emp::N; ++i) {
            ios_ctrl_[i]->send_data(&tuple_cnt, 4);
            ios_ctrl_[i]->flush();
        }

    }
    else {
        tpio_ctrl_->recv_data(&tuple_cnt, 4);
        tpio_ctrl_->flush();
    }

    return tuple_cnt;
}

QueryTable<Bit> *OutsourcedMpcManager::secretShare(const QueryTable<bool> *src) {
    int tuple_cnt = src->getTupleCount();

    if(party_ == emp::TP) {
        for(int i = 0; i < emp::N; ++i) {
            ios_ctrl_[i]->send_data(&tuple_cnt, 4);
            ios_ctrl_[i]->flush();
        }
    }
    else {
        tpio_ctrl_->recv_data(&tuple_cnt, 4);
        tpio_ctrl_->flush();
    }

    if(tuple_cnt == 0) throw std::invalid_argument("No tuples to process!");
    QuerySchema plain_schema = src->getSchema();
    QuerySchema dst_schema = QuerySchema::toSecure(plain_schema);
    QueryTable<Bit> *dst = TableFactory<Bit>::getTable(tuple_cnt, dst_schema, src->storageModel(), src->getSortOrder());

    if(party_ == emp::TP) {
        for(int i = 0; i < tuple_cnt; ++i) {
            FieldUtilities::secret_share_send(src, i, dst,  i, TP);
        }
    }
    else { // computing parties
        for(int i = 0; i < tuple_cnt; ++i) {
            FieldUtilities::secret_share_recv(dst, i, TP);

        }
    }

    return dst;

}



#endif
