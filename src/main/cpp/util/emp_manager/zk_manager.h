#ifndef _ZK_MANAGER_H_
#define _ZK_MANAGER_H_
#include "emp_manager.h"

#if __has_include("emp-zk/emp-zk.h")

#include <emp-zk/emp-zk.h>
namespace vaultdb {
    class ZKManager : public EmpManager {
    public:
        static const int threads_ = 4;
        BoolIO<NetIO> *ios_[threads_];
        int party_;


        ZKManager(string gen_host, int party, int port) : party_(party) {
            for(int i = 0; i < threads_; ++i)
                ios_[i] = new BoolIO<NetIO>(new NetIO(party == ALICE ? nullptr
                                        : gen_host.c_str(),port+i), party==ALICE);
            setup_zk_bool<BoolIO<NetIO>>(ios_, threads_, party_);

        }

        size_t andGateCount() const override {
            return emp::CircuitExecution::circ_exec->num_and();
        }

        void feed(int8_t *labels, int party, const bool *b, int byte_count) override {
            emp::ProtocolExecution::prot_exec->feed((block *) labels, party, b, byte_count);
        }

        void flush() override {
            ios_[0]->flush();
        }

        QueryTable<Bit> *secretShare(const QueryTable<bool> *src) override;

        ~ZKManager() {
            for(int i = 0; i < threads_; ++i) {
                delete ios_[i]->io;
                delete ios_[i];
            }
        }

    private:
        static void secret_share_send(const int &party, const QueryTable<bool> *src_table, QueryTable<Bit> *dst_table);
        static void secret_share_recv(const size_t &tuple_count, const int &dst_party,
                                      QueryTable<Bit> *dst_table);

    };
}
#else
namespace  vaultdb {
    // placeholder to make this build
    class ZKManager : public EmpManager {

    public:

        ZKManager(string gen_host, int party, int port)  {
            throw;
        }


        size_t andGateCount() const { return 0; }

        size_t feed(int8_t *labels, int party, const bool *b, int byte_count) {
            throw;
        }

        void flush() { throw; }

        ~SH2PCManager() = default;

        QueryTable<Bit> *secretShare(const QueryTable<bool> *src) override {
            throw;
        }

    };
}
#endif // end if-emp-tool


#endif // _ZK_MANAGER_H_