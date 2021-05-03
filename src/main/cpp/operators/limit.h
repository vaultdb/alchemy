#ifndef _LIMIT_H
#define _LIMIT_H

#include <operators/operator.h>
#include <query_table/query_table.h>

namespace  vaultdb {

    template<typename B>
    class Limit : public Operator<B> {

        size_t limit;
    public:
        Limit(Operator<B> *child, const size_t &outputTuples, const SortDefinition & sort = SortDefinition());

        Limit(shared_ptr<QueryTable<B> > child, const size_t & outputTuples, const SortDefinition & sort = SortDefinition());

        ~Limit() = default;

        std::shared_ptr<QueryTable<B> > runSelf()  override;


    };


}




#endif //_LIMIT_H
