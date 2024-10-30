#ifndef _MERGE_INPUT_
#define _MERGE_INPUT_

#include "operators/operator.h"
#include "operators/secure_sql_input.h"
#include <string>

// for SQL input where we have truncation, but we are also using it for a MergeJoin - so it needs to be padded to line up with the other input
// only enabled when there is an input tuple limit on the SQL input - i.e., 
//  takes a SQL input from each party and coalesces each row to a single tuple - one of the two parties will input a dummy every time
// for Alice and Bob, this looks similar to:
// WITH alice_orders AS (SELECT * FROM orders_alice ORDER BY o_orderkey),
//    bob_orders AS (SELECT * FROM orders_bob ORDER BY o_orderkey) -- dummy padded so that when alice has a given row, bob has a dummy in this table offset using a supporting table like order_keys with public domain of primary key
// SELECT COALESCE(alice_vals, bob_vals) FROM alice_orders JOIN bob_orders ON alice_orders.o_orderkey = bob_orders.o_orderkey;
namespace vaultdb {
class MergeInput : public Operator<Bit> {
public:
    MergeInput(const string & db, const string & sql, const bool & dummy_tag, const size_t & input_tuple_cnt, const SortDefinition & def);
    MergeInput(const MergeInput & src) : Operator<Bit>(src), lhs_(src.lhs_->clone()), rhs_(src.rhs_->clone()) {}

    void updateCollation() override {
        lhs_->updateCollation();
        rhs_->updateCollation();
    }

    SecureOperator *clone() const override {
        return new MergeInput(*this);
    }

    ~MergeInput() {
        if(lhs_ != nullptr) delete lhs_;
        if(rhs_ != nullptr) delete rhs_;
    }

    bool operator==(const Operator<Bit> & other) const override {
        if(other.getType() != OperatorType::MERGE_INPUT) return false;

        auto other_node = dynamic_cast<const MergeInput &>(other);

        if(*lhs_ != *other_node.lhs_) return false;
        if(*rhs_ != *other_node.rhs_) return false;

        return this->operatorEquality(other);

    }

    string getSqlInput() const { return reinterpret_cast<SecureSqlInput *>(lhs_)->input_query_; }
    bool getHasDummyTag() const { return reinterpret_cast<SecureSqlInput *>(lhs_)->has_dummy_tag_; }
    string getDbName() const { return reinterpret_cast<SecureSqlInput *>(lhs_)->db_name_; }
    int getTupleLimit() const { return reinterpret_cast<SecureSqlInput *>(lhs_)->input_tuple_limit_; }

protected:
    QueryTable<Bit> *runSelf() override;
    OperatorType getType() const override {     return OperatorType::MERGE_INPUT;  }
    string getParameters() const override {
        return "\"" + reinterpret_cast<SecureSqlInput *>(lhs_)->input_query_ + "\", tuple_count=" + std::to_string(this->output_cardinality_);
    }

    SecureOperator *lhs_, *rhs_;
};



} // namespace vaultdb
#endif
