#ifndef _REPLACE_TUPLE_H
#define _REPLACE_TUPLE_H


#include <vaultdb.h>

// class that obliviously writes to a QueryTable in the clear
// setup for PKeyFkeyJoin and aggregates to prevent branching at every tuple write for plaintext/encrypted decision
// implements the following logic:
// if(dstTable[writeIdx] is dummy)
//    dstTable[writeIdx] = dstTable[writeIdx];
// else
//    dstTable[writeIdx] = inputTuple;

// this is the base case, for writing in the plaintext case
// TODO: might replace this with ptrs or compare and swap
namespace vaultdb {
    class ReplaceTuple {


    public:
        explicit ReplaceTuple(std::shared_ptr<QueryTable> table);

        // toWrite is a bool || emp::Bit
        virtual void
        conditionalWrite(const uint32_t &writeIdx, const QueryTuple &inputTuple, const Field & toWrite);

        virtual ~ReplaceTuple() = default;

    protected:
        std::shared_ptr<QueryTable> dstTable;

    };
}

#endif //_WRITE_TUPLE_H
