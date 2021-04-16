#include "limit.h"

using namespace vaultdb;

template<typename B>
Limit<B>::Limit(Operator<B> *child, const size_t &outputTuples)  : Operator<B>(child), limit(outputTuples) {

}

template<typename B>
Limit<B>::Limit(shared_ptr<QueryTable<B>> child, const size_t &outputTuples) : Operator<B>(child), limit(outputTuples) {

}

template<typename B>
shared_ptr<QueryTable<B>> Limit<B>::runSelf() {
   std::shared_ptr<QueryTable<B> > input = Operator<B>::children[0]->getOutput();
    Operator<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(*input));

    // validate setup
    SortDefinition orderBy = input->getSortOrder();
    assert(orderBy.size() >= 1 && orderBy[0].first == -1); // dummy tags are at the end, otherwise output won't make sense.


    std::cout << "Limit received " << input->getTupleCount() << " tuples." << std::endl;

   if(Operator<B>::output->getTupleCount() < limit) {
       return Operator<B>::output;
   }

    Operator<B>::output->resize(limit);
    return Operator<B>::output;
}

template class vaultdb::Limit<bool>;
template class vaultdb::Limit<emp::Bit>;
