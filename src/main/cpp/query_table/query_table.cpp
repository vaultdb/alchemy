#include <util/data_utilities.h>
#include <util/type_utilities.h>
#include <util/field_utilities.h>
#include <util/logger.h>
#include "query_table.h"
#include "plain_tuple.h"
#include "operators/sort.h"
#include "table_factory.h"


using namespace vaultdb;





template<typename B>
PlainTable *QueryTable<B>::reveal(const int &party) {
    {
        if(!this->isEncrypted()) { return (QueryTable<bool> *) this;  }
        auto table = (QueryTable<Bit> *) this;
        QuerySchema dst_schema = QuerySchema::toPlain(this->schema_);
        SortDefinition collation = table->getSortOrder();

        // if it is not sorted so that the dummies are at the end, sort it now.
        if(collation.empty() || collation[0].first != -1) {
            SortDefinition tmp{ColumnSort(-1, SortDirection::ASCENDING)};
            for(int i = 0; i < collation.size(); ++i) {
                tmp.emplace_back(collation[i]);
            }
            Sort sort(((QueryTable<Bit> *) this)->clone(), tmp);
            sort.setOperatorId(-2);
            table = sort.getOutput()->clone();
        }

        // count # of real (not dummy) rows
        int row_cnt = 0;
        while(!table->getDummyTag(row_cnt).reveal()
            && (row_cnt < table->getTupleCount())) {
            ++row_cnt;
        }

        auto dst_table = TableFactory<bool>::getTable(row_cnt, dst_schema, collation);

        for(int i = 0; i < row_cnt; ++i)  {
            PlainTuple dst_tuple = table->revealRow(i, dst_schema, party);
            dst_table->putTuple(i, dst_tuple);
        }

        if(table != (QueryTable<Bit> *) this) // extra sort
            delete table;

        return dst_table;
    }
}


template<typename B>
PlainTable *QueryTable<B>::revealInsecure(const int &party) const {

    if(!this->isEncrypted()) {
        return (QueryTable<bool> *) this;
    }

    QuerySchema dst_schema = QuerySchema::toPlain(this->schema_);

    auto dst_table = TableFactory<bool>::getTable(this->tuple_cnt_, dst_schema, this->getSortOrder());

    for(uint32_t i = 0; i < this->tuple_cnt_; ++i)  {
        PlainTuple t = this->revealRow(i, dst_schema, party);
        dst_table->putTuple(i, t);
    }
    return dst_table;

}



template class vaultdb::QueryTable<bool>;
template class vaultdb::QueryTable<emp::Bit>;
