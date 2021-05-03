#include "limit.h"

using namespace vaultdb;

template<typename B>
Limit<B>::Limit(Operator<B> *child, const size_t &outputTuples, const SortDefinition & sort)  : Operator<B>(child, sort), limit(outputTuples) {

}

template<typename B>
Limit<B>::Limit(shared_ptr<QueryTable<B>> child, const size_t &outputTuples, const SortDefinition & sort) : Operator<B>(child, sort), limit(outputTuples) {

}

template<typename B>
shared_ptr<QueryTable<B>> Limit<B>::runSelf() {
   std::shared_ptr<QueryTable<B> > input = Operator<B>::children_[0]->getOutput();
    Operator<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(*input));

    // validate setup
    SortDefinition orderBy = input->getSortOrder();
    assert(orderBy.size() >= 1 && orderBy[0].first == -1); // dummy tags are at the end, otherwise output won't make sense.


    std::cout << "Limit received " << input->getTupleCount() << " tuples." << std::endl;

   if(Operator<B>::output_->getTupleCount() < limit) {
       return Operator<B>::output_;
   }

    Operator<B>::output_->resize(limit);
    return Operator<B>::output_;
}

template class vaultdb::Limit<bool>;
template class vaultdb::Limit<emp::Bit>;
