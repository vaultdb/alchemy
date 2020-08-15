#include <iso646.h>
#pragma once
#include "emp-tool/emp-tool.h"
#include "type.h"
#include "type_id.h"
#include "common/defs.h"
#include <cstdint>
#include <memory>
#include <boost/variant.hpp>


class variant;
namespace vaultdb::types {
class Value {

public:


  Value();
    Value(int32_t val);
    Value(int64_t val);
    Value(bool val);
    Value(emp::Bit val);
    Value(TypeId type, const emp::Integer val);
    Value(double val);
    Value(float val);
    Value(const Value *val);
    Value(const Value & val);

    Value(const std::string & val);
    Value(emp::Float32 float32);
    Value(emp::Float float32);


    void setValue(const Value *v);
  // for unencrypted values, the TypeId is fixed.  This only becomes dynamic for the encrypted values
  // hence only take as input a TypeId for the latter.
  void setValue(int32_t val);
  void setValue(int64_t val);
  void setValue(bool val);
  void setValue(emp::Bit val);
  void setValue(TypeId type, emp::Integer val);
  void setValue(float val);
  void setValue(double val);
  void setValue(std::string aString);
  void setValue(emp::Float32 val);
  void setValue(emp::Float val);



  ~Value();

  TypeId getType() const;
  int64_t getInt64() const;
  int32_t getInt32() const;
  bool getBool() const;
  emp::Integer *getEmpInt() const ;
  emp::Bit *getEmpBit() const;
    std::string getVarchar() const;


    float getFloat32() const;
    double getFloat64() const;

    string getValueString() const;
    friend std::ostream& operator<<(std::ostream &strm, const types::Value &aValue);
    Value& operator=(const Value& other); // copy assign operator overload

    void serialize(bool *dst) const;

    emp::Float32 *getEmpFloat32() const ;
    emp::Float *getEmpFloat64() const ;
    Value reveal(EmpParty party) const;


    // comparators
    Value operator>=(const Value &rhs) const;
    Value operator>(const Value &rhs) const;
    Value operator<=(const Value &rhs) const;
    Value operator<(const Value &rhs) const;
    Value operator==(const Value &rhs) const;
    Value operator!=(const Value &rhs) const;


    // arithmetic expressions
    Value operator+(const Value& rhs)const;
    Value operator-(const Value& rhs)const;
    Value operator*(const Value& rhs)const;
    Value operator/(const Value& rhs)const;



protected:
  bool is_encrypted_;
  TypeId type_;
public:
    void setType(TypeId type);

protected:



  struct ValueStruct {
      boost::variant<bool, int32_t, int64_t, float_t, double_t, std::string> unencrypted_val;
    emp::Bit *emp_bit_;
    emp::Integer  *emp_integer_;
    emp::Float *emp_float_;
    emp::Float32 *emp_float32_;
  } value_   {false, nullptr, nullptr, nullptr, nullptr};
         // false, emp::Bit(false, emp::PUBLIC), emp::Integer(1, 0, emp::PUBLIC), emp::Float(24, 9, 0, emp::PUBLIC), emp::Float32(0.0, emp::PUBLIC)
 // };


};
} // namespace vaultdb::types

