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
    struct JoinPairInfo {
        std::pair<Operator<B> *, string> lhs;
        std::pair<Operator<B> *, string> rhs;
        char joinType;
        size_t outputCardinality;

        JoinPairInfo(std::pair<Operator<B> *, string> lhs, std::pair<Operator<B> *, string> rhs, char joinType, int outputCardinality)
                : lhs(lhs), rhs(rhs), joinType(joinType), outputCardinality(outputCardinality) {}
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
        std::vector<std::pair<Operator<B>*, std::vector<JoinPairInfo<B>>>> getConnectedOperators(
                const std::vector<JoinPairInfo<B>>& currentjoinPairs, const std::vector<JoinPairInfo<B>>& alljoinPairs) const {
            // Map to accumulate connections and their corresponding linkages.
            std::map<Operator<B>*, std::vector<JoinPairInfo<B>>> connectedOperatorsMap;

            // Set to track operators directly involved in the initial join pairs.
            std::set<Operator<B>*> directlyInvolvedOperators;
            for (const auto& pair : currentjoinPairs) {
                directlyInvolvedOperators.insert(pair.lhs.first);
                directlyInvolvedOperators.insert(pair.rhs.first);
            }

            // Process each edge to find connections excluding direct ones.
            for (const auto& edge : edges) {
                Operator<B>* fromOp = edge.from->op;
                Operator<B>* toOp = edge.to->op;

                // Check if this edge represents a direct connection already accounted for.
                if (directlyInvolvedOperators.find(fromOp) != directlyInvolvedOperators.end() &&
                    directlyInvolvedOperators.find(toOp) != directlyInvolvedOperators.end()) {
                    continue; // Skip as it's the same edge.
                }

                // For edges not representing the same edge, accumulate linkages.

                // If fromOp is directly involved, add the connection to the map.
                if (directlyInvolvedOperators.find(fromOp) != directlyInvolvedOperators.end()) {
                    // Find the join pair from alljoinPairs that includes fromOp and toOp.
                    auto joinPairIt = std::find_if(alljoinPairs.begin(), alljoinPairs.end(),
                            [fromOp, toOp](const JoinPairInfo<B>& joinPair) {
                                return (joinPair.lhs.first == fromOp && joinPair.rhs.first == toOp) ||
                                       (joinPair.lhs.first == toOp && joinPair.rhs.first == fromOp);
                            });
                    connectedOperatorsMap[toOp].push_back({*joinPairIt});
                }
                // If toOp is directly involved, add the connection to the map.
                else if (directlyInvolvedOperators.find(toOp) != directlyInvolvedOperators.end()) {
                    // Find the join pair from alljoinPairs that includes fromOp and toOp.
                    auto joinPairIt = std::find_if(alljoinPairs.begin(), alljoinPairs.end(),
                                                   [fromOp, toOp](const JoinPairInfo<B>& joinPair) {
                                                       return (joinPair.lhs.first == fromOp && joinPair.rhs.first == toOp) ||
                                                              (joinPair.lhs.first == toOp && joinPair.rhs.first == fromOp);
                                                   });
                    connectedOperatorsMap[fromOp].push_back({*joinPairIt});
                }
            }

            // Convert the map to the expected vector format.
            std::vector<std::pair<Operator<B>*, std::vector<JoinPairInfo<B>>>> connectedOperatorsWithLinkages;
            for (const auto& entry : connectedOperatorsMap) {
                connectedOperatorsWithLinkages.push_back({entry.first, entry.second});
            }

            return connectedOperatorsWithLinkages;
        }




        const std::vector<Node> &getNodes() const { return nodes; }
        const std::vector<Edge> &getEdges() const { return edges; }

    };
}
#endif