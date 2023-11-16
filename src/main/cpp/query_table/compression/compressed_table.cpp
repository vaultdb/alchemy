#include "compressed_table.h"

using namespace vaultdb;

template<>
void CompressedTable<bool>::compareSwap(const bool &swap, const int &lhs_row, const int &rhs_row) {
    if(swap) {
        for(auto pos : column_encodings_) {
            auto lhs = pos.second->getField(lhs_row);
            auto rhs = pos.second->getField(rhs_row);
            pos.second->setField(lhs_row, rhs);
            pos.second->setField(rhs_row, lhs);
        }
    }
}

template<>
void CompressedTable<Bit>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {
    for(auto pos : column_encodings_) {
        auto lhs = pos.second->getField(lhs_row);
        auto rhs = pos.second->getField(rhs_row);
        Field<Bit>::compareAndSwap(swap, lhs, rhs);

        pos.second->setField(lhs_row, lhs);
        pos.second->setField(rhs_row, rhs);
    }
}


template class vaultdb::CompressedTable<bool>;
template class vaultdb::CompressedTable<emp::Bit>;
