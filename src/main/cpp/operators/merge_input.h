#ifndef _MERGE_INPUT_
#define _MERGE_INPUT_

#include "operator.h"
#include "secure_sql_input.h"


// for SQL input where we have truncation, but we are also using it for a MergeJoin - so it needs to be padded to line up with the other input
// only enabled when there is an input tuple limit on the SQL input - i.e., DESIGNED FOR TESTING, NOT (PUBLICATION) EXPERIMENTAL RESULTS
// essentially takes a SQL input from each party and coalesces each row to a single tuple - one of the two parties will input a dummy every time
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



    protected:
        QueryTable<Bit> *runSelf() override;
        OperatorType getType() const override {     return OperatorType::MERGE_INPUT;  }
        string getParameters() const {
            return "\"" + ((SecureSqlInput *)lhs_)->input_query_ + "\", tuple_count=" + std::to_string(this->output_cardinality_);
        }

        SecureOperator *lhs_, *rhs_;
    };



}
#endif
