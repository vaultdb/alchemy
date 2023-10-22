#ifndef __SORT_MERGE_JOIN__
#define __SORT_MERGE_JOIN__

#include <expression/generic_expression.h>
#include "operator.h"
#include "join.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

namespace vaultdb {
	template<typename B>
	class SortMergeJoin : public Join<B> {
	public:
		SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());
	};
};

#endif
