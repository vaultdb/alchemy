#include "operator.h"


using namespace vaultdb;

template<typename B>
Operator<B>::Operator(shared_ptr<QueryTable<B> > lhsSrc) {
        lhs = new TableInput(lhsSrc);
        children.push_back((Operator *) lhs);
}

template<typename B>
Operator<B>::Operator(shared_ptr<QueryTable<B> > lhsSrc, shared_ptr<QueryTable<B> > rhsSrc) {
    lhs = new TableInput(lhsSrc);
    children.push_back((Operator *) lhs);

    rhs = new TableInput(rhsSrc);
    children.push_back((Operator *) rhs);

}

template<typename B>
Operator<B>::Operator(Operator *child) {

     child->setParent(this);
    children.push_back(child);

}

template<typename B>
Operator<B>::Operator(Operator *lhs, Operator *rhs) {

    lhs->setParent(this);
    rhs->setParent(this);
    children.push_back(lhs);
    children.push_back(rhs);

}


template<typename B>
std::shared_ptr<QueryTable<B> > Operator<B>::run() {
    if(operatorExecuted) // prevent duplicate executions of operator
        return output;

    for(Operator *op : children) {
        op->run();
    }

    output = runSelf(); // delegated to children
    operatorExecuted = true;
    return output;
}

template<typename B>
 std::shared_ptr<QueryTable<B> > Operator<B> ::getOutput()  {
    if(output.get() == nullptr) { // if we haven't run it yet
        output = run();
    }
    return output;
}

template<typename B>
Operator<B> * Operator<B>::getParent() const {
    return parent;
}

template<typename B>
 Operator<B> * Operator<B>::getChild(int idx) const {
    return children[idx];
}

template<typename B>
void Operator<B>::setParent(Operator *aParent) {
  parent = aParent;
}

template<typename B>
void Operator<B>::setChild(Operator *aChild, int idx) {
   children[idx] = aChild;
}

template<typename B>
 Operator<B>::~Operator() {
    if(lhs) delete lhs;
    if(rhs) delete rhs;

}

template class vaultdb::Operator<bool>;
template class vaultdb::Operator<emp::Bit>;




