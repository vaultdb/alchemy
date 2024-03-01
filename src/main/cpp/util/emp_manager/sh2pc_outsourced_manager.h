#ifndef SH2PC_OUTSOURCED_MANAGER_
#define SH2PC_OUTSOURCED_MANAGER_

#include "emp_manager.h"
#include <util/system_configuration.h>

// like SH2PCManager, but with only Alice providing input
#if __has_include("emp-sh2pc/emp-sh2pc.h")
#include <emp-sh2pc/emp-sh2pc.h>

namespace  vaultdb {
    class SH2PCOutsourcedManager : public EmpManager {

    public:
        NetIO *netio_ = nullptr;
        int party_;

        SH2PCOutsourcedManager(string alice_host, int party, int port)  : party_(party) {
            netio_  = new emp::NetIO(party_ == emp::ALICE ? nullptr : alice_host.c_str(), port);

            emp::setup_semi_honest(netio_, party_, 1024 * 16);
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.party_ = party;
            s.emp_mode_ = EmpMode::SH2PC_OUTSOURCED;
        }

        // set up EMP for insecure execution
        SH2PCOutsourcedManager()  {
            setup_plain_prot(false, "");
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.emp_mode_ = EmpMode::PLAIN;

        }

         int sendingParty() const override {
            return ALICE;
        }

        size_t andGateCount() const override {
           return  emp::CircuitExecution::circ_exec->num_and();
        }

        size_t getCommCost() const override {
            return netio_->counter;
        }


        void  feed(Bit *labels, int party, const bool *b, int byte_count) override {
             emp::ProtocolExecution::prot_exec->feed((block *) labels, party, b, byte_count);
        }

        void flush() override { netio_->flush(); }

        ~SH2PCOutsourcedManager() {
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

        inline void pack(Bit *src, Bit *dst, const int & bit_cnt)  override {
            memcpy(dst, src, bit_cnt * sizeof(Bit));
        }

        inline void unpack(Bit *src, Bit *dst, const int & bit_cnt)  override {
            memcpy(dst, src, bit_cnt * sizeof(Bit));
        }

        void sendPublic(const int & to_send) override {
            netio_->send_data(&to_send, 4);
            netio_->flush();
        }

        int recvPublic() override {
            int to_recv;
            netio_->recv_data(&to_recv, 4);
            netio_->flush();
            return to_recv;
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
    class SH2PCOutsourcedManager : public EmpManager {

    public:
        SH2PCOutsourcedManager(string alice_host, int party, int port)  {
            throw;
        }

        SH2PCOutsourcedManager()  { throw; }
        int sendingParty() const override { throw; }

        size_t andGateCount() const override { return 0; }

        size_t getCommCost() const override { return 0; }

        void  feed(Bit *labels, int party, const bool *b, int byte_count) override  {
            throw;
        }

        void flush() override { throw; }

        ~SH2PCOutsourcedManager() = default;

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

        void sendPublic(const int & to_send) override { throw; }

        int recvPublic() override { throw; }


    };
}
#endif // end if-emp-tool

#endif // end SH2PC_OUTSOURCED_MANAGER_