#ifndef _OPERATOR_H
#define _OPERATOR_H

#include <functional>
#include <vector>
#include <query_table/query_table.h>

// for query operator tree and perhaps operator factory
// operator may run in the clear or in MPC
namespace  vaultdb {
    class Operator {

    protected:
        std::shared_ptr<Operator> parent;
        std::vector<std::shared_ptr<Operator> > children;
        std::shared_ptr<QueryTable> output;


    public:

        // insert op as new root to tree
        static std::shared_ptr<Operator> getOperatorTree(Operator *op, std::shared_ptr<Operator> child);

        // insert op as new root to tree with children lhs and rhs
        static std::shared_ptr<Operator> getOperatorTree(Operator *op, std::shared_ptr<Operator> lhs, std::shared_ptr<Operator> rhs);

        Operator()  { myRef = std::shared_ptr<Operator>(this); }
        Operator(std::shared_ptr<Operator> &child);

        ~Operator() { std::cout << "Freeing Operator at " << this << std::endl; }
        Operator(std::shared_ptr<Operator> &lhs, std::shared_ptr<Operator> &rhs);
        
        // recurses first, then invokes runSelf method
        std::shared_ptr<QueryTable> run();

        std::shared_ptr<QueryTable> getOutput();

        std::shared_ptr<Operator> &getPtr(); // this is the shared_ptr / handle with which we refer to this operator

        std::shared_ptr<Operator> getParent() const;

        std::shared_ptr<Operator> getChild(int idx = 0) const;

        void setParent(std::shared_ptr<Operator> &aParent);

        void setChild(std::shared_ptr<Operator> aChild, int idx = 0);

        // Operator& operator=(const Operator& other); // TODO: copy assign operator overload

    protected:
        // to be implemented by the operator classes, e.g., sort, filter, et cetera
        virtual std::shared_ptr<QueryTable> runSelf() = 0;

        std::shared_ptr<Operator> myRef; // TODO: for sharing among parents and children
    };
}

#endif //_OPERATOR_H
