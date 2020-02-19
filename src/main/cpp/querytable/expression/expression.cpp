//
// Created by madhav on 1/17/20.
//

#include "expression.h"
#include "common/macros.h"
namespace vaultdb::expression {
Expression::Expression(vaultdb::types::Value *v1, ExpressionId id) {
  values_[0] = v1;
  num_values_ = 1;
  id_ = id;
}

Expression::Expression(const types::Value *v1, const types::Value *v2, ExpressionId id) {
  values_[0] = v1;
  values_[1] = v2;
  num_values_ = 2;
  //values_.reserve(2);
  //values_.push_back(v1);
  //values_.push_back(v2);
  id_ = id;
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
    // return EvaluateTrinary(values_[0], values_[1], values_[3], id_);
  }
  }
}

vaultdb::types::Value
Expression::EvaluateBinary(const types::Value *v1, const types::Value *v2,
                           ExpressionId id) {
  VAULTDB_ASSERT(v1->GetType() == v2->GetType());

  switch (id) {

  case ExpressionId::INVALID:
    throw;
  case ExpressionId::LESSTHANOREQUAL:
    throw;
  case ExpressionId::EQUAL:
    return vaultdb::types::Type::GetInstance(v1->GetType())
        .CompareEquals(*v1, *v2);
  case ExpressionId::GREATERTHANOREQUAL:
    throw;
  case ExpressionId::LESSTHAN:
    throw;
  case ExpressionId::GREATERTHAN:
    throw;
  case ExpressionId::SUBSTRING:
    throw;
  case ExpressionId::AND:
    return vaultdb::types::Type::GetInstance(v1->GetType())
        .And(*v1, *v2);
  }
}
} // namespace vaultdb::expression
