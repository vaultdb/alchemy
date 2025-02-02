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



    };

}


#else

#include <emp-rescu/emp-rescu.h>
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

        OutsourcedMpcManager(string hosts[], int party, int comm_port, int ctrl_port)  : party_(party) {
            ios_ = emp::setup_netio(tpio_, hosts, comm_port, party_, N);
            ios_ctrl_ = emp::setup_netio(tpio_ctrl_, hosts, ctrl_port, party_, N);
            emp::backend = new OMPCBackend<N>(ios_, tpio_, party_);
            protocol_ = (OMPCBackend<N> *) emp::backend;
            cout << "Memory usage of backend setup: " << Utilities::checkMemoryUtilization(true) << endl;
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.party_ = party;
            s.emp_mode_ = EmpMode::OUTSOURCED;
        }

        OutsourcedMpcManager(string hosts[], int party, int comm_port, int ctrl_port, string backend_state_path)  : party_(party) {
            ios_ = emp::setup_netio(tpio_, hosts, comm_port, party_, N);
            ios_ctrl_ = emp::setup_netio(tpio_ctrl_, hosts, ctrl_port, party_, N);
            emp::backend = new OMPCBackend<N>(ios_, tpio_, party_, backend_state_path);
            protocol_ = (OMPCBackend<N> *) emp::backend;
            cout << "Memory usage of backend setup: " << Utilities::checkMemoryUtilization(true) << endl;
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.party_ = party;
            s.emp_mode_ = EmpMode::OUTSOURCED;
        }

        // setup for plaintext execution
        OutsourcedMpcManager() {
            emp::backend = new ClearPrinter();
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.emp_mode_ = EmpMode::PLAIN;

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

            // flush buffer pool
            system_conf_.bpm_.reset();
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




    };
}

#endif // end if emp-sh2pc||emp-zk

#endif // end OUTSOURCE_MANAGER