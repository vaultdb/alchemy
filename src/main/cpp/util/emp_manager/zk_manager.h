#ifndef _ZK_MANAGER_H_
#define _ZK_MANAGER_H_
#include "emp_manager.h"
#include <util/system_configuration.h>


#if __has_include("emp-zk/emp-zk.h")

#include <emp-zk/emp-zk.h>
namespace vaultdb {
    class ZKManager : public EmpManager {
    public:
        static const int threads_ = 4;
        BoolIO<NetIO> *ios_[threads_];


        ZKManager(string gen_host, int party, int port) {

            for(int i = 0; i < threads_; ++i)
                ios_[i] = new BoolIO<NetIO>(new NetIO(party == ALICE ? nullptr
                                        : gen_host.c_str(),port+i), party==ALICE);
            setup_zk_bool<BoolIO<NetIO>>(ios_, threads_, party);
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.party_ = party;
            s.emp_mode_ = EmpMode::ZK;
        }

        // set up EMP for insecure execution
        ZKManager()  {
            setup_plain_prot(false, "");
            SystemConfiguration & s = SystemConfiguration::getInstance();
            s.emp_mode_ = EmpMode::PLAIN;
        }

        int sendingParty() const override {
            return ALICE;
        }
        
        size_t andGateCount() const override {
            return emp::CircuitExecution::circ_exec->num_and();
        }

        size_t getCommCost() const override {
            size_t comm_cost = 0;

            for(int i = 0; i < threads_; ++i) {
                comm_cost += ios_[i]->counter;
            }

            return comm_cost;
        };


        void feed(Bit *labels, int party, const bool *b, int byte_count) override {
            emp::ProtocolExecution::prot_exec->feed((block *) labels, party, b, byte_count);
        }

        void flush() override {
            for(int i = 0; i < threads_; ++i)
                ios_[i]->flush();
        }

        bool finalize() {
            return finalize_zk_bool<BoolIO<NetIO>>();
        }

        QueryTable<Bit> *secretShare(const QueryTable<bool> *src) override;

        void reveal(bool *dst, const int & party, Bit *src, const int & bit_cnt) override {
            ProtocolExecution::prot_exec->reveal(dst, party, (block *) src, bit_cnt);
        }

        string revealToString(const emp::Integer & i, const int & party = PUBLIC)  const override {
           return  i.reveal<std::string>(party);
        }

        ~ZKManager() {
            if(ios_[0] != nullptr) {
                for(int i = 0; i < threads_; ++i) {
                    delete ios_[i]->io;
                    delete ios_[i];
                }
            }
            else
                finalize_plain_prot();

        }

        size_t getTableCardinality(const int & local_cardinality) override;

        void pack(Bit *src, Bit *dst, const int & bit_cnt)  override {
            memcpy(dst, src, bit_cnt * sizeof(Bit));
        }

        void unpack(Bit *src, Bit *dst, const int & bit_cnt)  override {
            memcpy(dst, src, bit_cnt * sizeof(Bit));
        }

        void sendPublic(const int & to_send) override {
            NetIO *netio = ios_[0]->io;
            netio->send_data(&to_send, 4);
            netio->flush();

        }

        int recvPublic() override {
            NetIO *netio = ios_[0]->io;
            int to_recv;
            netio->recv_data(&to_recv, 4);
            netio->flush();
            return to_recv;
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

        // set up EMP for insecure execution
        ZKManager()  { throw; }

        int sendingParty() const override { throw; }

        size_t andGateCount() const override { return 0; }

        size_t getCommCost() const override { return 0; }

        void  feed(Bit *labels, int party, const bool *b, int byte_count) override  {
            throw;
        }

        void flush() override { throw; }

        bool finalize() {
            return false;
        }

        ~ZKManager() = default;

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
#endif // end if-emp-tool


#endif // _ZK_MANAGER_H_