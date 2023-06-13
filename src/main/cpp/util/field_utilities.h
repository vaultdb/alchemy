#ifndef _FIELD_UTILITIES_H
#define _FIELD_UTILITIES_H


#include<type_traits>
#include <map>
#include <query_table/field/field.h>
#include <query_table/query_tuple.h>
#include <query_table/secure_tuple.h>
#include "query_table/field/field_type.h"
#include "common/defs.h"
#include "expression/expression_node.h"
#include "expression/comparator_expression_nodes.h"
#include "expression/generic_expression.h"
#include <expression/visitor/to_packed_expression_visitor.h>


namespace vaultdb {

    template <typename B> class Operator;

    class FieldUtilities {
    public:
        // size is in bytes
        static size_t getPhysicalSize(const FieldType &id, const size_t & str_length = 0);
        static emp::Float toFloat(const emp::Integer &input);

        template<typename B>
        static inline bool validateTypeAlignment(const Field<B> &field) {

            switch(field.getType()) {
                case FieldType::BOOL:
                    return field.payload_.which() == 0;
                case FieldType::INT:
                    return field.payload_.which() == 1;
                case FieldType::LONG:
                    return field.payload_.which() == 2;
                case FieldType::FLOAT:
                    return field.payload_.which() == 3;
                case FieldType::STRING:
                    return field.payload_.which() == 4;
                case FieldType::SECURE_BOOL:
                    return field.payload_.which() == 5;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING:
                    return field.payload_.which() == 6;
                case FieldType::SECURE_FLOAT:
                    return field.payload_.which() == 7;
                default:
                    return true; // uninitialized for FieldType::INVALID

            }

        }

        static void secret_share_send(const PlainTable *src, const int &src_idx, SecureTable *dst, const int &dst_idx,
                                      const int &party);

//        static void secret_share_send(const QueryTuple<bool> &src_tuple, QueryTuple<Bit> &dst_tuple, const int &party);
//        static void secret_share_recv(SecureTuple &dst_tuple, const int &party);

        static void secret_share_recv(SecureTable *dst, const int & idx, const int &party);


        // for template<typename T> case
        static bool extract_bool(const emp::Bit & b) { return b.reveal();  }
        static bool extract_bool(const bool & b) { return b; }

//        static PlainTuple revealTuple(const PlainTuple & p) {return p; }
//        static PlainTuple revealTuple(const SecureTuple  & s);

        // expected size in bits
        inline static int getExpectedSize(const FieldType & t, int str_length) {
            return TypeUtilities::getTypeSize(t) * ((t == FieldType::SECURE_STRING) ? str_length : 1);
        }

        static inline Field<bool> getBoolField(const bool & input) { return Field<bool>(FieldType::BOOL, input, 0); }
        static inline Field<emp::Bit> getBoolField(const emp::Bit & input) { return Field<emp::Bit>(FieldType::SECURE_BOOL, input, 0); }

        static std::string printTupleBits(const PlainTuple & p);
        static std::string printTupleBits(const SecureTuple & s);

        static bool getBoolPrimitive(const Field<bool> & input) { return input.getValue<bool>(); }
        static emp::Bit getBoolPrimitive(const Field<emp::Bit> & input) { return input.getValue<emp::Bit>(); }

        static bool select(const bool & choice, const bool & lhs, const bool & rhs);
        static emp::Bit select(const emp::Bit & choice, const emp::Bit & lhs, const emp::Bit & rhs);

        static BitPackingMetadata getBitPackingMetadata(const std::string & db_name);

        static inline emp::Integer addSignBit(const emp::Integer & src) {
            emp::Integer dst(src);
            dst.resize(src.bits.size() + 1);
            return dst;
        }

        static inline emp::Integer padInteger(const emp::Integer & src, const int & len) {
            if(src.size() == len) return src;

            emp::Integer dst(len, 0, PUBLIC);
            memcpy(dst.bits.data(), src.bits.data(), src.size() * TypeUtilities::getEmpBitSize());
            return dst;
        }



        template<typename B>
        static  Expression<B> *
        getEqualityPredicate(const uint32_t &lhs_idx, const QuerySchema &lhs, const uint32_t &rhs_idx,
                             const QuerySchema & rhs) {

            InputReference<B> *lhs_input = new InputReference<B>(lhs_idx, lhs,  rhs);
            InputReference<B> *rhs_input = new InputReference<B>(rhs_idx, lhs,  rhs);
            ExpressionNode<B> *equality_node = new EqualNode<B>(lhs_input, rhs_input);

            ToPackedExpressionVisitor pack_it(equality_node);
            ExpressionNode<B> *packed_predicate = pack_it.getRoot();

            GenericExpression<B> *g =  new GenericExpression<B>(packed_predicate, "predicate",
                                                                std::is_same_v<B, bool> ? FieldType::BOOL : FieldType::SECURE_BOOL);

            delete equality_node;
            delete lhs_input;
            delete rhs_input;
            return g;

        }

        // for use in joins
        // indexes are based on the concatenated tuple, not addressing each input to the join comparison individually
        template<typename B>
        static Expression<B> *
        getEqualityPredicate(const Operator<B> *lhs, const uint32_t &lhs_idx, const Operator<B> *rhs,
                             const uint32_t &rhs_idx) {

            return FieldUtilities::getEqualityPredicate<B>(lhs_idx, lhs->getOutputSchema(), rhs_idx, rhs->getOutputSchema());

        }


    };


}
#endif //_FIELD_UTILITIES_H
