#include <util/data_utilities.h>
#include "operator.h"


using namespace vaultdb;

template<typename B>
Operator<B>::Operator(shared_ptr<QueryTable<B> > lhsSrc, const SortDefinition & sorted_on) : sort_definition_(sorted_on) {
    lhs_ = new TableInput(lhsSrc);
    children_.push_back((Operator *) lhs_);
    output_schema_ = *lhsSrc->getSchema();
    if(sorted_on.empty())
        sort_definition_ = lhsSrc->getSortOrder();
}

template<typename B>
Operator<B>::Operator(shared_ptr<QueryTable<B> > lhsSrc, shared_ptr<QueryTable<B> > rhsSrc, const SortDefinition & sorted_on) : sort_definition_(sorted_on) {
    lhs_ = new TableInput(lhsSrc);
    children_.push_back((Operator *) lhs_);

    rhs_ = new TableInput(rhsSrc);
    children_.push_back((Operator *) rhs_);

}

template<typename B>
Operator<B>::Operator(Operator *child, const SortDefinition & sorted_on) : sort_definition_(sorted_on)  {

     child->setParent(this);
    children_.push_back(child);
    output_schema_ = child->output_schema_;
    if(sorted_on.empty())
        sort_definition_ = child->sort_definition_;

}

template<typename B>
Operator<B>::Operator(Operator *lhs, Operator *rhs, const SortDefinition & sorted_on) : sort_definition_(sorted_on)  {

    lhs->setParent(this);
    rhs->setParent(this);
    children_.push_back(lhs);
    children_.push_back(rhs);

}


template<typename B>
std::shared_ptr<QueryTable<B> > Operator<B>::run() {
    if(operator_executed_) // prevent duplicate executions of operator
        return output_;

    for(Operator *op : children_) {
        op->run();
    }

    auto start_time = clock_start(); // from emp toolkit

    output_ = runSelf(); // delegated to children
    run_time_ = time_from(start_time);
    std::cout << "Operator ran for " << run_time_/10e-6 << " seconds." << std::endl;
    operator_executed_ = true;
    sort_definition_  = output_->getSortOrder(); // update this if needed
    return output_;
}


template<typename B>
std::string Operator<B>::printTree() const {
    return printHelper("");

}

template<typename B>
std::string Operator<B>::printHelper(const string &prefix) const {
    stringstream  ss;
    ss << prefix << toString() << endl;
    string indent = prefix + "    ";
    for(Operator<B> *op : children_) {
        ss << op->printHelper(indent);
    }

    return ss.str();
}


template<typename B>
std::string Operator<B>::toString() const {
    stringstream  ss;

    string bool_type = (std::is_same_v<B, bool>) ? "bool" : "Bit";
    ss <<   getOperatorType() << "<" << bool_type << "> ";

    string params = getParameters();
    if(!params.empty())
        ss << "(" << params << ") ";

    ss << ": " <<  output_schema_ <<    " order by: " << DataUtilities::printSortDefinition(sort_definition_);

    if(output_.get() != nullptr) {
            ss << " tuple count: " << output_->getTupleCount();
            //if(std::is_same_v<bool, B>) {
            //    ss << " true tuple count: " << output_->getTrueTupleCount();
           // }
    }
    return ss.str();

}



template<typename B>
 std::shared_ptr<QueryTable<B> > Operator<B> ::getOutput()  {
    if(!operator_executed_) { // if we haven't run it yet
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

template<typename B>
SortDefinition Operator<B>::getSortOrder() const {
    assert(output_.get() == nullptr || sort_definition_ == output_->getSortOrder()); // check that output table is aligned with operator's sort order
    return sort_definition_;
}


std::ostream &vaultdb::operator<<(std::ostream &os, const PlainOperator &op) {
    os << op.printTree();
    return os;
}

std::ostream &vaultdb::operator<<(std::ostream &os, const SecureOperator &op) {
    os << op.printTree();
    return os;
}

template class vaultdb::Operator<bool>;
template class vaultdb::Operator<emp::Bit>;




