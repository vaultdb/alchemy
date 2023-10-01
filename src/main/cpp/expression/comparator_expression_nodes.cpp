#include "comparator_expression_nodes.h"

template class vaultdb::EqualNode<bool>;
template class vaultdb::EqualNode<emp::Bit>;

template class vaultdb::NotEqualNode<bool>;
template class vaultdb::NotEqualNode<emp::Bit>;

template class vaultdb::GreaterThanNode<bool>;
template class vaultdb::GreaterThanNode<emp::Bit>;

template class vaultdb::GreaterThanEqNode<bool>;
template class vaultdb::GreaterThanEqNode<emp::Bit>;


template class vaultdb::LessThanNode<bool>;
template class vaultdb::LessThanNode<emp::Bit>;

template class vaultdb::LessThanEqNode<bool>;
template class vaultdb::LessThanEqNode<emp::Bit>;


