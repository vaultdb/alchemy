#include <querytable/query_table.h>
#include <memory>
#include <defs.h>
#include <operator.h>

class Sort : public Operator {
    SortDefinition sortDefinition;

public:
    Sort(const SortDefinition & aSortDefinition, std::shared_ptr<Operator> &child);


    std::shared_ptr<QueryTable> runSelf() override;
};

//void Sort(QueryTable *input, SortDefinition &s);
