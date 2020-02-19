//
// Created by madhav on 1/15/20.
//

#ifndef TESTING_MACROS_H
#define TESTING_MACROS_H
namespace vaultdb {
#ifdef NDEBUG
#define VAULTDB_ASSERT(expr) ((void)0)
#else
#define VAULTDB_ASSERT(expr) assert((expr))
#endif /* NDEBUG */

} // namespace vaultdb

#endif // TESTING_MACROS_H
