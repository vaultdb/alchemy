
#ifndef __LRU_K_REPLACER_CPP__
#define __LRU_K_REPLACER_CPP__
#include <vector>
#include <list>
// #include <unordered_map>

#include "common/defs.h"


namespace vaultdb {
    class BufferPoolManager;
    class SystemConfiguration;

    class LRUKNode {
    public:
        std::list<size_t> history_;
        // bool is_evictable_ = true; // default to eviction candidate, not designed for OLTP.
        LRUKNode() = default;

        size_t backwardsKDistance(size_t & current_timestamp, size_t & k) const {
            if (history_.size() < k) {
                return std::numeric_limits<size_t>::max();
            }
            return current_timestamp - history_.front();
        }

        bool hasInfBackwardKDistance(size_t & k) const {
            return history_.size() < k;
        }




    };

    // modeled loosely on BusTub: https://github.com/cmu-db/bustub
    class LRUKReplacer {
        public:
            LRUKReplacer(long frame_cnt, long k_dist = 3);
                // :  k_(k_dist), frame_cnt_(frame_cnt), bpm_(SystemConfiguration::getInstance().bpm_) {}

          frame_id_t evict() {
                if (nodes_.empty()) {
                    throw std::runtime_error("LRUKReplacer: empty page list. Cannot evict.");
                }

                size_t max_backward_k_dist = 0;
                size_t min_timestamp = std::numeric_limits<size_t>::max();

                frame_id_t eviction_candidate = -1;
                frame_id_t inf_candidate = -1;
                int inf_cnt = 0;


                 for (auto &[frame_id, node] : nodes_) {
                     if (!isEvictable(frame_id)) {
                         continue; // skip non-evictable nodes
                     }

                    // cout << "Frame " << frame_id << " history len: " << node.history_.size() <<  " min timestamp: " << node.history_.front() << " backwards k distance: " <<  node.backwardsKDistance(current_timestamp_, k_) << '\n';
                    if (node.hasInfBackwardKDistance(k_)) {
                             if (node.history_.front() < min_timestamp) {
                             // cout << "    recording inf candidate " << frame_id << " with timestamp " << node.history_.front() << '\n';
                             // if we have not found an inf candidate yet, we need to initialize it
                             inf_candidate = frame_id;
                             min_timestamp = node.history_.front();
                         }
                        ++inf_cnt;
                         continue;
                     }

                     auto curr_backward_k_dist = node.backwardsKDistance(current_timestamp_, k_);

                         // Otherwise, find the frame whose backward k-distance is maximum of all frames in the replacer.
                     if (curr_backward_k_dist > max_backward_k_dist) {
                             // cout << "      recording new max backwards k distance " << curr_backward_k_dist << " for frame " << frame_id << '\n';
                             max_backward_k_dist = curr_backward_k_dist;
                             eviction_candidate = frame_id;

                     }
                } // end loop over candidate nodes
                // cout << "Ending with eviction candidate " << eviction_candidate << " with max backward k distance " << max_backward_k_dist << " and inf candidate " << inf_candidate << " with inf count " << inf_cnt << '\n';
                // need 2+ inf nodes to evict an inf candidate
                if (inf_cnt > 1) {
                    eviction_candidate = inf_candidate;
                }

                // cout << "Selected " << eviction_candidate << " for eviction.\n";

                if (eviction_candidate == -1) {
                    return -1; // no eviction candidate found
                }

                remove(eviction_candidate);
                return eviction_candidate;
            }


          void recordAccess(frame_id_t frame_id) {
            assert(frame_id >= 0 && frame_id < frame_cnt_);

           if (nodes_.find(frame_id) == nodes_.end()) {
                nodes_[frame_id] = LRUKNode();
               ++current_timestamp_;
            }



              LRUKNode &node = nodes_[frame_id];
              if (node.history_.size() == k_) {
                  node.history_.pop_front();
              }

              node.history_.push_back(current_timestamp_++);
          }

        bool isEvictable(frame_id_t frame_id);
        // {
        //     if(nodes_.find(frame_id) != nodes_.end()) {
        //         PageId pid = bpm_.reverse_position_map_.at(frame_id);
        //         return !bpm_.position_map_.at(pid).pinned_;
        //     }
        // }

        // void setEvictable(frame_id_t frame_id, bool set_evictable) {
        //       assert(nodes_.find(frame_id) != nodes_.end());
        //       LRUKNode &node = nodes_[frame_id];
        //
        //     //   if (node.is_evictable_ == !set_evictable) {
        //     //       --current_size_;
        //     //   }
        //     // if (!node.is_evictable_ && set_evictable) {
        //     //     ++current_size_;
        //     // }
        //
        //       node.is_evictable_ = set_evictable;
        //   }

        void remove(frame_id_t frame_id)
        {
            if (isEvictable(frame_id)) {
                nodes_.erase(frame_id);
            }
        }





    private:
        size_t current_timestamp_ = 0;
        size_t k_;
        size_t frame_cnt_;
        std::unordered_map<frame_id_t, LRUKNode > nodes_; // frame_id, history of timestamps
        BufferPoolManager &bpm_;
    };

}

#endif
