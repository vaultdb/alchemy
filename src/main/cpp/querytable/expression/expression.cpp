//
// Created by madhav on 1/17/20.
//

#include "expression.h"
#include "common/macros.h"
#include "querytable/types/type.h"
#include "querytable/types/value.h"
namespace vaultdb::expression {
Expression::Expression(const vaultdb::types::Value *v1, const ExpressionId id) {
  values_[0] = v1;
  num_values_ = 1;
  id_ = id;
}

Expression::Expression(const types::Value *v1, const types::Value *v2,
                       const ExpressionId id) {
  values_[0] = v1;
  values_[1] = v2;
  num_values_ = 2;
  id_ = id;
}

Expression::Expression(const types::Value *v1, types::Value *v2,
                       types::Value *v3, ExpressionId id) {
  values_[0] = v1;
  mutable_values_[0] = v2;
  mutable_values_[1] = v3;
  num_values_ = 3;
  id_ = id;
}

void Expression::ExecuteMutable() {
  switch (num_values_) {
  case 3: {
    return EvaluateMutableTernary(values_[0], mutable_values_[0],
                                  mutable_values_[1], id_);
  }
  default:
    throw;
  }
}

vaultdb::types::Value Expression::execute() {
  switch (num_values_) {
  case 1: {
    // return EvaluateUnary(values_[0], id_);
  }
  case 2: {
    return EvaluateBinary(values_[0], values_[1], id_);
  }
  case 3: {
    throw;
  }
  }
}

void Expression::EvaluateMutableTernary(const types::Value *condition,
                                        types::Value *v2, types::Value *v3,
                                        ExpressionId id) {
  VAULTDB_ASSERT(v2->GetType() == v3->GetType());
  switch (id) {
  case ExpressionId::SWAP: {
    return vaultdb::types::Type::GetInstance(v2->GetType())
        .Swap(*condition, *v2, *v3);
  }
  default:
    throw;
  }
}

vaultdb::types::Value Expression::EvaluateBinary(const types::Value *v1,
                                                 const types::Value *v2,
                                                 const ExpressionId id) {
  VAULTDB_ASSERT(v1->GetType() == v2->GetType());

  switch (id) {

  case ExpressionId::SWAP:
  case ExpressionId::INVALID:
  case ExpressionId::SELECT:
    throw;
  case ExpressionId::LESSTHANOREQUAL:
    return vaultdb::types::Type::GetInstance(v1->GetType())
        .CompareLessThanOrEqual(*v1, *v2);
  case ExpressionId::EQUAL:
    return vaultdb::types::Type::GetInstance(v1->GetType())
        .CompareEquals(*v1, *v2);
  case ExpressionId::GREATERTHAN:
    return vaultdb::types::Type::GetInstance(v1->GetType())
        .CompareGreaterThan(*v1, *v2);
  case ExpressionId::GREATERTHANOREQUAL:
    throw;
  case ExpressionId::LESSTHAN:
    return vaultdb::types::Type::GetInstance(v1->GetType())
        .CompareLessThan(*v1, *v2);
  case ExpressionId::SUBSTRING:
    throw;
  case ExpressionId::AND:
    return vaultdb::types::Type::GetInstance(v1->GetType()).And(*v1, *v2);
  case ExpressionId::OR:
    return vaultdb::types::Type::GetInstance(v1->GetType()).Or(*v1, *v2);
  }
}
} // namespace vaultdb::expression
