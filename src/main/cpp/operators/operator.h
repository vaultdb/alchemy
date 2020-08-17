//
// Created by Jennie Rogers on 8/15/20.
//

#ifndef _OPERATOR_H
#define _OPERATOR_H

#include <functional>
#include <vector>
#include <querytable/query_table.h>

// for query operator tree and perhaps operator factory
// operator may run in the clear or in MPC
class Operator {

protected:
    std::shared_ptr<Operator> parent;
    std::vector<std::shared_ptr<Operator> > children;
    std::shared_ptr<QueryTable> output;


public:
    Operator() {} // initialize children and parent later
    Operator(std::shared_ptr<Operator> &child);
    Operator(std::shared_ptr<Operator> &lhs, std::shared_ptr<Operator> &rhs);
    ~Operator();

    // recurses first, then invokes runSelf method
    std::shared_ptr<QueryTable> run();
    std::shared_ptr<QueryTable> getOutput();
    std::shared_ptr<Operator> & getPtr(); // this is the shared_ptr / handle with which we refer to this operator

    std::shared_ptr<Operator> getParent() const;
    std::shared_ptr<Operator> getChild(int idx  = 0) const;

    void setParent(std::shared_ptr<Operator> & aParent);
    void setChild(std::shared_ptr<Operator>  aChild, int idx = 0);

   // Operator& operator=(const Operator& other); // TODO: copy assign operator overload

protected:
    // to be implemented by the operator classes, e.g., sort, filter, et cetera
    virtual std::shared_ptr<QueryTable> runSelf() = 0;
    std::shared_ptr<Operator> myRef; // TODO: for sharing among parents and children
};


#endif //_OPERATOR_H
