//
// Created by madhav on 1/7/20.
//

#ifndef TESTING_QUERY_TABLE_H
#define TESTING_QUERY_TABLE_H

#include "emp-tool/emp-tool.h"
#include "query_schema.h"
#include "query_tuple.h"
#include <memory>
using namespace vaultdb;

template <class BaseIterator> class DereferenceIterator : public BaseIterator {
public:
  using value_type = typename BaseIterator::value_type::element_type;
  using pointer = value_type *;
  using reference = value_type &;

  explicit DereferenceIterator(const BaseIterator &other)
      : BaseIterator(other) {}

  reference operator*() const { return *(this->BaseIterator::operator*()); }
  pointer operator->() const { return this->BaseIterator::operator*().get(); }
  reference operator[](size_t n) const {
    return *(this->BaseIterator::operator[](n));
  }
};

template <typename Iterator>
DereferenceIterator<Iterator> dereference_iterator(Iterator t) {
  return DereferenceIterator<Iterator>(t);
}

class QueryTable {
private:
  void *base_ptr;
  QueryTuple *tuples_2;

  const unsigned int num_tuples_;
  std::vector<std::unique_ptr<vaultdb::QueryTuple>> tuples_;
  std::unique_ptr<QuerySchema> schema_;
  const bool is_encrypted_;

public:
  const bool GetIsEncrypted() const;
  QueryTable(int num_tuples);
  QueryTable(bool is_encrypted, int num_tuples);
  void AllocateQueryTuples();
  void SetSchema(std::unique_ptr<QuerySchema> s);
  void SetSchema(const QuerySchema* s);
  const QuerySchema *GetSchema() const;
  QueryTuple *GetTuple(int idx) const;
  unsigned int GetNumTuples() const;

  typedef DereferenceIterator<
      std::vector<std::unique_ptr<vaultdb::QueryTuple>>::iterator>
      iterator;
  typedef DereferenceIterator<
      std::vector<std::unique_ptr<vaultdb::QueryTuple>>::const_iterator>
      const_iterator;

  iterator begin() { return dereference_iterator(tuples_.begin()); }
  const_iterator begin() const { return dereference_iterator(tuples_.begin()); }
  const_iterator cbegin() const {
    return dereference_iterator(tuples_.cbegin());
  }
  iterator end() { return dereference_iterator(tuples_.end()); }
  const_iterator end() const { return dereference_iterator(tuples_.end()); }
  const_iterator cend() const { return dereference_iterator(tuples_.cend()); }
  std::unique_ptr<QuerySchema> ReleaseSchema();
};

#endif // TESTING_QUERY_TABLE_H
