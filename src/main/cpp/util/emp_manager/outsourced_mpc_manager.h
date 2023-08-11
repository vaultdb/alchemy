#ifndef _OUTSOURCE_MANAGER_
#define _OUTSOURCE_MANAGER_

#include "emp_manager.h"
#include <util/system_configuration.h>

#if __has_include("emp-rescu/emp-rescu.h")
#include <emp-rescu/emp-rescu.h>

namespace  vaultdb {
    class OutsourcedMpcManager : public EmpManager {

    public:
        emp::NetIO *tpio_ = nullptr;
        vector<NetIO *> ios_;

        emp::NetIO *tpio_ctrl_ = nullptr;
        vector<NetIO *> ios_ctrl_;
        int party_;

        OutsourcedMpcManager(string hosts[], int party, int comm_port, int ctrl_port)  : party_(party) {
            ios_ = emp::setup_netio(tpio_, hosts, comm_port, party_, N);
            ios_ctrl_ = emp::setup_netio(tpio_ctrl_, hosts, ctrl_port, party_, N);
            emp::backend = new OMPCBackend<N>(ios_, tpio_, party_);
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.emp_bit_size_bytes_  =  sizeof(Bit);
            s.party_ = party;
        }

        // setup for plaintext execution
        OutsourcedMpcManager() {
            emp::backend = new ClearPrinter();
        }

        size_t andGateCount() const override {
           return  ((OMPCBackend<N> *) backend)->num_and();
        }

        void  feed(Bit *labels, int party, const bool *b, int byte_count) override {
            ((OMPCBackend<N> *) backend)->feed((Bit *) labels, party, b, byte_count);
        }

        void flush() override {
            if(tpio_ != nullptr) tpio_->flush();

            for(NetIO *io : ios_) {
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
            in.reveal(b, party);
            stringstream s;
            for(int i = 0; i < in.size(); ++i) {
                char res =   (b[i]) ? '1' : '0';
                s << res;
            }

            delete [] b;
           return s.str();
        }

        size_t getTableCardinality(const int & local_cardinality) override;

    };
}

#else

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

        size_t andGateCount() const override { return 0; }

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

    };
}
#endif // end if-emp-tool

#endif // end OUTSOURCE_MANAGER