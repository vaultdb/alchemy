#ifndef _JOIN_GRAPH_
#define _JOIN_GRAPH_

#include <algorithm>
#include <operators/operator.h>
#include <expression/expression.h>
#include "expression/visitor/join_equality_condition_visitor.h"
#include "util/field_utilities.h"
#include "operators/join.h"
#include "common/defs.h"
#include "data/psql_data_provider.h"
#include <string>
#include <utility>
#include <vector>

// Generates join graph for a given query
// nodes are leafs, aka inputs to joins
// edges link two leafs and contain join predicates
// only handles trees where joins are all in same query expression
// - i.e., no interspersed aggregation, but projections between joins are ok

namespace vaultdb {

// join graph setup
template<typename B>
class JoinEdge;

template<typename B>
struct JoinInput {
    // in most cases this will be SecureSqlInput, but sometimes it might be a MergeJoin or CTE
    Operator<B> *node_;
        // TODO(future): fill these in later
    //        vector<JoinEdge<B> *> edges_;

    bool operator==(const JoinInput<B> &other) const {
        return *node_ == *other.node_;
    }
//        ~JoinInput() { if(node_ != nullptr) delete node_; }
};

template<typename B>
// inputs are ordered by operator ID for now
struct JoinEdge {
    int lhs_input_ = -1;
    int rhs_input_ = -1;
//        JoinInput<B> *lhs_;
//        JoinInput<B> *rhs_;
    Expression<B> *join_condition_;

    int operator_id_; // temp, for debugging
    int foreign_key_input_ = -1; // -1 - not keyed, 0 == lhs is foreign key, 1 == rhs is foreign key

    bool matchedInputs(const JoinEdge<B> &other) const {
        return (lhs_input_ == other.lhs_input_ && rhs_input_ == other.rhs_input_);
    }

    bool operator==(const JoinEdge<B> &other) const {
        if(*this->lhs_->node_ != *other.lhs_->node_ || *this->rhs_->node_ != *other.rhs_->node_)  return false;
        return *this->join_condition_ == *other.join_condition_;
    }
};

template<typename B>
class JoinGraph {

public:
    map<int, JoinInput<B>> nodes_; // operator ID, JoinInput *
    vector<JoinEdge<B>> edges_;

private:
    vector<ForeignKeyConstraint> foreign_key_constraints_;

public:
    // populate nodes and edges in graph
    JoinGraph(Operator<B> *root, string db_name) {
        if(!hasJoin(root)) return; // no joins in this tree
        foreign_key_constraints_ = PsqlDataProvider::getForeignKeys(db_name);

        cout << "Key constraints: " << endl;
        for(auto f : foreign_key_constraints_) {
            cout << f.toString() << endl;
        }

        joinInputCollector(root);
        joinHelper(root);
        //takeClosure();

    }


    void joinInEdge(JoinEdge<B> & edge) {
        // join its two JoinInputs
        // for each edge that references these JoinInputs, update the join condition

    }
    // two steps:
    // first: need new JoinInput<B> for the join
    // second: rewire the ordinals on join expression for new child
    void replaceEdge(JoinEdge<B> & to_update, bool update_lhs, JoinEdge<B> & prev_edge, Join<B> *child_join) {
        vector<pair<int, int> > new_keys;
        int field_cnt_delta;
        // update lhs child
        if (update_lhs) {
            field_cnt_delta = child_join->getOutputSchema().getFieldCount() - prev_edge.lhs_->node_->getOutputSchema().getFieldCount();
            auto keys = extractJoinKeyOrdinals(to_update.join_condition_, to_update.lhs_->node_->getOutputSchema().getFieldCount());

            if (to_update.lhs_ == prev_edge.lhs_) {
                to_update.lhs_ = &this->nodes_[child_join->getOperatorId()];

                for (auto k: keys) {
                    int l_key = child_join->getDestOrdinal(prev_edge.lhs_->node_, k.first);
                    int r_key = k.second + field_cnt_delta;
                    new_keys.push_back(make_pair(l_key, r_key));
                }
                to_update.join_condition_ = getEqualityPredicateMultiExpression(to_update.lhs_->node_, to_update.rhs_->node_, new_keys);
                to_update.lhs_ = &this->nodes_[child_join->getOperatorId()];
            } else if (to_update.lhs_ == prev_edge.rhs_) {
                to_update.lhs_ = &this->nodes_[child_join->getOperatorId()];

                for (auto k: keys) {
                    int l_key = child_join->getDestOrdinal(prev_edge.rhs_->node_, k.first);
                    int r_key = k.second + field_cnt_delta;
                    new_keys.push_back(make_pair(l_key, r_key));
                }
                to_update.join_condition_ = getEqualityPredicateMultiExpression(to_update.lhs_->node_,  to_update.rhs_->node_, new_keys);
            }

        } else { // update rhs child
            field_cnt_delta = child_join->getOutputSchema().getFieldCount() - prev_edge.rhs_->node_->getOutputSchema().getFieldCount();
            auto keys = extractJoinKeyOrdinals(to_update.join_condition_,  to_update.lhs_->node_->getOutputSchema().getFieldCount());
            if(to_update.rhs_ == prev_edge.lhs_) {
                to_update.rhs_ = &this->nodes_[child_join->getOperatorId()];

                for (auto k: keys) {
                    int l_key = k.first;
                    int r_key = child_join->getDestOrdinal(prev_edge.lhs_->node_, k.second);
                    new_keys.push_back(make_pair(l_key, r_key));
                }
                to_update.join_condition_ = getEqualityPredicateMultiExpression(to_update.lhs_->node_, to_update.rhs_->node_, new_keys);
            } else if(to_update.rhs_ == prev_edge.rhs_) {
                to_update.rhs_ = &this->nodes_[child_join->getOperatorId()];

                for (auto k: keys) {
                    int l_key = k.first;
                    int r_key = child_join->getDestOrdinal(prev_edge.rhs_->node_, k.second);
                    new_keys.push_back(make_pair(l_key, r_key));
                }
                to_update.join_condition_ = getEqualityPredicateMultiExpression(to_update.lhs_->node_, to_update.rhs_->node_, new_keys);
            }

        }
        // TODO(future): might need to update PK-FK status?
    }

    void replaceEdge(JoinEdge<B> & prev_edge, Join<B> *join) {
        auto prev_lhs_input = prev_edge.lhs_;
        auto prev_rhs_input = prev_edge.rhs_;

        join->setOperatorId(prev_edge.operator_id_); // give it a unique operator ID
        JoinInput<B> new_node;
        new_node.node_ = join;
        this->nodes_[join->getOperatorId()] = new_node;

        // delete old edge
        for (int i = 0; i < this->edges_.size(); ++i) {
            if (this->edges_[i] == prev_edge) {
                this->edges_.erase(this->edges_.begin() + i);
                break;
            }
        }

        // for each edge, if it has a leaf that's LHS, then replace with join
        // edges are all unique
        for (auto e: this->edges_) {
            // what about self-joins? This is why we call replaceEdge twice.
            replaceEdge(e, true, prev_edge, join);
            replaceEdge(e, false, prev_edge, join);
        }

        // dedupe edges
        for (auto &e_outer: this->edges_) {
            // can have > 1 dupe if we have self-joins
            for (auto &e_inner: this->edges_) {
                if (e_outer.matchedInputs(e_inner)) {
                    e_outer = mergeEdges(e_outer, e_inner);
                    break;
                }
            }

        }
    }

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
                auto lhs_op = nodes_.at(edge.lhs_input_).node_;
                auto rhs_op = nodes_.at(edge.rhs_input_).node_;
                s << "  * Operator " << edge.operator_id_ << ": " << edge.join_condition_->toString()
                  << " schema: " << QuerySchema::concatenate(lhs_op->getOutputSchema(),
                                                             rhs_op->getOutputSchema()) << ", FK side: " << edge.foreign_key_input_ <<  endl;
                s << "           --> " << lhs_op->toString() << endl;
                s << "           --> " << rhs_op->toString() << endl;

            }
        }
        return s.str();
    }


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
           e.lhs_input_ =  lhs.first->node_->getOperatorId();
           e.rhs_input_ =  rhs.first->node_->getOperatorId();
           e.foreign_key_input_ = getKeyConstraint(lhs.first->node_->getOutputSchema(), lhs.second, rhs.first->node_->getOutputSchema(), rhs.second);
            // set up join predicate
           ExpressionNode<B> *cond = getEqualityPredicate(lhs.first->node_, rhs.first->node_, lhs.second, rhs.second);
//                       e.lhs_->node_, e.rhs_->node_, lhs.second, rhs.second);
           bool found = false;

           // if we have this edge, append to its predicate
           for(int i = 0; i < edges.size(); ++i) {
               if(edges[i].matchedInputs(e)) {
                   // append to predicate
                   assert(edges[i].join_condition_->exprClass() == ExpressionClass::GENERIC);
                   ExpressionNode<B> *existing = ((GenericExpression<B> *) edges[i].join_condition_)->root_;
                   ExpressionNode<B> *conj = new AndNode<B>(existing, cond);

                   edges[i].join_condition_ = new GenericExpression<B>(conj, "predicate", FieldType::BOOL);
                   if(edges[i].foreign_key_input_ == -1 && e.foreign_key_input_ != -1) {
                       edges[i].foreign_key_input_ = e.foreign_key_input_;
                   }

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

        // TODO(future): for each edge, determine PK-FK if applicable here

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
//                    edge.lhs_->edges_.push_back(&edges_.back());
//                    edge.rhs_->edges_.push_back(&edges_.back());
            }
        }

        // recurse to children
        joinHelper(op->getChild(0));
        joinHelper(op->getChild(1));


    }

    JoinEdge<B> mergeEdges(const JoinEdge<B> &lhs_edge, const JoinEdge<B> &rhs_edge) {
        // merge lhs and rhs edges
       // don't attempt to merge incompatible inputs
       assert(*nodes_.at(lhs_edge.lhs_input_).node_ == *nodes_.at(rhs_edge.lhs_input_).node_  && *nodes_.at(lhs_edge.rhs_input_).node_ == *nodes_.at(rhs_edge.rhs_input_).node_);
//            assert(*lhs_edge.lhs_->node_ == *rhs_edge.lhs_->node_ && *lhs_edge.rhs_->node_ == *rhs_edge.rhs_->node_);
        JoinEdge<B> merged;
        merged.lhs_input_ = lhs_edge.lhs_input_;
        merged.rhs_input_ = lhs_edge.rhs_input_;


        // take conjunction of predicates
        int lhs_field_cnt = nodes_.at(lhs_edge.lhs_input_).node_->getOutputSchema().getFieldCount();
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


        if(lhs_edge.foreign_key_input_ == -1 && rhs_edge.foreign_key_input_ != -1) {
            merged.foreign_key_input_ = rhs_edge.foreign_key_input_;
        } else if(lhs_edge.foreign_key_input_ != -1 && rhs_edge.foreign_key_input_ == -1) {
            merged.foreign_key_input_ = lhs_edge.foreign_key_input_;
        } else {
            merged.foreign_key_input_ = rhs_edge.foreign_key_input_;
        }

        merged.join_condition_ = getEqualityPredicateMultiExpression(nodes_.at(merged.lhs_input_).node_, nodes_.at(merged.rhs_input_).node_, lhs_join_keys);
//                    merged.lhs_->node_, merged.rhs_->node_, lhs_join_keys);
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


    // returns -1 if no key constraint exists, 0 if lhs is fkey and 1 if rhs is fkey
    int getKeyConstraint(const QuerySchema & lhs_schema, const int & lhs_ordinal, const QuerySchema & rhs_schem, const int & rhs_ordinal) {
        ColumnReference lhs(lhs_schema.getField(lhs_ordinal).getTableName(), lhs_schema.getField(lhs_ordinal).getName());
        ColumnReference rhs(rhs_schem.getField(rhs_ordinal).getTableName(), rhs_schem.getField(rhs_ordinal).getName());
        ForeignKeyConstraint fk(lhs, rhs);

        if(std::find(foreign_key_constraints_.begin(), foreign_key_constraints_.end(), fk)  != foreign_key_constraints_.end()) {
            return 1;
        }
        fk = ForeignKeyConstraint(rhs, lhs);
        if(std::find(foreign_key_constraints_.begin(), foreign_key_constraints_.end(), fk)  != foreign_key_constraints_.end()) {
            return 0;
        }
        return -1;
    }
};
} // namespace vaultdb

#endif