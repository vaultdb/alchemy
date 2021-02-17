#ifndef _OPERATOR_H
#define _OPERATOR_H

#include <functional>
#include <vector>
#include <query_table/query_table.h>

using namespace std;



// for query operator tree and perhaps operator factory
// operator may run in the clear or in MPC
namespace  vaultdb {

    class TableInput;

    class Operator {

    protected:
        Operator *parent;
        vector<Operator *> children;
        shared_ptr<QueryTable> output;
        TableInput *lhs = 0;
        TableInput *rhs = 0;


    public:


        Operator()  {  }

        virtual ~Operator();

        Operator(shared_ptr<QueryTable> lhs);
        Operator(shared_ptr<QueryTable> lhs, shared_ptr<QueryTable> rhs);


        Operator(Operator *child);
        Operator(Operator *lhs, Operator *rhs);
        
        // recurses first, then invokes runSelf method
        std::shared_ptr<QueryTable> run();

        std::shared_ptr<QueryTable> getOutput();


        Operator * getParent() const;

        Operator * getChild(int idx = 0) const;

        void setParent(Operator *aParent);

        void setChild(Operator *aChild, int idx = 0);

        // Operator& operator=(const Operator& other); // TODO: copy assign operator overload

    protected:
        // to be implemented by the operator classes, e.g., sort, filter, et cetera
        virtual std::shared_ptr<QueryTable> runSelf() = 0;
        bool operatorExecuted = false; // set when runSelf() executed once
    };

    // essentially CommonTableExpression, written here to avoid forward declarations
    class TableInput: public Operator {

    public:
        TableInput(const std::shared_ptr<QueryTable> & inputTable) {
            output = std::move(inputTable);
            operatorExecuted = true;
        }

        std::shared_ptr<QueryTable> runSelf() override {
            return output;
        }

    };
}

#endif //_OPERATOR_H
