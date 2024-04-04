#ifndef _JOIN_GRAPH_H
#define _JOIN_GRAPH_H

#include <string>
#include <tuple>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <emp-tool/emp-tool.h>
#include <operators/operator.h>
#include "util/system_configuration.h"

namespace vaultdb {
    template<typename B>
    struct inputRelation {
        Operator<B> * table;
        size_t cost;

        inputRelation(Operator<B> * table, size_t cost) : table(table), cost(cost) {}
    };

    template<typename B>
    struct subPlan {
        string lhs;
        string rhs;
        OperatorType type;
        size_t cost;
        SortDefinition output_order;

        subPlan(string lhs, string rhs, OperatorType type, size_t cost, SortDefinition output_order)
        : lhs(lhs), rhs(rhs), type(type), cost(cost), output_order(output_order) {}

    };

    template<typename B>
    struct JoinPair {
        Operator<B> *lhs;
        Operator<B> *rhs;
        string lhs_predicate;
        string rhs_predicate;

        JoinPair(Operator<B> *lhs, Operator<B> *rhs, string lhs_predicate, string rhs_predicate)
                : lhs(lhs), rhs(rhs), lhs_predicate(lhs_predicate), rhs_predicate(rhs_predicate) {}

        bool operator==(const JoinPair& other) const {
            // Assuming equality is determined by the operators
            return (*this->lhs == *other.lhs && *this->rhs == *other.rhs) ||
                    (*this->lhs == *other.rhs && *this->rhs == *other.lhs);
        }
    };

    template<typename B>
    struct JoinPairInfo {
        std::pair<Operator<B> *, string> lhs;
        std::pair<Operator<B> *, string> rhs;
        char joinType;
        size_t outputCardinality;

        JoinPairInfo(std::pair<Operator<B> *, string> lhs, std::pair<Operator<B> *, string> rhs, char joinType, size_t outputCardinality)
                : lhs(lhs), rhs(rhs), joinType(joinType), outputCardinality(outputCardinality) {}

        bool operator==(const JoinPairInfo& other) const {
            // Assuming equality is determined by the operators and the join type
            // You might need to adjust the logic based on what you consider makes two JoinPairInfo instances equal
            return *this->lhs.first == *other.lhs.first && *this->rhs.first == *other.rhs.first &&
                   this->lhs.second == other.lhs.second && this->rhs.second == other.rhs.second &&
                   this->joinType == other.joinType;
        }
    };

    template<typename B>
    class JoinGraph {
    public:
        struct Node {
            Operator<B> *op; // Node represented by Operator pointer
        };

        struct Edge {
            Node *from;
            Node *to;
            char joinType;

            Edge(Node *from, Node *to, char joinType)
                    : from(from), to(to), joinType(joinType) {}
        };

    private:
        std::vector<Node> nodes; // List of nodes
        std::vector<Edge> edges; // List of edges
        std::map<Operator<B>*, size_t> operatorToIndex; // Map to track existing nodes

    public:
        // Add a node if it doesn't already exist
        Node* addNode(Operator<B>* op) {
            auto it = operatorToIndex.find(op);
            if (it != operatorToIndex.end()) {
                // Node already exists, return it by index
                return &nodes[it->second];
            } else {
                // Node doesn't exist, create a new one and store its index
                nodes.emplace_back(Node{op});
                size_t newIndex = nodes.size() - 1;
                operatorToIndex[op] = newIndex;
                return &nodes[newIndex];
            }
        }

        // Add an edge between two operators
        void addEdge(Operator<B>* from, Operator<B>* to, char joinType) {
            // Check if the nodes exist in the map, and retrieve their indices
            auto fromIt = operatorToIndex.find(from);
            auto toIt = operatorToIndex.find(to);

            if (fromIt == operatorToIndex.end() || toIt == operatorToIndex.end()) {
                // One of the operators does not have a corresponding node in the graph,
                // which should likely be handled as an error or by adding the node.
                throw std::runtime_error("Attempting to add an edge for an operator without a node.");
            }

            // Retrieve nodes for 'from' and 'to' operators using the indices
            Node* fromNode = &nodes[fromIt->second];
            Node* toNode = &nodes[toIt->second];

            // Add the edge
            edges.emplace_back(Edge{fromNode, toNode, joinType});
        }

        // Method to get operators connected to a given join pair
        std::vector<Operator<B>*> getConnectedOperators(const std::vector<JoinPairInfo<B>>& currentJoinPairs) const {
            std::set<Operator<B>*> directlyInvolvedOperators;
            for (const auto& pair : currentJoinPairs) {
                directlyInvolvedOperators.insert(pair.lhs.first);
                directlyInvolvedOperators.insert(pair.rhs.first);
            }

            std::set<Operator<B>*> connectedOperators;
            // Iterate through each edge to check for connections.
            for (const auto& edge : edges) {
                Operator<B>* fromOp = edge.from->op;
                Operator<B>* toOp = edge.to->op;

                // If fromOp is directly involved and toOp is not already considered, mark toOp as connected.
                if (directlyInvolvedOperators.find(fromOp) != directlyInvolvedOperators.end() &&
                    directlyInvolvedOperators.find(toOp) == directlyInvolvedOperators.end()) {
                    connectedOperators.insert(toOp);
                }
                    // Similarly, if toOp is directly involved and fromOp is not, mark fromOp as connected.
                else if (directlyInvolvedOperators.find(toOp) != directlyInvolvedOperators.end() &&
                         directlyInvolvedOperators.find(fromOp) == directlyInvolvedOperators.end()) {
                    connectedOperators.insert(fromOp);
                }
            }

            // Convert the set of connected operators to the expected vector format.
            return std::vector<Operator<B>*>(connectedOperators.begin(), connectedOperators.end());
        }


        const std::vector<Node> &getNodes() const { return nodes; }
        const std::vector<Edge> &getEdges() const { return edges; }

    };
}
#endif