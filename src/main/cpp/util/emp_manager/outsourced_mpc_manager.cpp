#include "outsourced_mpc_manager.h"

#ifdef __OMPC_BACKEND__
#include "emp-rescu/emp-rescu.h"
#include <query_table/query_table.h>
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
    int tuple_cnt = src->tuple_cnt_;

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

    QuerySchema plain_schema = src->getSchema();
    QuerySchema dst_schema = QuerySchema::toSecure(plain_schema);
    QueryTable<Bit> *dst = QueryTable<Bit>::getTable(tuple_cnt, dst_schema, src->order_by_);
    if(tuple_cnt == 0) return dst;

    if(party_ == system_conf_.input_party_) {
        for(int j = -1; j < dst_schema.getFieldCount(); ++j) {
            QueryFieldDesc dst_field_desc = dst_schema.getField(j);
            for (int i = 0; i < tuple_cnt; ++i) {

                PlainField src_field = src->getField(i, j);
                SecureField dst_field = SecureField::secret_share_send(src_field, dst_field_desc,
                                                                       system_conf_.input_party_);
                dst->setField(i, j, dst_field);
            }
        }
    }
    else { // computing parties
        for(int j = -1; j < dst_schema.getFieldCount(); ++j) {
            QueryFieldDesc dst_field_desc = dst_schema.getField(j);
            for(int i = 0; i < tuple_cnt; ++i) {
                SecureField dst_field = SecureField::secret_share_recv(dst_field_desc, system_conf_.input_party_);
                dst->setField(i, j, dst_field);
            }
        }
    }

    return dst;

}



#endif
