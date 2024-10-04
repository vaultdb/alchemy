#include <util/data_utilities.h>
#include "operators/operator.h"
#include "util/logger.h"
#include "operators/sort.h"
#include "opt/operator_cost_model.h"
#include <ctime>
#include <cmath>
#include <set>

using namespace vaultdb;
using namespace Logging;

template<typename B>
Operator<B>::Operator(QueryTable<B> *lhs, const SortDefinition & sorted_on)
: sort_definition_(sorted_on), system_conf_(SystemConfiguration::getInstance()) {
    lhs_child_ = new TableInput<B>(lhs);
    output_schema_ = lhs->getSchema(); // copy up child schema by default

    if(sorted_on.empty())
        sort_definition_ = lhs->order_by_;
}

template<typename B>
Operator<B>::Operator(QueryTable<B> *lhs, QueryTable<B> *rhs, const SortDefinition & sorted_on)
    : lhs_child_(new TableInput(lhs)), rhs_child_(new TableInput(rhs)), sort_definition_(sorted_on),  system_conf_(SystemConfiguration::getInstance())  {
}

template<typename B>
Operator<B>::Operator(Operator *child, const SortDefinition & sorted_on)
: lhs_child_(child), sort_definition_(sorted_on), system_conf_(SystemConfiguration::getInstance())   {

     child->setParent(this);

    output_schema_ = child->output_schema_;
    output_cardinality_ = child->output_cardinality_; // default to passing on child cardinality
    if(sorted_on.empty())
        sort_definition_ = child->sort_definition_;

}

template<typename B>
Operator<B>::Operator(Operator *lhs, Operator *rhs, const SortDefinition & sorted_on)
     :  lhs_child_(lhs), rhs_child_(rhs), sort_definition_(sorted_on),  system_conf_(SystemConfiguration::getInstance())  {

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

    if(std::is_same_v<B, Bit> && this->getOperatorId() > -2) {
		Logger* log = get_log();

        log->write("Operator #" + std::to_string(this->getOperatorId()) + " " + getTypeString() +
				" ran for " + std::to_string(runtime_ms_) + " ms, " + 
				"gate count: " + std::to_string(gate_cnt_) + 
				" output cardinality: " + std::to_string(output_->tuple_cnt_) +
				", row width=" + std::to_string(output_schema_.size()), Level::INFO);

        if (gate_cnt_ > 0  && this->getOperatorId() >= -1) {
            size_t estimated_gates = OperatorCostModel::operatorCost(reinterpret_cast<const SecureOperator *>(this));
            float relative_error = std::fabs(static_cast<float>( estimated_gates) - static_cast<float>(gate_cnt_)) / static_cast<float>( gate_cnt_) * 100.0;
            log->write("Estimated cost for " + this->toString() + " : " + std::to_string(estimated_gates) +
                    ", Observed gates: " + std::to_string(gate_cnt_) +
                    ", Error rate(%) : " + std::to_string(relative_error), Level::DEBUG);
//            if(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE) {
//                log->write("Buffer pool statistics: " + SystemConfiguration::getInstance().bpm_.stats(), Level::DEBUG);
//            }
        }
    }

    operator_executed_ = true;
    sort_definition_ = output_->order_by_; // update this if needed

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
        size_t estimated_gates = OperatorCostModel::operatorCost(reinterpret_cast<const SecureOperator *>(this));
        ss << ", cost est: " << estimated_gates;
        ss << ", output card: " << output_cardinality_;
    }
    return ss.str();

}


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

    summed_cost += OperatorCostModel::operatorCost(reinterpret_cast<const SecureOperator *>(this));
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

template<typename B>
std::string Operator<B>::getOutputOrderinString(bool order_by_first_collation_) const{
    QuerySchema output_schema = getOutputSchema();
    SortDefinition output_order = getSortOrder();
    std::string output_order_str = "";
    std::set<std::string> included_suffixes;

    if (order_by_first_collation_ && !output_order.empty()) {
        const auto& first_sort = output_order[1];
        if (first_sort.first != -1) {
            std::string first_column_name = output_schema.fields_[first_sort.first].getName();
            auto pos = first_column_name.find_last_of('_');
            std::string first_suffix = (pos != std::string::npos) ? first_column_name.substr(pos + 1) : first_column_name;

            output_order_str += first_suffix;
            if (first_sort.second == SortDirection::DESCENDING)
                output_order_str += " DESC";
        }
        return output_order_str;
    }

    if (output_order.empty())
        output_order_str = "unordered";
    else {
        for (const auto& sort : output_order) {
            if (sort.first == -1)
                continue;

            std::string column_name = output_schema.fields_[sort.first].getName();
            // Split the column_name by '_' and take the part after it
            auto pos = column_name.find_last_of('_');
            std::string suffix = (pos != std::string::npos) ? column_name.substr(pos + 1) : column_name;

            // Check if this suffix is already included
            if (included_suffixes.find(suffix) == included_suffixes.end()) {
                // Not included yet, so let's add it
                included_suffixes.insert(suffix);
                if (sort.second == SortDirection::DESCENDING)
                    output_order_str += suffix + " DESC, ";
                else
                    output_order_str += suffix + ", ";
            }
        }
        // Remove trailing comma and space if there is one
        if (!output_order_str.empty())
            output_order_str.erase(output_order_str.length() - 2);
    }
    return output_order_str;
}

template class vaultdb::Operator<bool>;
template class vaultdb::Operator<emp::Bit>;




