#ifndef _MULTIPLE_UNION_H
#define _MULTIPLE_UNION_H

#include "operators/operator.h"
#include "operators/secure_sql_input.h"
#include "operators/basic_join.h"
#include "operators/union.h"

#include <vector>
#include <string>

namespace vaultdb {

    template<typename B>
    class MultipleUnion : public Operator<B> {
    public:
        MultipleUnion(const std::string &db,
                      const std::string &sql1,
                      const std::string &sql2,
                      const std::vector<int> &filter,
                      const SortDefinition &sort_def = SortDefinition());

        Operator<B> *clone() const override {
            return new MultipleUnion<B>(*this);
        }

        void updateCollation() override {
            this->sort_definition_ = collation_;
        }

    protected:
        QueryTable<B> *runSelf() override;
        OperatorType getType() const override { return OperatorType::MULTIPLE_UNION; } // use custom type
        std::string getParameters() const override { return ""; }
        bool operator==(const Operator<B> &other) const override {
            if (other.getType() != this->getType()) return false;
            return this->operatorEquality(other);
        }


    private:
        std::string db_;
        std::string sql1_;
        std::string sql2_;
        std::vector<int> filter_;
        SortDefinition collation_;
    };

} // namespace vaultdb

#endif //_MULTIPLE_UNION_H
