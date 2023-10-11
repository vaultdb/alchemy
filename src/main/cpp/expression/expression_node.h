#ifndef _EXPRESSION_NODE_H
#define _EXPRESSION_NODE_H

#include <query_table/query_tuple.h>
#include <query_table/query_table.h>

#include <expression/expression_kind.h>
#include "visitor/expression_visitor.h"
#include <query_table/field/field_factory.h>

// building blocks for composing expressions
// initially building around Calcite JSON expression format, but may extend this to join and filter expressions (etc.)

namespace vaultdb {

    template<typename B> class QueryTable;

    // all expressions implement the call method in this
    template<typename B>
    class ExpressionNode {
    public:
        ExpressionNode(ExpressionNode<B> *lhs);

        ExpressionNode(ExpressionNode<B> *lhs, ExpressionNode<B> *rhs);



        virtual Field<B> call(const QueryTuple<B> & target) const = 0;
        virtual Field<B> call(const QueryTable<B>  *src, const int & row) const = 0;
        virtual Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const = 0;

        virtual ExpressionNode<B> *clone() const = 0;

        virtual ExpressionKind kind() const = 0;
        virtual void accept(ExpressionVisitor<B> * visitor) = 0;
        virtual ~ExpressionNode() {
            if(lhs_ != nullptr)  {  delete lhs_; }
            if(rhs_ != nullptr)  { delete rhs_; }
        }
        std::string toString();
        virtual bool operator==(const ExpressionNode<B> &other) const = 0;


        ExpressionNode<B> *lhs_ = nullptr;
        ExpressionNode<B> *rhs_ = nullptr;
        QueryFieldDesc output_schema_;

    };

    static std::ostream &operator<<(std::ostream &os,  ExpressionNode<bool> &expression);
    static std::ostream &operator<<(std::ostream &os,  ExpressionNode<Bit> &expression);


    // read a field from a tuple
    template<typename B>
    class InputReference : public ExpressionNode<B> {
    public:
        InputReference(const InputReference<B> & src)
                : ExpressionNode<B>(nullptr), read_idx_(src.read_idx_),
                   binary_mode_(src.binary_mode_), output_idx_(src.output_idx_), read_lhs_(src.read_lhs_) {
            this->output_schema_ = src.output_schema_;
        }

        InputReference(const uint32_t & read_idx, const QueryFieldDesc &schema)
                : ExpressionNode<B>(nullptr), read_idx_(read_idx), output_idx_(read_idx_) {
            this->output_schema_ = schema;
        }
        InputReference(const uint32_t & read_idx, const QuerySchema &schema)
            : ExpressionNode<B>(nullptr), read_idx_(read_idx), output_idx_(read_idx) {
            this->output_schema_ = schema.getField(read_idx);
        }
         // needed for binary (lhs, rhs) invocation
        InputReference(const uint32_t & read_idx, const QuerySchema & lhs_schema, const QuerySchema & rhs_schema)
                : ExpressionNode<B>(nullptr), output_idx_(read_idx), binary_mode_(true), read_idx_(read_idx) {

            if(rhs_schema.getFieldCount() == -1) { // empty placeholder on rhs, treat it like single QuerySchema input
                binary_mode_ = false;
                this->output_schema_ = lhs_schema.getField(read_idx_);
                return;
            }

            if(read_idx >= lhs_schema.getFieldCount()) {
                read_lhs_ = false;
                read_idx_ = read_idx - lhs_schema.getFieldCount();
                this->output_schema_ = rhs_schema.getField(read_idx_);
            }
            else {
                this->output_schema_ = lhs_schema.getField(read_idx_);
            }
        }

        ~InputReference() = default;

        bool operator==(const ExpressionNode<B> &other) const override {
            if(other.kind() != ExpressionKind::INPUT_REF) {
                return false;
            }
            const InputReference<B> &other_ref = static_cast<const InputReference<B> &>(other);
            return (other_ref.read_idx_ == read_idx_) && (other_ref.output_idx_ == output_idx_) &&
                   (other_ref.binary_mode_ == binary_mode_) && (other_ref.read_lhs_ == read_lhs_);
        }


        inline Field<B> call(const QueryTuple<B> & target) const override {
            return target.getField(read_idx_);
        }
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            assert(!binary_mode_);
            return src->getField(row, read_idx_);
        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            assert(binary_mode_);
            return (read_lhs_) ?
                lhs->getField(lhs_row, read_idx_) :
                rhs->getField(rhs_row, read_idx_);
        }


        ExpressionKind kind() const override {     return ExpressionKind::INPUT_REF;
        }

        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);

        }

        ExpressionNode<B> *clone() const override {
            return new InputReference<B>(*this);
        }

        int read_idx_;
        uint32_t output_idx_;
        bool binary_mode_ = false;
        bool read_lhs_ = true;
    };


    template<typename B>
    class PackedInputReference : public ExpressionNode<B> {
    public:
        PackedInputReference(const PackedInputReference<B> & src)
                : ExpressionNode<B>(nullptr), read_idx_(src.read_idx_),
                  binary_mode_(src.binary_mode_), output_idx_(src.output_idx_), read_lhs_(src.read_lhs_) {
            this->output_schema_ = src.output_schema_;
        }

        PackedInputReference(const InputReference<B> & src)
                : ExpressionNode<B>(nullptr), read_idx_(src.read_idx_),
                  binary_mode_(src.binary_mode_), output_idx_(src.output_idx_), read_lhs_(src.read_lhs_) {
            this->output_schema_ = src.output_schema_;
        }


        PackedInputReference(const uint32_t & read_idx, const QueryFieldDesc &schema)
                : ExpressionNode<B>(nullptr), read_idx_(read_idx), output_idx_(read_idx_) {
            this->output_schema_ = schema;
        }
        PackedInputReference(const uint32_t & read_idx, const QuerySchema &schema)
                : ExpressionNode<B>(nullptr), read_idx_(read_idx), output_idx_(read_idx) {
            this->output_schema_ = schema.getField(read_idx);
        }
        // needed for binary (lhs, rhs) invocation
        PackedInputReference(const uint32_t & read_idx, const QuerySchema & lhs_schema, const QuerySchema & rhs_schema)
                : ExpressionNode<B>(nullptr), binary_mode_(true), output_idx_(read_idx), read_idx_(read_idx) {

            if(rhs_schema.getFieldCount() == -1) { // empty placeholder on rhs, treat it like single QuerySchema input
                binary_mode_ = false;
                this->output_schema_ = lhs_schema.getField(read_idx_);
                return;
            }

            if(read_idx >= lhs_schema.getFieldCount()) {
                read_lhs_ = false;
                read_idx_ = read_idx - lhs_schema.getFieldCount();
                this->output_schema_ = rhs_schema.getField(read_idx_);
            }
            else {
                this->output_schema_ = lhs_schema.getField(read_idx_);
            }
        }

        ~PackedInputReference() = default;

         bool operator==(const ExpressionNode<B> &other) const override {
            if(other.kind() != ExpressionKind::PACKED_INPUT_REF) {
                return false;
            }
            const PackedInputReference<B> &other_ref = static_cast<const PackedInputReference<B> &>(other);
            return (other_ref.read_idx_ == read_idx_) && (other_ref.output_idx_ == output_idx_) &&
                   (other_ref.binary_mode_ == binary_mode_) && (other_ref.read_lhs_ == read_lhs_);
        }

        inline Field<B> call(const QueryTuple<B> & target) const override {
            return target.getPackedField(read_idx_);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            assert(!binary_mode_);
            return src->getPackedField(row, read_idx_);
        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            assert(binary_mode_);
            return (read_lhs_) ?
                   lhs->getPackedField(lhs_row, read_idx_) :
                   rhs->getPackedField(rhs_row, read_idx_);        
		}


        ExpressionKind kind() const override {     return ExpressionKind::PACKED_INPUT_REF;  }

        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);

        }

        ExpressionNode<B> *clone() const override {
            return new PackedInputReference<B>(*this);
        }

        uint32_t read_idx_;
        uint32_t output_idx_;
        bool binary_mode_ = false;
        bool read_lhs_ = true;
    };

    // leave output_schema_ blank initially and adjust automatically based on any bit packing of parent
    template<typename B>
    class LiteralNode : public ExpressionNode<B> {
    public:
        LiteralNode(const Field<B> & literal) : ExpressionNode<B>(nullptr), payload_(literal) {

        }
        ~LiteralNode() = default;

         bool operator==(const ExpressionNode<B> &other) const override;

        Field<B> call(const QueryTuple<B> & target) const override { return  payload_; }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return payload_;
        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            return payload_;
        }

        ExpressionKind kind() const override {    return ExpressionKind::LITERAL; }

        void accept(ExpressionVisitor<B> *visitor) override {  visitor->visit(*this); }

        LiteralNode<emp::Bit>  *toSecure() const {
            return new LiteralNode<emp::Bit>(payload_.secret_share());

        }

        ExpressionNode<B> *clone() const override {
            return  new LiteralNode<B>(payload_);
        }



        Field<B> payload_;



    };


    // placeholder that always returns true for when our predicate is NULL
    template<typename B>
    class NoOp : public ExpressionNode<B> {
    public:
        NoOp() : ExpressionNode<B>(nullptr) {
            B payload = true;
            FieldType type = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;
            output_ = Field<B>(type, payload);

        }
        ~NoOp() = default;

        bool operator==(const ExpressionNode<B> &other) const override;

        Field<B> call(const QueryTuple<B> & target) const override { return  output_; }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return output_;
        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            return output_;
        }

        ExpressionKind kind() const override {    return ExpressionKind::NO_OP; }

        void accept(ExpressionVisitor<B> *visitor) override {  visitor->visit(*this); }

        NoOp<emp::Bit>  *toSecure() const { return new NoOp<emp::Bit>(); }

        ExpressionNode<B> *clone() const override { return  new NoOp<B>(); }


        Field<B> output_;

    };



    template<typename B>
    class CastNode : public ExpressionNode<B> {
    public:
        CastNode(ExpressionNode<B> *input, const FieldType &dst_type) : ExpressionNode<B>(input),  dst_type_(dst_type) { }

        ~CastNode() = default;

        bool operator==(const ExpressionNode<B> &other) const override {
            if(other.kind() != ExpressionKind::CAST) {
                return false;
            }
            const CastNode<B> &other_ref = static_cast<const CastNode<B> &>(other);
            return (other_ref.dst_type_ == dst_type_);
        }

        inline Field<B> call(const QueryTuple<B> &target) const override {
            Field<B> src = ExpressionNode<B>::lhs_->call(target);
            return castField(src);
        }

        inline Field<B> call(const QueryTable<B> *src, const int &row) const override {
            Field<B> field = ExpressionNode<B>::lhs_->call(src, row);
            return castField(field);
        }

        Field<B>  call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> field = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            return castField(field);
        }

        ExpressionKind kind() const override {     return ExpressionKind::CAST; }

        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            return new CastNode<B>(ExpressionNode<B>::lhs_, dst_type_);
        }

        FieldType dst_type_;

    private:
        inline Field<B> castField(const Field<B> &src) const {
            switch (dst_type_) {
                case FieldType::INT:
                case FieldType::SECURE_INT:
                    return FieldFactory<B>::toInt(src);
                case FieldType::LONG:
                case FieldType::SECURE_LONG:
                    return FieldFactory<B>::toLong(src);
                case FieldType::FLOAT:
                case FieldType::SECURE_FLOAT:
                    return FieldFactory<B>::toFloat(src);
                default:
                    throw;
            }

        }

    };




}





#endif //_EXPRESSION_NODE_H
