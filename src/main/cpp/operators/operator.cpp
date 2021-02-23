#include "operator.h"


using namespace vaultdb;


Operator::Operator(shared_ptr<QueryTable> lhsSrc) {
        lhs = new TableInput(lhsSrc);
        children.push_back((Operator *) lhs);
}

Operator::Operator(shared_ptr<QueryTable> lhsSrc, shared_ptr<QueryTable> rhsSrc) {
    lhs = new TableInput(lhsSrc);
    children.push_back((Operator *) lhs);

    rhs = new TableInput(rhsSrc);
    children.push_back((Operator *) rhs);

}


Operator::Operator(Operator *child) {

     child->setParent(this);
    children.push_back(child);

}

Operator::Operator(Operator *lhs, Operator *rhs) {

    lhs->setParent(this);
    rhs->setParent(this);
    children.push_back(lhs);
    children.push_back(rhs);

}



std::shared_ptr<QueryTable> Operator::run() {
    if(operatorExecuted) // prevent duplicate executions of operator
        return output;

    for(Operator *op : children) {
        op->run();
    }

    output = runSelf(); // delegated to children
    operatorExecuted = true;
    return output;
}


 std::shared_ptr<QueryTable> Operator::getOutput()  {
    if(output.get() == nullptr) { // if we haven't run it yet
        output = run();
    }
    return output;
}

Operator * Operator::getParent() const {
    return parent;
}

 Operator * Operator::getChild(int idx) const {
    return children[idx];
}

void Operator::setParent(Operator *aParent) {
  parent = aParent;
}

void Operator::setChild(Operator *aChild, int idx) {
   children[idx] = aChild;
}

 Operator::~Operator() {
    if(lhs) delete lhs;
    if(rhs) delete rhs;

}




