#ifndef _QUERY_TUPLE_H
#define _QUERY_TUPLE_H

#include "query_schema.h"
#include <map>
#include <memory>
#include <vector>
#include <util/type_utilities.h>
#include <emp-tool/circuits/bit.h>

using namespace emp;

namespace vaultdb {

    // QueryTuple is a thin wrapper around one entry in a QueryTable.  Its data is managed by the parent table.
    // specialized for pointer arithmetic in PlainTuple and SecureTuple

    // methods have to be real, i.e., not virtual, to be visible to projects and enable instantiating objects instead of ptrs

    template<typename B>
    class QueryTuple {
    public:
        QueryTuple() {};
        ~QueryTuple() = default;

        QueryTuple(QuerySchema *query_schema, int8_t *src) { assert(src != nullptr); }
        QueryTuple(QuerySchema *query_schema, const int8_t *src) { assert(src != nullptr); } // immutable case

        bool operator==(const QueryTuple<B> & other) const {throw; } // implement in instances
        bool operator!=(const QueryTuple<B> & other) const {throw; } // implement in instances, not supported for SecureTuple/emp case
        QueryTuple<B>& operator=(const QueryTuple<B> & other) { throw; } // need to do this in child classes
        Field<B> operator[](const int32_t & idx ) { throw; }
        Field<B> operator[](const int32_t & idx ) const {throw; }
        void setField(const size_t & idx, const Field<B> & field) { throw; }
        Field<B> getField(const int & ordinal) const { throw; }
        void setDummyTag(const bool & b) { throw; }
        void setDummyTag(const emp::Bit & b) { throw; }
        static void writeSubset(const QueryTuple<B> & src_tuple, const QueryTuple<B> & dst_tuple, uint32_t src_start_idx, uint32_t src_attr_cnt, uint32_t dst_start_idx) { throw; }
        B getDummyTag() const { throw; }
        string toString(const bool &showDummies = false) const { throw; }
        int8_t *getData() const { return nullptr; }
        static void compareSwap(const B &cmp, QueryTuple<B>  & lhs, QueryTuple<B>  & rhs){ throw; } // implemented in template specializations

        static QueryTuple<B> If(const B & cond, const QueryTuple<B> & lhs, const QueryTuple<B> & rhs) { throw; }
    };




} // namespace vaultdb
#endif // QUERY_TUPLE_H
