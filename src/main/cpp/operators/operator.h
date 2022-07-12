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
        Operator *parent_;
        vector<Operator *> children_;
        shared_ptr<QueryTable<B> > output_;
        TableInput<B> *lhs_ = nullptr;
        TableInput<B> *rhs_ = nullptr;
        SortDefinition sort_definition_; // start out with empty sort
        QuerySchema output_schema_;
        double run_time_;  // microseconds


    public:


        Operator(const SortDefinition & sorted_on = SortDefinition()) : sort_definition_(sorted_on) {}

        virtual ~Operator();

        Operator(shared_ptr<QueryTable<B> > lhs, const SortDefinition & sorted_on = SortDefinition());
        Operator(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs, const SortDefinition & sorted_on = SortDefinition());

        Operator(Operator *child, const SortDefinition & sorted_on = SortDefinition());
        Operator(Operator *lhs, Operator *rhs, const SortDefinition & sorted_on = SortDefinition());
        
        // recurses first, then invokes runSelf method
        std::shared_ptr<QueryTable<B> > run();
        std::string printTree() const;
        std::string toString() const;
        int getOperatorId() const;
        void setOperatorId(int op_id);
        std::shared_ptr<QueryTable<B> > getOutput();


        Operator * getParent() const;

        Operator * getChild(int idx = 0) const;

        void setParent(Operator *aParent);

        void setChild(Operator *aChild, int idx = 0);

        // Operator& operator=(const Operator& other); // TODO: copy assign operator overload

        SortDefinition  getSortOrder() const;
        void setSortOrder(const SortDefinition & aSortDefinition) { sort_definition_ = aSortDefinition; }

        QuerySchema getOutputSchema() const { return output_schema_; }

    protected:
        // to be implemented by the operator classes, e.g., sort, filter, et cetera
        virtual std::shared_ptr<QueryTable<B> > runSelf() = 0;
        virtual std::string getOperatorType() const  = 0;
        virtual std::string getParameters() const = 0;

        bool operator_executed_ = false; // set when runSelf() executed once

    private:
        std::string printHelper(const std::string & prefix) const;
        int operator_id_ = -1;

    };

    // essentially CommonTableExpression, written here to avoid forward declarations
    template<typename B>
    class TableInput : public Operator<B> {

    public:
        TableInput(const std::shared_ptr<QueryTable<B> > & inputTable) {
            Operator<B>::output_ = std::move(inputTable);
            Operator<B>::output_schema_ = *(Operator<B>::output_->getSchema());
            Operator<B>::sort_definition_ = inputTable->getSortOrder();
            Operator<B>::operator_executed_ = true;
        }

        std::shared_ptr<QueryTable<B> > runSelf() override {
            return  Operator<B>::output_;
        }

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
