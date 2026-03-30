#ifndef _OUTSOURCE_MANAGER_
#define _OUTSOURCE_MANAGER_

#include "emp_manager.h"
#include <util/system_configuration.h>

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

namespace  vaultdb {
    // placeholder to make this build
    class OutsourcedMpcManager : public EmpManager {

    public:

        OutsourcedMpcManager(string hosts[], int party, int comm_port, int ctrl_port) {
            throw;
        }

        OutsourcedMpcManager() {
            throw;
        }

        EmpMode empMode() override {
            return EmpMode::OUTSOURCED;
        }

        void initialize(string setup_file = "") {
            throw;
        }

        size_t andGateCount() const override { return 0; }

        size_t getCommCost() const override { return 0; }

        void  feed(Bit *labels, int party, const bool *b, int bit_cnt) override  {
            throw;
        }

        void flush()  override{ throw; }

        ~OutsourcedMpcManager() = default;

        int sendingParty() const override {
            return TP;
        }

        void setDelta(const block & delta) override {
            throw;
        }

        QueryTable<Bit> *secretShare(const QueryTable<bool> *src) override {
            throw;
        }

        void reveal(bool *dst, const int & party, Bit *src, const int & bit_cnt) override {
            throw;
        }

        string revealToString(const emp::Integer & i, const int & party = PUBLIC)  const override {
            throw;
        }

        size_t getTableCardinality(const int & local_cardinality) override {
            throw;
        }


        void pack(Bit *src, Bit *dst, const int & bit_cnt)  override {  throw; }

        void unpack(Bit *src, Bit *dst, const int & bit_cnt) override { throw; }

        void sendPublic(const int & to_send) override { throw; }

        int recvPublic() override { throw; }



        vector<int8_t> serializePackedWire(OMPCPackedWire & wire) const  {
           throw;
        }

        emp::OMPCPackedWire deserializePackedWire(int8_t *serialized_packed_wire) const {
             throw;
        }

        void serializeBit(const Bit & b, int8_t *dst, bool is_evaluator) const  {
            throw;
        }

        vector<Bit> deserializeBits(int8_t *src, int bit_cnt, bool is_evaluator, bool is_input_party) const  {
            throw;
        }

        int8_t getLambda(const Bit & b) {
            return 0;
        }
    };

}


#else

#include "emp-rescu/emp-rescu.h"
#include <util/utilities.h>
#define __OMPC_BACKEND__ 1
namespace  vaultdb {
    class OutsourcedMpcManager : public EmpManager {

    public:
        emp::NetIO *tpio_ = nullptr;
        vector<NetIO *> ios_;

        emp::NetIO *tpio_ctrl_ = nullptr;
        vector<NetIO *> ios_ctrl_;
        int party_;
        OMPCBackend<N> *protocol_ = nullptr;
        SystemConfiguration & system_conf_ = SystemConfiguration::getInstance();
        EmpMode emp_mode_ = EmpMode::OUTSOURCED;
        int lpn_fraction_ = 1;



        OutsourcedMpcManager(string hosts[], int party, int comm_port, int ctrl_port, int lpn_fraction=1)  : party_(party), lpn_fraction_(lpn_fraction) {
            ios_ = emp::setup_netio(tpio_, hosts, comm_port, party_, N);
            ios_ctrl_ = emp::setup_netio(tpio_ctrl_, hosts, ctrl_port, party_, N);
            emp::backend = new OMPCBackend<N>(ios_, tpio_, party_, lpn_fraction);
            protocol_ = (OMPCBackend<N> *) emp::backend;

            std::this_thread::sleep_for(std::chrono::seconds(1));
            cout << "Memory usage of backend setup: " << Utilities::checkMemoryUtilization(false) << endl;

            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.party_ = party;

        }

        // JMR: we never seem to use this lpn_fraction_ member variable elsewhere.
        OutsourcedMpcManager(string hosts[], int party, int comm_port, int ctrl_port, string backend_state_path, int lpn_fraction=1)  : party_(party), lpn_fraction_(lpn_fraction) {
            ios_ = emp::setup_netio(tpio_, hosts, comm_port, party_, N);
            ios_ctrl_ = emp::setup_netio(tpio_ctrl_, hosts, ctrl_port, party_, N);

            emp::backend = new OMPCBackend<N>(ios_, tpio_, party_, backend_state_path, lpn_fraction);
            protocol_ = (OMPCBackend<N> *) emp::backend;

            std::this_thread::sleep_for(std::chrono::seconds(1));
            cout << "Memory usage of backend setup: " << Utilities::checkMemoryUtilization(false)  << '\n';

            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.party_ = party;
            emp_mode_ = EmpMode::OUTSOURCED;
        }

        EmpMode empMode() override {
            return emp_mode_;
        }

        // setup for plaintext execution
        OutsourcedMpcManager() {
            emp::backend = new ClearPrinter();
            SystemConfiguration & s = SystemConfiguration::getInstance();
            emp_mode_ = EmpMode::PLAIN;

        }

        void initialize(string setup_file = "");

        size_t andGateCount() const override {
           return  ((OMPCBackend<N> *) backend)->num_and();
        }

        size_t getCommCost() const override {
            size_t comm_cost = 0;

            for(int i = 0; i < ios_.size(); ++i) {
                if(ios_[i] != nullptr)
                    comm_cost += ios_[i]->counter;
            }

            if(tpio_ != nullptr)
                comm_cost += tpio_->counter;

            return comm_cost;
        };


        void  feed(Bit *labels, int party, const bool *b, int bit_cnt) override {
            ((OMPCBackend<N> *) backend)->feed(labels, party, b, bit_cnt);
        }

        void flush() override {
            if(tpio_ != nullptr) tpio_->flush();

            for(NetIO *io : ios_) {
                if(io != nullptr) io->flush();
            }

            if(tpio_ctrl_ != nullptr) tpio_ctrl_->flush();
            for(NetIO *io : ios_ctrl_) {
                if(io != nullptr) io->flush();
            }

        }

        void setDelta(const block & delta) override {
            ((OMPCBackend<N> *) emp::backend)->multi_pack_delta = delta;
        }


        ~OutsourcedMpcManager() {
            delete emp::backend;
            if(tpio_ != nullptr) delete_netio(tpio_, ios_, party_);
            if(tpio_ctrl_ != nullptr) delete_netio(tpio_ctrl_, ios_ctrl_, party_);
        }

        QueryTable<Bit> *secretShare(const QueryTable<bool> *src) override;

        void reveal(bool *dst, const int & party, Bit *src, const int & bit_cnt) override {
            ((OMPCBackend<N> *) emp::backend)->reveal(dst,party,src,bit_cnt);
        }

        string revealToString(const emp::Integer & in, const int & party = PUBLIC)  const override {
            bool *b = new bool[in.size()];
            in.reveal(b, party);
            stringstream s;
            for(int i = 0; i < in.size(); ++i) {
                if(i % 8 == 0) s << " ";
                char res =   (b[i]) ? '1' : '0';
                s << res;
            }

            delete [] b;
           return s.str();
        }

        size_t getTableCardinality(const int & local_cardinality) override;

        int sendingParty() const override {
            return TP;
        }

        void pack(Bit *src, Bit *dst, const int & bit_cnt)  override {
            protocol_->pack(src, *((OMPCPackedWire *) dst), bit_cnt);

        }

        void unpack(Bit *src, Bit *dst, const int & bit_cnt) override {

            protocol_->unpack(dst, *((OMPCPackedWire *) src), bit_cnt);
        }

        void sendPublic(const int & to_send) override {
            tpio_ctrl_->send_data(&to_send, 4);
            tpio_ctrl_->flush();
        }

        int recvPublic() override {
            int to_recv;
            tpio_ctrl_->recv_data(&to_recv, 4);
            tpio_ctrl_->flush();
            return to_recv;
        }


        //         int page_size_bits_ = system_conf_.bpm_.page_size_bytes_ / sizeof(emp::Bit);
        // int block_n_ = (page_size_bits_ / 128) + (page_size_bits_ % 128 != 0);

        // int packed_wire_size_bytes_ = (2 * block_n_ + 1) * sizeof(block); // 16 bytes per block = 128 bits

        vector<int8_t> serializePackedWire(OMPCPackedWire &wire) const;

        emp::OMPCPackedWire deserializePackedWire(int8_t *serialized_packed_wire) const;


        void serializeBit(const Bit & b, int8_t *dst, bool is_evaluator) const  {

            int8_t *write_cursor = dst;
            memcpy(write_cursor, (int8_t *) &b.bit.auth.mac, emp::N * sizeof(emp::block));
            write_cursor += emp::N * sizeof(emp::block);

            memcpy(write_cursor, (int8_t *) &b.bit.auth.key, emp::N * sizeof(emp::block));
            write_cursor += emp::N * sizeof(emp::block);

            memcpy(write_cursor, (int8_t *) &b.bit.auth.lambda, 1);
            ++write_cursor;

            if(is_evaluator) {
                memcpy(write_cursor, (int8_t *) &b.bit.masked_value, 1);
                ++write_cursor;
            }
        }

        vector<Bit> deserializeBits(int8_t *src, int dst_bit_cnt, bool is_evaluator, bool is_input_party) const  {


            auto *auth_shares = new AuthShare<emp::N>[dst_bit_cnt];
            bool *masked_values = new bool[dst_bit_cnt];

            int8_t *read_cursor = src;


            for(size_t i = 0; i < dst_bit_cnt; ++i) {
                AuthShare<emp::N> cur_auth_share;

                if (!is_input_party) {
                    memcpy((int8_t *) cur_auth_share.mac, read_cursor, emp::N * sizeof(emp::block));
                    read_cursor += emp::N * sizeof(emp::block);
                    memcpy((int8_t *) cur_auth_share.key, read_cursor, emp::N * sizeof(emp::block));
                    read_cursor += emp::N * sizeof(emp::block);
                }

                // all parties read lambda
                int8_t cur_lambda = 0;
                memcpy(&cur_lambda, read_cursor, 1);
                ++read_cursor;
                cur_auth_share.lambda = ((cur_lambda & 1) != 0);

                auth_shares[i] = cur_auth_share;

                // alice only reads masked value
                if(is_evaluator) {
                    int8_t cur_masked_value = *read_cursor;
                    ++read_cursor;
                    masked_values[i] = ((cur_masked_value & 1) != 0);
                }
            }

            Integer dst_int(dst_bit_cnt, 0L, emp::PUBLIC);
            protocol_->regen_label(dst_int.bits.data(), masked_values, auth_shares, dst_bit_cnt);

            delete [] auth_shares;
            delete [] masked_values;

            return dst_int.bits;
        }

        int8_t getLambda(const Bit & b) {
            return b.bit.auth.lambda;
        }

    };
}

#endif // end if emp-sh2pc||emp-zk

#endif // end OUTSOURCE_MANAGER