#ifndef _JOIN_GRAPH_
#define _JOIN_GRAPH_

#include <operators/operator.h>
#include <expression/expression.h>
#include "expression/visitor/join_equality_condition_visitor.h"
#include "util/field_utilities.h"
#include "operators/join.h"

// Generates join graph for a given query
// nodes are leafs, aka inputs to joins
// edges link two leafs and contain join predicates
// only handles trees where joins are all in same query expression
// - i.e., no interspersed aggregation, but projections between joins are ok

namespace vaultdb {
//    typedef pair<int, int> EdgeReference; // edge operator ID, ordinal
//    typedef pair<int, int> NodeReference; // node operator ID, ordinal

    // join graph setup
    template<typename B>
    class JoinEdge;

    template<typename B>
    struct JoinInput {
        // in most cases this will be SecureSqlInput, but sometimes it might be a MergeJoin or CTE
        Operator<B> *node_;

        vector<JoinEdge<B> *> edges_;
        bool operator==(const JoinInput<B> &other) const {
            return *node_ == *other.node_;
        }
//        ~JoinInput() { if(node_ != nullptr) delete node_; }
    };

    template<typename B>
    // inputs are ordered by operator ID for now
    struct JoinEdge {
        JoinInput<B> *lhs_;
        JoinInput<B> *rhs_;
        Expression<B> *join_condition_;

        int operator_id_; // temp, for debugging

        bool matchedInputs(const JoinEdge<B> &other) const {
            return (*lhs_->node_ == *other.lhs_->node_ && *rhs_->node_ == *other.rhs_->node_);
        }

        bool operator==(const JoinEdge<B> &other) const {
            if(*this->lhs_->node_ != *other.lhs_->node_ || *this->rhs_->node_ != *other.rhs_->node_)  return false;
            return *this->join_condition_ == *other.join_condition_;
        }
    };

    template<typename B>
    class JoinGraph {

    public:
        // populate nodes and edges in graph
        JoinGraph(Operator<B> *root) {
            if(!hasJoin(root)) return; // no joins in this tree
            joinInputCollector(root);

            joinHelper(root);
            //takeClosure();

        }



/*
        void replaceEdge(JoinEdge<B> & edge, Join<B> *join) {
            auto lhs = edge.lhs_;
            auto rhs = edge.rhs_;

            for(auto e : edges_) {
                if(edge != e) {
                    // replace lhs on this edge with new join
                    // JMR return here
                    if(e.lhs_ == lhs) {
                        auto keys = extractJoinKeyOrdinals(e.join_condition_, e.lhs_->node_->getOutputSchema().getFieldCount());

                        e.lhs_->node_ = join;
                    }
                    if(e.lhs_ == lhs || e.lhs_ == rhs) {
                        e.lhs_->node_ = join;
                        auto keys = extractJoinKeyOrdinals(e.join_condition_, e.lhs_->node_->getOutputSchema().getFieldCount());
                        // TODO: map from lhs keys to new join
                        // know this will be one of the children of the new join because otherwise it wouldn't be a node
                        //  figure out which join input it is and map backwards from this
                    }
                    // TODO: update ordinals in edge's join condition
                    if(e.rhs_ == lhs || e.rhs_ == rhs) e.rhs_->node_ = join;
                }
            }
            // TODO: dedupe edges
        }
*/
        string toString() const {
            if(edges_.empty() && nodes_.empty()) return "";
            stringstream s;

            if(!nodes_.empty()) {
                    s << "Nodes: \n";
                for(auto node : nodes_) {
                    s << "     * " << node.second.node_->toString() << endl;
                }
            }

            if(!edges_.empty()) {
                s << " Edges: " << endl;
                for (auto edge: edges_) {
                    s << "  * Operator " << edge.operator_id_ << ": " << edge.join_condition_->toString()
                      << " schema: " << QuerySchema::concatenate(edge.lhs_->node_->getOutputSchema(),
                                                                 edge.rhs_->node_->getOutputSchema()) << endl;
                    s << "           --> " << edge.lhs_->node_->toString() << endl;
                    s << "           --> " << edge.rhs_->node_->toString() << endl;

                }
            }
            return s.str();
        }

        map<int, JoinInput<B>> nodes_; // operator ID, JoinInput *
        vector<JoinEdge<B>> edges_;

    private:
        // fill in implied edges in join graph
        void takeClosure() {
            // if two edges refer to the same ordinal in the same operator, they are already connected
            for(auto in : nodes_) {
                map<int, vector<JoinEdge<B> *>> edges_by_ordinal;
                for(auto edge : in.second.edges_) {

                }
            }


        }

        void joinInputCollector(Operator<B> *op) {
            // if it's not at a leaf in the join tree yet, recurse!
            if(hasJoin(op)) {
                if(op->getChild(0) != nullptr) joinInputCollector(op->getChild(0));
                if(op->getChild(1) != nullptr) joinInputCollector(op->getChild(1));
                return;
            }
            // else register leaf
            // if it already exists, skip this step
            if(nodes_.find(op->getOperatorId()) != nodes_.end()) return;

            JoinInput<B> node;
            node.node_ = op;
            nodes_[op->getOperatorId()] = node;
        }

        // check if there are any joins in this op or its children
        bool hasJoin(const Operator<B> *op) {
            // only get here if we have not encountered a join yet
            if(op->isLeaf())
                return false;

            if(isJoin(op->getType()))
                return true;

            if(op->getChild(0) != nullptr && hasJoin(op->getChild(0)))
                return true;

            if(op->getChild(1) != nullptr && hasJoin(op->getChild(1)))
                return true;

            return false;
        }


        // returns the join graph entry for this and the ordinal
        // for use when it has a join parent that needs to map back to child through another join
        // gives the ordinal for the target field in the current join
        // run after joinInputCollector to populate all join graph nodes first (i.e., join inputs)
        std::pair<JoinInput<B> *, int> findInputNode(Operator<B> *op, const int & ordinal) {
            // keep recursing until we find correct JoinInput
            auto src_reln = op->getSourceOperator(ordinal);
            int src_idx = op->getSourceOrdinal(ordinal);

            // while not a leaf
            while(nodes_.find(src_reln->getOperatorId()) == nodes_.end()) {
               int last = src_idx;
               src_idx = src_reln->getSourceOrdinal(src_idx);
               src_reln = src_reln->getSourceOperator(last);

            }

            JoinInput<B> *node = &nodes_[src_reln->getOperatorId()];
            return std::make_pair(node, src_idx);
        }

        vector<pair<int, int> > extractJoinKeyOrdinals(Join<B> *join) {
                return extractJoinKeyOrdinals(join->getPredicate(), join->getChild(0)->getOutputSchema().getFieldCount());
        }
            
            vector<pair<int, int> > extractJoinKeyOrdinals(Expression<B> *p, int lhs_field_count) {
                assert(p->exprClass() == ExpressionClass::GENERIC);
                JoinEqualityConditionVisitor<B> visitor(((GenericExpression<B> *) p)->root_);
            vector<pair<uint32_t, uint32_t> > join_equalities = visitor.getEqualities();
            vector<pair<int, int> > res;

            // create list of rhs and lhs ordinalis
            for (auto entry: join_equalities) {
                int lt = entry.first;
                int gt = entry.second;
                // verify that we have one reference to lhs and one to rhs
                assert(lt != gt);
                assert(lt < lhs_field_count &&
                       gt >= lhs_field_count);
                res.push_back(make_pair(lt, gt));
            }

            return res;
        }



        void joinHelper(Operator<B> *op) {

            if(!hasJoin(op)) return;

            // traverse tree, if we encounter a join, then construct an edge for each join condition
            if(!isJoin(op->getType())) {
                // recurse to next join
                if(op->getChild(0) != nullptr) joinHelper(op->getChild(0));
                if(op->getChild(1) != nullptr) joinHelper(op->getChild(1));
                return;
            }

            Join<B> *join = (Join<B> *) op;

            auto keys = extractJoinKeyOrdinals(join);
            vector<JoinEdge<B>> edges;

            // pair of JoinInputs matched to keys
            for(auto k : keys) {

               pair<JoinInput<B> *, int> lhs = findInputNode(join, k.first);
               pair<JoinInput<B> *, int> rhs = findInputNode(join, k.second);

               JoinEdge<B> e;
               e.lhs_ =  lhs.first;
               e.rhs_ =  rhs.first;
                // set up join predicate
               ExpressionNode<B> *cond = getEqualityPredicate(e.lhs_->node_, e.rhs_->node_, lhs.second, rhs.second);
               bool found = false;
               
               // if we have this edge, append to its predicate
               for(int i = 0; i < edges.size(); ++i) {
                   if(edges[i].matchedInputs(e)) {
                       // append to predicate
                       assert(edges[i].join_condition_->exprClass() == ExpressionClass::GENERIC);
                       ExpressionNode<B> *existing = ((GenericExpression<B> *) edges[i].join_condition_)->root_;
                       ExpressionNode<B> *conj = new AndNode<B>(existing, cond);

                       edges[i].join_condition_ = new GenericExpression<B>(conj, "predicate", FieldType::BOOL);
                       delete cond;
                       delete conj;
                       found = true;
                       break;
                   }
               }

               // otherwise prepare to register new edge
               if(!found) {
                   e.join_condition_ = new GenericExpression<B>(cond, "predicate", FieldType::BOOL);;
                   delete cond;
                   e.operator_id_ = op->getOperatorId();

                   edges.push_back(e);
               }
            } // end for-keys

            
            // merge keys with existing edges
            for(auto edge : edges) {
                bool found = false;
                for(auto & existing_edge : edges_) {
                    if(existing_edge.matchedInputs(edge)) {
                        existing_edge = mergeEdges(existing_edge, edge);
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    edges_.push_back(edge);
                    edge.lhs_->edges_.push_back(&edges_.back());
                    edge.rhs_->edges_.push_back(&edges_.back());
                }
            }

            // recurse to children
            joinHelper(op->getChild(0));
            joinHelper(op->getChild(1));


        }

        JoinEdge<B> mergeEdges(const JoinEdge<B> &lhs_edge, const JoinEdge<B> &rhs_edge) {
            // merge lhs and rhs edges
           // don't attempt to merge incompatible inputs
            assert(*lhs_edge.lhs_->node_ == *rhs_edge.lhs_->node_ && *lhs_edge.rhs_->node_ == *rhs_edge.rhs_->node_); 
            JoinEdge<B> merged;
            merged.lhs_ = lhs_edge.lhs_;
            merged.rhs_ = lhs_edge.rhs_;
            

            // take conjunction of predicates
            int lhs_field_cnt = lhs_edge.lhs_->node_->getOutputSchema().getFieldCount();
            vector<pair<int, int> > lhs_join_keys = extractJoinKeyOrdinals(lhs_edge.join_condition_, lhs_field_cnt);
            vector<pair<int, int> > rhs_join_keys = extractJoinKeyOrdinals(rhs_edge.join_condition_, lhs_field_cnt);
            map<pair<int, int>, bool> seen;
            for(auto l : lhs_join_keys) {
                seen[l] = true;
            }
            for(auto r : rhs_join_keys) {
                if(seen.find(r) == seen.end()) {
                    lhs_join_keys.push_back(r);
                }
            }

            merged.join_condition_ = getEqualityPredicateMultiExpression(merged.lhs_->node_, merged.rhs_->node_, lhs_join_keys);
            return merged;

        }
        
        ExpressionNode<B> *getEqualityPredicate(Operator<B> *lhs, Operator<B> *rhs, int lhs_ordinal, int rhs_ordinal) {
            auto lhs_schema = lhs->getOutputSchema();
            auto rhs_schema = rhs->getOutputSchema();

            return (ExpressionNode<B> *) new EqualNode<B>(new InputReference<B>(lhs_ordinal, lhs_schema, rhs_schema),
                                                      new InputReference<B>(rhs_ordinal + lhs_schema.getFieldCount(), lhs_schema, rhs_schema));

        }

        // caution: this breaks coding convention by passing in ordinals with lhs and rhs inputs both zero-indexed
        static GenericExpression<B> *getEqualityPredicateMultiExpression(Operator<B> *lhs, Operator<B> *rhs, vector<pair<int, int> > & join_ordinals) {

            auto lhs_schema = lhs->getOutputSchema();
            auto rhs_schema = rhs->getOutputSchema();

            auto lhs_input = new InputReference<B>(join_ordinals[0].first, lhs_schema);
            auto rhs_input = new InputReference<B>(join_ordinals[0].second, rhs_schema);
            ExpressionNode<B> *res = new EqualNode<B>(lhs_input, rhs_input);

            for(int i = 1; i < join_ordinals.size(); ++i) {
                ExpressionNode<B> *next = new EqualNode<B>(new InputReference<B>(join_ordinals[i].first, lhs_schema),
                                                           new InputReference<B>(join_ordinals[i].second, rhs_schema));
                res = new AndNode<B>(res, next);
            }


            GenericExpression<B> *g =  new GenericExpression<B>(res, "predicate",
                                                                std::is_same_v<B, bool> ? FieldType::BOOL : FieldType::SECURE_BOOL);

            delete res;
            return g;

        }

        // helper function for ID'ing joins
        static bool isJoin(OperatorType t) {
            // only consider joins that are equi-joins - i.e., ones we are optimizing
            if(  t == OperatorType::NESTED_LOOP_JOIN ||      t ==  OperatorType::KEYED_NESTED_LOOP_JOIN || t ==  OperatorType::BLOCK_NESTED_LOOP_JOIN || 	t == OperatorType::SORT_MERGE_JOIN || t ==  OperatorType::KEYED_SORT_MERGE_JOIN ) {
                return true;
            }
            return false;
        }

    };
}

#endif