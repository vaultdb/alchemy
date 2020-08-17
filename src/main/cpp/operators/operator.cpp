//
// Created by Jennie Rogers on 8/15/20.
//

#include "operator.h"



Operator::Operator(std::shared_ptr<Operator> &child) {

    myRef = std::shared_ptr<Operator>(this);

    child->setParent(myRef);

    children.push_back(child);

}

Operator::Operator(std::shared_ptr<Operator> &lhs, std::shared_ptr<Operator> &rhs) {
    myRef = std::shared_ptr<Operator>(this);

    lhs->setParent(myRef);
    rhs->setParent(myRef);
    children.push_back(lhs);
    children.push_back(rhs);

}


Operator::~Operator() {
    std::cout << "Freeing an operator at " << this << " with output " << output.get() << std::endl;


    std::cout << "Parent: " << parent.get() << " children.size() " << children.size() << std::endl;
    if(parent.get() != nullptr) {
        std::cout << "Parent ref counter: " << parent.use_count() << std::endl;
        parent.reset();
    }

    std::cout << "Freeing children: " << std::endl;
    for(std::shared_ptr<Operator> child : children) {
        child.reset();
    }

    std::cout << "Freeing output!" << std::endl;
    if(output.get() != nullptr) {
        output.reset();
    }

    std::cout << "Voila!" << std::endl;
}

std::shared_ptr<QueryTable> Operator::run() {
    for(std::shared_ptr<Operator> op : children) {
        op->run();
    }

    return runSelf(); // delegated to children

}


 std::shared_ptr<QueryTable> Operator::getOutput()  {
    if(output.get() == nullptr) { // if we haven't run it yet
        output = run();
    }
    return output;
}

std::shared_ptr<Operator> Operator::getParent() const {
    return parent;
}

 std::shared_ptr<Operator> Operator::getChild(int idx) const {
    return children[idx];
}

void Operator::setParent(std::shared_ptr<Operator> & aParent) {
  parent = aParent;
}

void Operator::setChild(std::shared_ptr<Operator> aChild, int idx) {
   children[idx] = aChild;
}

std::shared_ptr<Operator> & Operator::getPtr() {
    return myRef;
}

