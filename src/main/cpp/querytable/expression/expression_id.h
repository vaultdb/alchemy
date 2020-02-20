//
// Created by madhav on 1/17/20.
//

#ifndef TESTING_EXPRESSION_ID_H
#define TESTING_EXPRESSION_ID_H
namespace vaultdb::expression {
enum class ExpressionId {
  INVALID = 0,
  LESSTHANOREQUAL,
  EQUAL,
  GREATERTHANOREQUAL,
  LESSTHAN,
  GREATERTHAN,
  SUBSTRING,
  AND,
};

};

#endif // TESTING_EXPRESSION_ID_H
