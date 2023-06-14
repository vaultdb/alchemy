#ifndef _OPERATOR_H
#define _OPERATOR_H

#include <functional>
#include <vector>
#include <query_table/query_table.h>
#include <common/defs.h>

using namespace std;



// for query operator tree and perhaps operator factory
// operator may run in the clear or in MPC
namespace  vaultdb {

    template<typename B> class TableInput;

    template<typename B> class Operator;

    typedef  Operator<bool> PlainOperator;
    typedef  Operator<emp::Bit> SecureOperator;


    template<typename B>
    class Operator {

    protected:
        Operator *parent_ = nullptr;
        Operator *lhs_child_ = nullptr;
        Operator *rhs_child_ = nullptr;
        QueryTable<B> *output_ = nullptr;
        SortDefinition sort_definition_; // start out with empty sort
        QuerySchema output_schema_;
        time_point<high_resolution_clock> start_time_;
        size_t start_gate_cnt_ = 0L;
        double runtime_ = 0.0;

    public:

        Operator(const SortDefinition & sorted_on = SortDefinition()) : sort_definition_(sorted_on) {


        }

        virtual ~Operator() {
            if(output_ != nullptr)  delete output_;
            if(lhs_child_  != nullptr)   delete lhs_child_;
            if(rhs_child_ != nullptr)   delete rhs_child_;
        }

        Operator(QueryTable<B> * lhs, const SortDefinition & sorted_on = SortDefinition());
        Operator(QueryTable<B> *lhs, QueryTable<B> *rhs, const SortDefinition & sorted_on = SortDefinition());

        Operator(Operator *child, const SortDefinition & sorted_on = SortDefinition());
        Operator(Operator *lhs, Operator *rhs, const SortDefinition & sorted_on = SortDefinition());
        
        // recurses first, then invokes runSelf method
        QueryTable<B>  *run();
        std::string printTree() const;
        std::string toString() const;
        inline int getOperatorId() const {     return operator_id_; }
        inline void setOperatorId(int op_id) { operator_id_ = op_id; }
        inline QueryTable<B> *getOutput() {
            if(!operator_executed_) { // if we haven't run it yet
                output_ = run();
            }
            return output_;


        }

        inline Operator * getParent() const { return parent_; }

        inline Operator * getChild(int idx = 0) const {
            if(idx == 0) return lhs_child_;
            return rhs_child_;
        }

        void setParent(Operator *p) {
            parent_ = p;

        }

        inline void setChild(Operator *c, int idx = 0) {
            if(idx == 0)
                lhs_child_ = c;
            else
              rhs_child_ = c;
        }

        bool isLeaf() const { return  (lhs_child_ == nullptr); }
        
        inline SortDefinition  getSortOrder() const {
            assert(output_ == nullptr || sort_definition_ == output_->getSortOrder()); // check that output table is aligned with operator's sort order
            // need to cache this locally in case output not initialized yet
            return sort_definition_;
        }

        void setSortOrder(const SortDefinition & aSortDefinition) { sort_definition_ = aSortDefinition; }

        QuerySchema getOutputSchema() const { return output_schema_; }


    protected:
        // to be implemented by the operator classes, e.g., sort, filter, et cetera
        virtual QueryTable<B> *runSelf() = 0;
        virtual std::string getOperatorType() const  = 0;
        virtual std::string getParameters() const = 0;
        inline void reset() {
            if(output_ != nullptr) {
                delete output_;
                output_ = nullptr;
            }

            operator_executed_ = false;
        }

        bool operator_executed_ = false; // set when runSelf() executed once

    private:
        std::string printHelper(const std::string & prefix) const;
        int operator_id_ = -1;

    };

    // essentially CommonTableExpression, written here to avoid forward declarations
    template<typename B>
    class TableInput : public Operator<B> {

    public:
        TableInput(QueryTable<B>  *input) {

            Operator<B>::output_ = input; // point to input table
            Operator<B>::output_schema_ = input->getSchema();
            Operator<B>::sort_definition_ = Operator<B>::output_->getSortOrder();
            Operator<B>::operator_executed_ = true;
        }

        QueryTable<B> *runSelf() override {
            return  Operator<B>::output_;
        }

        virtual ~TableInput() = default;

    protected:
        string getOperatorType() const override {
            return "TableInput";
        }

        string getParameters() const override {
            return std::string();
        }

    };

    std::ostream &operator<<(std::ostream &os, const PlainOperator &op);
    std::ostream &operator<<(std::ostream &os, const SecureOperator &op);

}

#endif //_OPERATOR_H
