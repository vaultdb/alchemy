//
// Created by madhav on 1/15/20.
//

#ifndef MACROS_H
#define MACROS_H
namespace vaultdb {
#ifdef NDEBUG
#define VAULTDB_ASSERT(expr) ((void)0)
#else
#define VAULTDB_ASSERT(expr) assert((expr))
#endif /* NDEBUG */

#define VAULTDB_DEBUG(label, statement)                                               \
  do {                                                                         \
    std::cout << "*****" << std::endl;                                         \
    std::cout << #label << statement << std::endl;                                      \
    std::cout << "*****" << std::endl;                                         \
  } while (0)

} // namespace vaultdb

#endif // MACROS_H
