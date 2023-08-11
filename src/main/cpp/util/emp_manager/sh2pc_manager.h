#ifndef _SH2PC_MANAGER_
#define _SH2PC_MANAGER_

#include "emp_manager.h"
#include <util/system_configuration.h>

#if __has_include("emp-sh2pc/emp-sh2pc.h")
#include <emp-sh2pc/emp-sh2pc.h>

namespace  vaultdb {
    class SH2PCManager : public EmpManager {

    public:
        NetIO *netio_ = nullptr;
        int party_;

        SH2PCManager(string alice_host, int party, int port)  : party_(party) {
            netio_  = new emp::NetIO(party_ == emp::ALICE ? nullptr : alice_host.c_str(), port);

            emp::setup_semi_honest(netio_, party_, 1024 * 16);
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.emp_bit_size_bytes_ = sizeof(emp::block);
            s.party_ = party;

        }

        // set up EMP for insecure execution
        SH2PCManager()  {
            setup_plain_prot(false, "");
        }

        size_t andGateCount() const override {
           return  emp::CircuitExecution::circ_exec->num_and();
        }

        void  feed(Bit *labels, int party, const bool *b, int byte_count) override {
             emp::ProtocolExecution::prot_exec->feed((block *) labels, party, b, byte_count);
        }

        void flush() override { netio_->flush(); }

        ~SH2PCManager() {
            if(netio_ != nullptr) {
                emp::finalize_semi_honest();
                delete netio_;
            }
            else {
                finalize_plain_prot();
            }

        }

        QueryTable<Bit> *secretShare(const QueryTable<bool> *src) override;

        void reveal(bool *dst, const int & party, Bit *src, const int & bit_cnt) override {
            ProtocolExecution::prot_exec->reveal(dst, party, (block *) src, bit_cnt);
        }

        string revealToString(const emp::Integer & i, const int & party = PUBLIC)  const override {
           return  i.reveal<std::string>(party);
        }

        size_t getTableCardinality(const int & local_cardinality) override;

        void pack(Bit *src, Bit *dst, const int & bit_cnt)  override {
            memcpy(dst, src, bit_cnt * sizeof(Bit));
        }

        void unpack(Bit *src, Bit *dst, const int & bit_cnt)  override {
            memcpy(dst, src, bit_cnt * sizeof(Bit));
        }

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
    // placeholder to make it build when we're in other modes
    class SH2PCManager : public EmpManager {

    public:

        SH2PCManager(string alice_host, int party, int port)  {
            throw;
        }

         SH2PCManager()  { throw; }

        size_t andGateCount() const override { return 0; }

        void  feed(Bit *labels, int party, const bool *b, int byte_count) override  {
            throw;
        }

        void flush() override { throw; }

        ~SH2PCManager() = default;

        QueryTable<Bit> *secretShare(const QueryTable<bool> *src) override {
            throw;
        }

        void reveal(bool *dst, const int & party, Bit *src, const int & bit_cnt) override { throw; }

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
#endif // end if-emp-tool

#endif // end SH2PC_MANAGER_