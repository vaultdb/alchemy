#ifndef _SHRINKWRAP_H
#define _SHRINKWRAP_H

#include "operator.h"

// obliviously truncate a query output
// sort to put dummies at the end
namespace vaultdb {
    template<typename B>
    class Shrinkwrap : public Operator<B> {
    public:
        Shrinkwrap(Operator<B> *child, const size_t & output_cardinality);
        Shrinkwrap(shared_ptr<QueryTable<B>> & input, const size_t & output_cardinality);
        ~Shrinkwrap() = default;
    protected:
        std::shared_ptr<QueryTable<B> > runSelf()  override;
        string getOperatorType() const override;
        string getParameters() const override;


    private:
        size_t cardinality_bound_;
    };


}
#endif //VAULTDB_EMP_SHRINKWRAP_H
