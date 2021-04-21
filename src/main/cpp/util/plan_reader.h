#ifndef _PLAN_READER_H
#define _PLAN_READER_H

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <emp-tool/emp-tool.h>
#include <operators/operator.h>


// TODO: figure out how to communicate sort order of inputs
// this might be in the comments when we write out the SQL
namespace vaultdb {

    typedef  std::pair<std::string, bool> SqlInputDefinition;
    template<typename B>
    class PlanReader {
    public:
        PlanReader(const std::string &db_name, std::string plan_name);
        PlanReader(const std::string &db_name, std::string plan_name, emp::NetIO * netio, const int & party );

    protected:
        std::string db_name_;
        emp::NetIO *netio_ = nullptr;
        int party_ = emp::PUBLIC;

        std::map<int, shared_ptr<Operator<B> > > operators_; // op ID --> operator instantiation

        void parseSqlInputs(const std::string & input_file);
        void parseSecurePlan(const std::string & plan_file);
        void print(const boost::property_tree::ptree &pt, const std::string &prefix);

        void parseSort(const int &operator_id, const boost::property_tree::ptree &pt);
        void parseAggregate(const int & operator_id, const boost::property_tree::ptree &pt);
        const std::shared_ptr<Operator<B> > getChildOperator(const int & my_operator_id) const;


        // faux template specialization
        shared_ptr<Operator<bool> > createInputOperator(const std::string & sql, const bool & has_dummy_tag);
        shared_ptr<Operator<emp::Bit> > createInputOperator(const std::string & sql, const emp::Bit & has_dummy_tag);

    };
}



#endif // _PLAN_READER_H
