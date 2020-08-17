//
// Created by Jennie Rogers on 8/15/20.
//

#include "operator.h"



Operator::Operator(std::shared_ptr<Operator> child) {
    child->setParent(std::shared_ptr<Operator>(this));

    children.push_back(child);

}

Operator::Operator(std::shared_ptr<Operator> lhs, std::shared_ptr<Operator> rhs) {
    lhs->setParent(std::shared_ptr<Operator>(this));
    rhs->setParent(std::shared_ptr<Operator>(this));
    children.push_back(lhs);
    children.push_back(rhs);

}

std::shared_ptr<QueryTable> Operator::run() {
    for(std::shared_ptr<Operator> op : children) {
        op->run();
    }

    return runSelf(); // delegated to children

}


 std::shared_ptr<QueryTable> Operator::getOutput()  {
    if(!output) { // if we haven't run it yet
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

void Operator::setParent(std::shared_ptr<Operator> aParent) {
  parent = aParent;
}

void Operator::setChild(std::shared_ptr<Operator> aChild, int idx) {
   children[idx] = aChild;
}

