#include <util/data_utilities.h>
#include "operator.h"
#include "util/logger.h"
#include "sort.h"
#include <ctime>
#include "opt/operator_cost_model.h"
#include <cmath>

using namespace vaultdb;
using namespace Logging;

template<typename B>
Operator<B>::Operator(QueryTable<B> *lhs, const SortDefinition & sorted_on)
: sort_definition_(sorted_on), system_conf_(SystemConfiguration::getInstance()) {
    lhs_child_ = new TableInput<B>(lhs);
    output_schema_ = lhs->getSchema(); // copy up child schema by default

    if(sorted_on.empty())
        sort_definition_ = lhs->getSortOrder();
}

template<typename B>
Operator<B>::Operator(QueryTable<B> *lhs, QueryTable<B> *rhs, const SortDefinition & sorted_on)
    : sort_definition_(sorted_on), lhs_child_(new TableInput(lhs)), rhs_child_(new TableInput(rhs)), system_conf_(SystemConfiguration::getInstance())  {

}

template<typename B>
Operator<B>::Operator(Operator *child, const SortDefinition & sorted_on)
: sort_definition_(sorted_on), lhs_child_(child), system_conf_(SystemConfiguration::getInstance())   {

     child->setParent(this);

    output_schema_ = child->output_schema_;
    output_cardinality_ = child->output_cardinality_; // default to passing on child cardinality
    if(sorted_on.empty())
        sort_definition_ = child->sort_definition_;

}

template<typename B>
Operator<B>::Operator(Operator *lhs, Operator *rhs, const SortDefinition & sorted_on)
     : sort_definition_(sorted_on),  lhs_child_(lhs), rhs_child_(rhs), system_conf_(SystemConfiguration::getInstance())  {

    lhs->setParent(this);
    rhs->setParent(this);

}


template<typename B>
QueryTable<B> *Operator<B>::run() {
    if (operator_executed_) // prevent duplicate executions of operator
        return output_;


    output_ = runSelf(); // delegated to children
    gate_cnt_ = system_conf_.andGateCount() - start_gate_cnt_;
    end_time_ = high_resolution_clock::now();
    runtime_ms_ = time_from(start_time_)/1e3;

    if(std::is_same_v<B, Bit> && this->getOperatorId() >= -1) {
		Logger* log = get_log();

        log->write("Operator #" + std::to_string(this->getOperatorId()) + " " + getTypeString() + 
				" ran for " + std::to_string(runtime_ms_) + " ms, " + 
				"gate count: " + std::to_string(gate_cnt_) + 
				" output cardinality: " + std::to_string(output_->getTupleCount()) + 
				", row width=" + std::to_string(output_schema_.size()) + "\n", Level::DEBUG);

        if (gate_cnt_ > 0) {
            size_t estimated_gates = OperatorCostModel::operatorCost((SecureOperator *) this);
            float relative_error = std::fabs(((float) estimated_gates) - ((float) gate_cnt_)) / (float) gate_cnt_ * 100.0;
            log->write("Estimated cost for" + this->toString() + " : " + std::to_string(estimated_gates) +
                    ", Observed gates: " + std::to_string(gate_cnt_) +
                    ", Error rate(%) : " + std::to_string(relative_error) + "\n", Level::DEBUG);
        }
    }

    operator_executed_ = true;
    sort_definition_ = output_->getSortOrder(); // update this if needed

    if(lhs_child_)     lhs_child_->reset();
    if(rhs_child_)     rhs_child_->reset();


    return output_;
}


template<typename B>
std::string Operator<B>::printTree() const {
    return printHelper("");

}

template<typename B>
std::string Operator<B>::printHelper(const string &prefix) const {
    stringstream  ss;
    ss << prefix << toString() << endl;
    string indent = prefix + "    ";
    if(lhs_child_)   ss << lhs_child_->printHelper(indent);
    if(rhs_child_)   ss << rhs_child_->printHelper(indent);

    return ss.str();
}


template<typename B>
std::string Operator<B>::toString() const {
    stringstream  ss;

    string bool_type = (std::is_same_v<B, bool>) ? "bool" : "Bit";
    ss << "#" << operator_id_ << ": " << getTypeString() << "<" << bool_type << "> ";

    string params = getParameters();
    if(!params.empty())
        ss << "(" << params << ") ";

    ss << ": " <<  output_schema_ <<    " order by: " << DataUtilities::printSortDefinition(sort_definition_);
    if(std::is_same_v<B, Bit>) {
        size_t estimated_gates = OperatorCostModel::operatorCost((SecureOperator *) this);
        ss << ", cost est: " << estimated_gates;
        ss << ", output card: " << output_cardinality_;
    }
    return ss.str();

}

template<typename B>
std::string Operator<B>::printMinCostPlan() {
    return printMinCostHelper("");

}

template<typename B>
std::string Operator<B>::printMinCostHelper(const string &prefix) {
    stringstream  ss;
    ss << prefix << toSortOptimizedString() << endl;
    string indent = prefix + "    ";

    if(lhs_child_){
        if(lhs_child_->getParent() == nullptr)
            lhs_child_->setParent(this);
        ss << lhs_child_->printMinCostHelper(indent);
    }
    if(rhs_child_){
        if(rhs_child_->getParent() == nullptr)
            rhs_child_->setParent(this);
        ss << rhs_child_->printMinCostHelper(indent);
    }

    return ss.str();
}

template<typename B>
std::string Operator<B>::toSortOptimizedString() const {
    stringstream  ss;

    ss << "#" << operator_id_ << ": " << getTypeString();
    ss << " order by: " << DataUtilities::printSortDefinition(sort_definition_);

    // if operator id is -1, then this sort is inserted
    if(operator_id_ == -1){
        if(parent_->getType() == OperatorType::SORT_MERGE_AGGREGATE)
            ss << " | parent : " << parent_->getOperatorId() << ", child : " << lhs_child_->getOperatorId();
        else if(parent_->getType() == OperatorType::KEYED_SORT_MERGE_JOIN)
            ss << " | parent : " << parent_->getOperatorId() << ", child : " << lhs_child_->getOperatorId();
    }
    return ss.str();

}

/*
template<typename B>
QueryTable<B> Operator<B>::sortByDummyTag(QueryTable<B> &table) {
    SortDefinition table_sort_def;
    table_sort_def.push_back(pair<int32_t, SortDirection>(-1, SortDirection::ASCENDING));
    SortDefinition originOrder = table.getSortOrder();
    for(size_t i = 0; i < table.getSchema()->getFieldCount(); i++) {
        table_sort_def.push_back(pair<int32_t, SortDirection>(i, SortDirection::ASCENDING));
    }
    Sort table_sort(table, table_sort_def);
    table_sort.setOperatorId(-1);
    table = table_sort.run();
    table.setSortOrder(originOrder);

    return table;
}

template<typename B>
QueryTable<B> Operator<B>::shrinkwrapToTrueCardinality(QueryTable<B> &table, bool isSorted) {
    // Get true cardinality
    uint32_t trueCardinality;
    if(table.party_ == ALICE) {
        trueCardinality = table.plain_table_->getTrueTupleCount();
        table.netio_->send_data(&trueCardinality,4);
    }
    else {
        table.netio_->recv_data(&trueCardinality,4);
    }

    // Shrinkwrap output to true cardinality
    if(!isSorted) {
        table = sortByDummyTag(table);
    }
    table.resize(trueCardinality);

    return table;
*/

std::ostream &vaultdb::operator<<(std::ostream &os, const PlainOperator &op) {
    os << op.printTree();
    return os;
}

std::ostream &vaultdb::operator<<(std::ostream &os, const SecureOperator &op) {
    os << op.printTree();
    return os;
}




template<typename B>
size_t Operator<B>::planCost() const {
    size_t summed_cost = 0L;
    if(std::is_same_v<bool, B>) return summed_cost; // no gates if in plaintext mode

    if(lhs_child_ != nullptr)
        summed_cost += lhs_child_->planCost();
    if(rhs_child_ != nullptr)
        summed_cost += rhs_child_->planCost();

    summed_cost += OperatorCostModel::operatorCost((SecureOperator *) this);
    return summed_cost;
}

template<typename B>
size_t Operator<B>::planGateCount() const {
	size_t summed_cost = 0L;
	if(std::is_same_v<bool, B>) return summed_cost;

	if(lhs_child_ != nullptr)
		summed_cost += lhs_child_->planGateCount();
	if(rhs_child_ != nullptr)
		summed_cost += rhs_child_->planGateCount();

	summed_cost += this->getGateCount();
	return summed_cost;
}

template<typename B>
double Operator<B>::planRuntime() const {
	double summed_cost = 0;
	
	if(lhs_child_ != nullptr)
		summed_cost += lhs_child_->planRuntime();
	if(rhs_child_ != nullptr)
		summed_cost += rhs_child_->planRuntime();

	summed_cost += this->getRuntimeMs();
	return summed_cost;
}

template class vaultdb::Operator<bool>;
template class vaultdb::Operator<emp::Bit>;




