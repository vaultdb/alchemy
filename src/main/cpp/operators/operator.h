#ifndef _OPERATOR_H
#define _OPERATOR_H

#include <functional>
#include <vector>
#include <query_table/query_table.h>

using namespace std;



// for query operator tree and perhaps operator factory
// operator may run in the clear or in MPC
namespace  vaultdb {

    template<typename B> class TableInput;

    template<typename B>
    class Operator {

    protected:
        Operator *parent;
        vector<Operator *> children;
        shared_ptr<QueryTable<B> > output;
        TableInput<B> *lhs = 0;
        TableInput<B> *rhs = 0;


    public:


        Operator()  {  }

        virtual ~Operator();

        Operator(shared_ptr<QueryTable<B> > lhs);
        Operator(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs);


        Operator(Operator *child);
        Operator(Operator *lhs, Operator *rhs);
        
        // recurses first, then invokes runSelf method
        std::shared_ptr<QueryTable<B> > run();

        std::shared_ptr<QueryTable<B> > getOutput();


        Operator * getParent() const;

        Operator * getChild(int idx = 0) const;

        void setParent(Operator *aParent);

        void setChild(Operator *aChild, int idx = 0);

        // Operator& operator=(const Operator& other); // TODO: copy assign operator overload

    protected:
        // to be implemented by the operator classes, e.g., sort, filter, et cetera
        virtual std::shared_ptr<QueryTable<B> > runSelf() = 0;
        bool operatorExecuted = false; // set when runSelf() executed once
    };

    // essentially CommonTableExpression, written here to avoid forward declarations
    template<typename B>
    class TableInput: public Operator<B> {

    public:
        TableInput(const std::shared_ptr<QueryTable<B> > & inputTable) {
            Operator<B>::output = std::move(inputTable);
            Operator<B>::operatorExecuted = true;
        }

        std::shared_ptr<QueryTable<B> > runSelf() override {
            return  Operator<B>::output;
        }

    };
}

#endif //_OPERATOR_H
