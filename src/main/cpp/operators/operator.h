#ifndef _OPERATOR_H
#define _OPERATOR_H

#include <functional>
#include <vector>
#include <query_table/query_table.h>
#include <common/defs.h>
#include <util/system_configuration.h>

using namespace std;



// for query operator tree and perhaps operator factory
// operator may run in the clear or in MPC
namespace  vaultdb {

    template<typename B> class TableInput;

    template<typename B> class Operator;

    typedef  Operator<bool> PlainOperator;
    typedef  Operator<emp::Bit> SecureOperator;


    template<typename B>
    class Operator {

    protected:
        Operator *parent_ = nullptr;
        Operator *lhs_child_ = nullptr;
        Operator *rhs_child_ = nullptr;
        QueryTable<B> *output_ = nullptr;
        SortDefinition sort_definition_; // start out with empty sort
        QuerySchema output_schema_;
        time_point<high_resolution_clock> start_time_, end_time_;
        size_t start_gate_cnt_ = 0L, gate_cnt_ = 0L;
        double runtime_ms_ = 0.0;
        SystemConfiguration & system_conf_;
        bool operator_executed_ = false; // set when runSelf() executed once
        size_t output_cardinality_ = 0L;




    public:

        Operator(const SortDefinition & sorted_on = SortDefinition()) : sort_definition_(sorted_on), system_conf_(SystemConfiguration::getInstance()) {}

        virtual ~Operator() {
            if(output_ != nullptr)  delete output_;
            if(lhs_child_  != nullptr)   delete lhs_child_;
            if(rhs_child_ != nullptr)   delete rhs_child_;
        }

        Operator(QueryTable<B> * lhs, const SortDefinition & sorted_on = SortDefinition());
        Operator(QueryTable<B> *lhs, QueryTable<B> *rhs, const SortDefinition & sorted_on = SortDefinition());

        Operator(Operator *child, const SortDefinition & sorted_on = SortDefinition());
        Operator(Operator *lhs, Operator *rhs, const SortDefinition & sorted_on = SortDefinition());
        // cloning children, omitting parent so as to not copy entire tree
        Operator(const Operator &o) :
            sort_definition_(o.sort_definition_), system_conf_(SystemConfiguration::getInstance()), output_schema_(o.output_schema_),
            operator_executed_(o.operator_executed_), output_cardinality_(o.output_cardinality_), operator_id_(o.operator_id_)  {

            if(o.lhs_child_ != nullptr) lhs_child_ = o.lhs_child_->clone();
            if(o.rhs_child_ != nullptr) rhs_child_ = o.rhs_child_->clone();
            if(o.output_ != nullptr) output_ = o.output_->clone();
        }

        virtual Operator<B> *clone() const = 0;

        virtual bool operator==(const Operator<B> &other) const = 0;

        bool operator!=(const Operator<B> & other) const {
            return !(*this == other);
        }

        // align collation  wrt child nodes
        // operator-specific logic
        virtual void updateCollation() = 0;


//        QueryTable<B> sortByDummyTag(QueryTable<B> & table);
//        QueryTable<B> shrinkwrapToTrueCardinality(QueryTable<B> & table, bool isSorted);
//

        // recurses first, then invokes runSelf method
        QueryTable<B>  *run();
        std::string printTree() const;
        std::string printMinCostPlan();
        std::string toString() const;
        std::string toSortOptimizedString() const;
        size_t getGateCount() const { return gate_cnt_; }
        double getRuntimeMs() const { return runtime_ms_; }
        inline int getOperatorId() const {     return operator_id_; }
        inline void setOperatorId(int op_id) { operator_id_ = op_id; }
        inline QueryTable<B> *getOutput() {
            if(!operator_executed_) { // if we haven't run it yet
                output_ = run();
            }
            return output_;
        }

        inline const QueryTable<B> *getImmutableOutput() const {
            return output_;
        }

        virtual OperatorType getType() const = 0;

        inline Operator *getParent() const { return parent_; }

        inline Operator *getChild(int idx = 0) const {
            if(idx == 0) return lhs_child_;
            return rhs_child_;
        }

        void setParent(Operator *p) {
            parent_ = p;

        }

        inline void setChild(Operator *c, int idx = 0) {
            if(idx == 0) {
                lhs_child_ = c;
                if(c != nullptr) lhs_child_->setParent(this);

            }
            else {
                rhs_child_ = c;
                if(c != nullptr) rhs_child_->setParent(this);
            }

            if(c != nullptr) this->updateCollation();
        }

        inline void setChildWithNoUpdateCollation(Operator *c, int idx = 0) {
            if(idx == 0) {
                lhs_child_ = c;
                lhs_child_->setParent(this);

            }
            else {
                rhs_child_ = c;
                rhs_child_->setParent(this);
            }
        }

        bool isLeaf() const { return  (lhs_child_ == nullptr); }
        
        inline SortDefinition  getSortOrder() const {
            assert(output_ == nullptr || sort_definition_ == output_->getSortOrder()); // check that output table is aligned with operator's sort order
            // need to cache this locally in case output not initialized yet
            return sort_definition_;
        }

        void setSortOrder(const SortDefinition & sort_def) {
            if(sort_def != sort_definition_) {
                sort_definition_ = sort_def;
                this->updateCollation();
            }
        }

        QuerySchema getOutputSchema() const { return output_schema_; }
        void setSchema(QuerySchema new_schema) { output_schema_.setSchema(new_schema); }
        size_t getOutputCardinality() const { return output_cardinality_; }
        void setOutputCardinality(size_t input_card) { output_cardinality_ = input_card; }

        // returns summed cost over this operator (as root) and all of its children
        size_t planCost() const;

		//returns sum of real gate costs
		size_t planGateCount() const;

		//returns total runtime of this operator and its children
		double planRuntime() const;

        inline string getTypeString() const {
            switch(this->getType()) {
                case OperatorType::SQL_INPUT:
                    return "SqlInput";
                case OperatorType::ZK_SQL_INPUT:
                    return "ZkSqlInput";
                case OperatorType::SECURE_SQL_INPUT:
                    return "SecureSqlInput";
                case OperatorType::TABLE_INPUT:
                        return "TableInput";
                case OperatorType::CSV_INPUT:
                        return "CsvInput";
                case OperatorType::MERGE_INPUT:
                        return "MergeInput";
                case OperatorType::FILTER:
                        return "Filter";
                case OperatorType::PROJECT:
                    return "Project";
                case OperatorType::NESTED_LOOP_JOIN:
                    return "BasicJoin";
                case OperatorType::KEYED_NESTED_LOOP_JOIN:
                    return "KeyedJoin";
                case OperatorType::KEYED_SORT_MERGE_JOIN:
                    return "SortMergeJoin";
                case OperatorType::MERGE_JOIN:
                    return "MergeJoin";
                case OperatorType::SORT:
                    return "Sort";
                case OperatorType::SHRINKWRAP:
                    return "Shrinkwrap";
                case OperatorType::SCALAR_AGGREGATE:
                    return "ScalarAggregate";
                case OperatorType::SORT_MERGE_AGGREGATE:
                    return "SortMergeAggregate";
                case OperatorType::NESTED_LOOP_AGGREGATE:
                    return "NestedLoopAggregate";
                case OperatorType::UNION:
                    return "Union";
                default:
                    throw;
            }
        }

        bool is_optimized_ = false;

    protected:
        // to be implemented by the operator classes, e.g., sort, filter, et cetera
        virtual QueryTable<B> *runSelf() = 0;

        inline bool operatorEquality(const Operator<Bit> &other) const {
            if(std::is_same_v<B, bool>) return false; // secure operator
            if (!Utilities::vectorEquality(this->getSortOrder(), other.getSortOrder())) return false;
            if (this->output_schema_ != other.getOutputSchema()) return false;
            if (this->output_cardinality_ != other.getOutputCardinality()) return false;

            // don't really need to check outputs - can just check control flow
//            if((this->output_ != nullptr) && (other.getImmutableOutput() != nullptr))
//            {
//                SecureTable *this_output = ((SecureTable *) this->output_);
//                Bit output_equality = (*this_output == *other.getImmutableOutput());
//                if(!output_equality.reveal<bool>())
//                    return false;
//            }

            SecureOperator *other_lhs_child = other.getChild(0);
            SecureOperator *other_rhs_child = other.getChild(1);
            SecureOperator *this_lhs_child = (SecureOperator *) this->getChild(0);
            SecureOperator *this_rhs_child = (SecureOperator *) this->getChild(1);


            bool lhs = (this_lhs_child != nullptr && other_lhs_child != nullptr) ? ( *this_lhs_child == *other_lhs_child ) : (other_lhs_child == this_lhs_child);
            bool rhs = (this_rhs_child != nullptr && other_rhs_child != nullptr) ? ( *this_rhs_child == *other_rhs_child ) : (other_rhs_child == this_rhs_child);
            return lhs && rhs; // true for self and children
        }

        // to handle template specialization
        inline bool operatorEquality(const Operator<bool> &other) const {

            if(std::is_same_v<B, Bit>) return false; // this is PlainOperator
            if (!Utilities::vectorEquality(this->getSortOrder(), other.getSortOrder())) return false;
            if (this->output_schema_ != other.getOutputSchema()) return false;
            if (this->output_cardinality_ != other.getOutputCardinality()) return false;

            // don't really need to check outputs -  check control flow
//            if ((this->output_ != nullptr) && (other.getImmutableOutput()) != nullptr ){
//                PlainTable *this_output = ((PlainTable *) this->output_);
//                if(*this_output != *other.getImmutableOutput())
//                    return false;
//            }

            PlainOperator *other_lhs_child = other.getChild(0);
            PlainOperator *other_rhs_child = other.getChild(1);
            PlainOperator *this_lhs_child = (PlainOperator *) this->getChild();
            PlainOperator *this_rhs_child = (PlainOperator *) this->getChild(1);


            bool lhs = (this_lhs_child != nullptr && other_lhs_child != nullptr) ? ( *this_lhs_child == *other_lhs_child ) : (other_lhs_child == this_lhs_child); 
            bool rhs = (this_rhs_child != nullptr && other_rhs_child != nullptr) ? ( *this_rhs_child == *other_rhs_child ) : (other_rhs_child == this_rhs_child);
            return lhs && rhs; // true for self and children
        }

        virtual std::string getParameters() const = 0;
        inline void reset() {
            if(output_ != nullptr) {
                delete output_;
                output_ = nullptr;
            }

            operator_executed_ = false;
        }

    private:
        std::string printHelper(const std::string & prefix) const;
        std::string printMinCostHelper(const std::string & prefix);
        int operator_id_ = -1;

    };



    // essentially CommonTableExpression, written here to avoid forward declarations
    template<typename B>
    class TableInput : public Operator<B> {

    public:
        TableInput(QueryTable<B>  *input) {

           this->output_ = input; // point to input table
            this->output_schema_ = input->getSchema();
            this->sort_definition_ = Operator<B>::output_->getSortOrder();
            this->operator_executed_ = true;
            this->output_cardinality_ = input->getTupleCount();
        }

        QueryTable<B> *runSelf() override {
            return  this->output_;
        }

        Operator<B> *clone() const override {
            return new TableInput<B>(this->output_->clone());
        }

        void updateCollation() override {
            this->sort_definition_ = this->output_->getSortOrder();
        }

        virtual ~TableInput() = default;

         bool operator==(const Operator<B> &other) const override {
             if(other.getType() != OperatorType::TABLE_INPUT) return false;
             // no operator-sepecific parameters to compare
            return this->operatorEquality(other);
        }

    protected:
        OperatorType getType() const override {
            return OperatorType::TABLE_INPUT;
        }

        string getParameters() const override {
            return std::string();
        }

    };

    std::ostream &operator<<(std::ostream &os, const PlainOperator &op);
    std::ostream &operator<<(std::ostream &os, const SecureOperator &op);

}

#endif //_OPERATOR_H
