#ifndef _OUTSOURCE_MANAGER_
#define _OUTSOURCE_MANAGER_

#include "emp_manager.h"
#include <util/system_configuration.h>

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/zk.h")
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

        int sendingParty() const override {
            throw;
        }

        size_t andGateCount() const override { return 0; }

        size_t getCommCost() const override { return 0; }

        void  feed(Bit *labels, int party, const bool *b, int bit_cnt) override  {
            throw;
        }

        void flush()  override{ throw; }

        ~OutsourcedMpcManager() = default;

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


    };
}

#else

#include <emp-rescu/emp-rescu.h>
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
        bool wire_packing_ = false;

        OutsourcedMpcManager(string hosts[], int party, int comm_port, int ctrl_port)  : party_(party) {
            ios_ = emp::setup_netio(tpio_, hosts, comm_port, party_, N);
            ios_ctrl_ = emp::setup_netio(tpio_ctrl_, hosts, ctrl_port, party_, N);
            emp::backend = new OMPCBackend<N>(ios_, tpio_, party_);
            protocol_ = (OMPCBackend<N> *) emp::backend;
            SystemConfiguration & s = SystemConfiguration::getInstance();
            wire_packing_ =  (system_conf_.storageModel() == StorageModel::PACKED_COLUMN_STORE);
            s.emp_bit_size_bytes_  = wire_packing_ ? sizeof(OMPCPackedWire) : sizeof(emp::Bit);
            s.party_ = party;
            s.emp_mode_ = EmpMode::OUTSOURCED;
        }

        // setup for plaintext execution
        OutsourcedMpcManager() {
            emp::backend = new ClearPrinter();
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.emp_mode_ = EmpMode::PLAIN;

        }

        int sendingParty() const override {
            return TP;
        }

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
            in.revealBools(b, party);
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

        void pack(Bit *src, Bit *dst, const int & bit_cnt)  override {
            if(wire_packing_) {
                protocol_->pack(src, (OMPCPackedWire *) dst, bit_cnt);
                return;
            }

            // else
            memcpy(dst, src, bit_cnt * sizeof(Bit));

        }

        void unpack(Bit *src, Bit *dst, const int & bit_cnt) override {
            if (wire_packing_) {
                protocol_->unpack(dst, (OMPCPackedWire *) src, bit_cnt);
                return;
            }
            memcpy(dst, src, bit_cnt * sizeof(Bit));
        }

    };
}

#endif // end if emp-sh2pc||emp-zk

#endif // end OUTSOURCE_MANAGER