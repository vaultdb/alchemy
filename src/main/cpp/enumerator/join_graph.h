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
    class JoinGraph {
    public:
        struct Node {
            Operator<B> *op; // Node represented by Operator pointer
        };

        struct Edge {
            size_t fromIndex;
            size_t toIndex;
            string from_predicate;
            string to_predicate;

            Edge(size_t fromIndex, size_t toIndex, string from_predicate, string to_predicate)
                    : fromIndex(fromIndex), toIndex(toIndex), from_predicate(from_predicate), to_predicate(to_predicate) {}
        };

    private:
        std::vector<Node> nodes; // List of nodes
        std::vector<Edge> edges; // List of edges
        std::map<Operator<B>*, size_t> operatorToIndex; // Map to track existing nodes

        // Helper method for findHamiltonianPaths
        void findPaths(size_t nodeIndex, std::vector<bool>& visited, std::vector<std::string>& paths, std::string currentPath) {
            visited[nodeIndex] = true;

            if (!currentPath.empty()) {
                currentPath += "-";  // Only add a hyphen if not the start of the path
            }
            currentPath += std::to_string(nodes[nodeIndex].op->getOperatorId());

            bool isCompletePath = std::all_of(visited.begin(), visited.end(), [](bool v) { return v; });

            if (isCompletePath) {
                paths.push_back(currentPath);
            } else {
                for (const Edge& edge : edges) {
                    if (edge.fromIndex == nodeIndex && !visited[edge.toIndex]) {
                        // When moving from 'fromIndex' to 'toIndex'
                        std::string extendedPath = currentPath + " {" + edge.from_predicate + "," + edge.to_predicate + "} ";
                        findPaths(edge.toIndex, visited, paths, extendedPath);
                    } else if (edge.toIndex == nodeIndex && !visited[edge.fromIndex]) {
                        // When moving from 'toIndex' to 'fromIndex' (reverse direction)
                        std::string extendedPath = currentPath + " {" + edge.to_predicate + "," + edge.from_predicate + "} ";
                        findPaths(edge.fromIndex, visited, paths, extendedPath);
                    }
                }
            }

            visited[nodeIndex] = false;
        }


        Operator<B>* findOperatorByIndex(const size_t operatorIndex) {
            auto it = std::find_if(operatorToIndex.begin(), operatorToIndex.end(),
                                   [operatorIndex](const auto& pair) { return pair.second == operatorIndex; });
            if (it != operatorToIndex.end()) {
                return it->first;
            }
            return nullptr; // or throw an exception if appropriate
        }

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
        void addEdge(Operator<B>* from, Operator<B>* to) {
            // Check if the nodes exist in the map, and retrieve their indices
            auto fromIt = operatorToIndex.find(from);
            auto toIt = operatorToIndex.find(to);

            if (fromIt == operatorToIndex.end() || toIt == operatorToIndex.end()) {
                // One of the operators does not have a corresponding node in the graph,
                // which should likely be handled as an error or by adding the node.
                throw std::runtime_error("Attempting to add an edge for an operator without a node.");
            }

            // Add the edge using indices
            edges.emplace_back(Edge{fromIt->second, toIt->second});
        }

        // Add an edge between two operators with predicate
        void addEdge(Operator<B>* from, Operator<B>* to, string from_predicate, string to_predicate) {
            // Check if the nodes exist in the map, and retrieve their indices
            auto fromIt = operatorToIndex.find(from);
            auto toIt = operatorToIndex.find(to);

            if (fromIt == operatorToIndex.end() || toIt == operatorToIndex.end()) {
                // One of the operators does not have a corresponding node in the graph,
                // which should likely be handled as an error or by adding the node.
                throw std::runtime_error("Attempting to add an edge for an operator without a node.");
            }

            // Add the edge using indices
            edges.emplace_back(Edge{fromIt->second, toIt->second, from_predicate, to_predicate});
        }

        // Method to find Hamiltonian Paths
        std::vector<std::string> findHamiltonianPaths() {
            std::vector<std::string> paths;
            std::vector<bool> visited(nodes.size(), false);

            for (size_t i = 0; i < nodes.size(); ++i) {
                findPaths(i, visited, paths, "");
            }

            return paths;
        }



        const std::vector<Node> &getNodes() const { return nodes; }
        const std::vector<Edge> &getEdges() const { return edges; }

    };
}
#endif