#ifndef _EMP_MANAGER_
#define _EMP_MANAGER_
#include <common/defs.h>

// abstract class for managing interactions with EMP connection
// used to abstract away different branches, e.g., emp-resco/emp-zk/emp-sh2pc
// can support multiple implementations in the same branch/repo

namespace vaultdb {

    // forward declaration
    template <typename B> class QueryTable;

    class EmpManager {
    public:
        virtual size_t andGateCount() const = 0;
        virtual void feed(int8_t *labels, int party, const bool *b, int byte_count) = 0;
        virtual void flush() = 0;
        virtual QueryTable<Bit> *secretShare(const QueryTable<bool> *src) = 0;

        virtual ~EmpManager() {}

    };
}
#endif
