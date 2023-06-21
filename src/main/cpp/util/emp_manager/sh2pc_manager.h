#ifndef _SH2PC_MANAGER_
#define _SH2PC_MANAGER_

#include "emp_manager.h"

#if __has_include("emp-sh2pc/emp-sh2pc.h")
#include <emp-sh2pc/emp-sh2pc.h>

namespace  vaultdb {
    class SH2PCManager : public EmpManager {

    public:
        NetIO *netio_;
        int party_;

        SH2PCManager(string alice_host, int party, int port)  : party_(party) {
            netio_  = new emp::NetIO(party_ == emp::ALICE ? nullptr : alice_host.c_str(), port);
            cout << "Initialized netio in SH2PCManager at " << (size_t) netio_ << endl;

            emp::setup_semi_honest(netio_, party_, 1024 * 16);
        }



        size_t andGateCount() const override {
           return  emp::CircuitExecution::circ_exec->num_and();
        }

        void  feed(int8_t *labels, int party, const bool *b, int byte_count) override {
             emp::ProtocolExecution::prot_exec->feed((block *) labels, party, b, byte_count);
        }

        void flush() override { netio_->flush(); }

        ~SH2PCManager() { delete netio_; }

        QueryTable<Bit> *secretShare(const QueryTable<bool> *src) override;

    private:
        static void secret_share_recv(const size_t &tuple_count, const int &dst_party,
                               QueryTable<Bit> *dst_table, const size_t &write_offset,
                               const bool &reverse_read_order);
        static void secret_share_send(const int &party, const QueryTable<bool> *src_table, QueryTable<Bit> *dst_table,
                                      const int &write_offset,
                                      const bool &reverse_read_order);
    };
}

#else

namespace  vaultdb {
    // placeholder to make this build
    class SH2PCManager : public EmpManager {

    public:

        SH2PCManager(string alice_host, int party, int port)  {
            throw;
        }

        inline int recvInt(const int &party) override {
           return 0;
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

#endif // end SH2PC_MANAGER_