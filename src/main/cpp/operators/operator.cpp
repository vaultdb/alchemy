#include "operator.h"


using namespace vaultdb;

template<typename B>
Operator<B>::Operator(shared_ptr<QueryTable<B> > lhsSrc) {
    lhs_ = new TableInput(lhsSrc);
        children_.push_back((Operator *) lhs_);
}

template<typename B>
Operator<B>::Operator(shared_ptr<QueryTable<B> > lhsSrc, shared_ptr<QueryTable<B> > rhsSrc) {
    lhs_ = new TableInput(lhsSrc);
    children_.push_back((Operator *) lhs_);

    rhs_ = new TableInput(rhsSrc);
    children_.push_back((Operator *) rhs_);

}

template<typename B>
Operator<B>::Operator(Operator *child) {

     child->setParent(this);
    children_.push_back(child);

}

template<typename B>
Operator<B>::Operator(Operator *lhs, Operator *rhs) {

    lhs->setParent(this);
    rhs->setParent(this);
    children_.push_back(lhs);
    children_.push_back(rhs);

}


template<typename B>
std::shared_ptr<QueryTable<B> > Operator<B>::run() {
    if(operatorExecuted) // prevent duplicate executions of operator
        return output_;

    for(Operator *op : children_) {
        op->run();
    }

    output_ = runSelf(); // delegated to children
    operatorExecuted = true;
    return output_;
}

template<typename B>
 std::shared_ptr<QueryTable<B> > Operator<B> ::getOutput()  {
    if(output_.get() == nullptr) { // if we haven't run it yet
        output_ = run();
    }
    return output_;
}

template<typename B>
Operator<B> * Operator<B>::getParent() const {
    return parent_;
}

template<typename B>
 Operator<B> * Operator<B>::getChild(int idx) const {
    return children_[idx];
}

template<typename B>
void Operator<B>::setParent(Operator *aParent) {
    parent_ = aParent;
}

template<typename B>
void Operator<B>::setChild(Operator *aChild, int idx) {
    children_[idx] = aChild;
}

template<typename B>
 Operator<B>::~Operator() {
    if(lhs_) delete lhs_;
    if(rhs_) delete rhs_;

}

template class vaultdb::Operator<bool>;
template class vaultdb::Operator<emp::Bit>;




