#ifndef _EMP_MANAGER_
#define _EMP_MANAGER_
#include <common/defs.h>

// abstract class for managing interactions with EMP connection
// used to abstract away different branches, e.g., emp-rescu/emp-zk/emp-sh2pc
// can support multiple implementations in the same branch/repo

namespace vaultdb {

    // forward declaration
    template <typename B> class QueryTable;

    class EmpManager {
    public:
        virtual size_t andGateCount() const = 0;
        virtual size_t getCommCost() const = 0;
        virtual void feed(Bit *labels, int party, const bool *b, int bit_cnt) = 0;
        virtual void flush() = 0;
        virtual QueryTable<Bit> *secretShare(const QueryTable<bool> *src) = 0;

        // for use in OMPC, all others have no-op
        virtual void pack(Bit *src, Bit *dst, const int & bit_cnt) = 0;
        virtual void unpack(Bit *src, Bit *dst, const int & bit_cnt) = 0;

        virtual void reveal(bool *dst, const int & party, Bit *src, const int & bit_cnt) = 0;
        virtual string revealToString(const emp::Integer & i, const int & party = PUBLIC)  const = 0;
        virtual size_t getTableCardinality(const int & local_cardinality) = 0;
        virtual ~EmpManager() {}

    };
}
#endif
