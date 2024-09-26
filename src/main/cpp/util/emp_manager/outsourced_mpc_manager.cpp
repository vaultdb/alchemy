#include "outsourced_mpc_manager.h"

#ifdef __OMPC_BACKEND__
#include "emp-rescu/emp-rescu.h"
#include <query_table/query_table.h>
#include <util/field_utilities.h>
#include <operators/sort.h>
#include <util/data_utilities.h>

using namespace vaultdb;

void OutsourcedMpcManager::initialize(std::string setup_file) {

    if (party_ == TP) {
        auto settings_raw = DataUtilities::readFile(setup_file);
        assert(settings_raw.size() == (3 + N) * sizeof(block));

        block *settings = (block *) settings_raw.data();
        if(protocol_->prg != nullptr) delete[] protocol_->prg;
        if(protocol_->LPN_prg != nullptr) delete[] protocol_->LPN_prg;

        protocol_->prg = new PRG[N];
        protocol_->LPN_prg = new PRG[N];

        protocol_->lpn_seed = settings[0];
        protocol_->lpn_prg_seed_mask = settings[1];
        protocol_->multi_pack_delta = settings[2];

        for (int i = 0; i < N; ++i) {
            protocol_->deltas[i] = settings[3 + i];
            protocol_->bool_ios[i]->send_block(protocol_->deltas + i, 1);
            protocol_->bool_ios[i]->send_block(&protocol_->lpn_seed, 1);
            protocol_->bool_ios[i]->send_block(&protocol_->lpn_prg_seed_mask, 1);
            protocol_->prg[i].reseed(protocol_->deltas + i); // align seed
            block lpn_prg_seed = protocol_->lpn_prg_seed_mask ^ protocol_->deltas[i];
            protocol_->LPN_prg[i].reseed(&lpn_prg_seed); // align seed
        }
    } else {
        protocol_->bool_io->recv_block(&protocol_->delta, 1);
        protocol_->bool_io->recv_block(&protocol_->lpn_seed, 1);
        protocol_->bool_io->recv_block(&protocol_->lpn_prg_seed_mask, 1);
        protocol_->prg = new PRG(&protocol_->delta);
        block lpn_prg_seed = protocol_->lpn_prg_seed_mask ^ protocol_->delta;
        protocol_->LPN_prg = new PRG(&lpn_prg_seed);
    }
    protocol_->seed_prg.reseed(&protocol_->lpn_seed);
    for (int i = 0; i < N; ++i) {
        protocol_->mac_nn.push_back(nullptr);
        protocol_->key_nn.push_back(nullptr);
    }
    bool true_val = true;
    protocol_->feed(&(protocol_->publicWire), TP, &true_val, 1);
    protocol_->publicAuthShare.tp_share(true, protocol_);
    protocol_->init_lpn(THREAD_N);
    protocol_->cot_ctr = 0;
    protocol_->bigbang = clock_start();
}

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
                SecureField dst_field = SecureField::secret_share_send(src_field, dst_field_desc,  system_conf_.input_party_);
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
