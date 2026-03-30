#include "lru_k_replacer.h"

#include "buffer_pool_manager.h"
#include "util/system_configuration.h"

namespace vaultdb {

    LRUKReplacer::LRUKReplacer(long frame_cnt, long k_dist)
                :  k_(k_dist), frame_cnt_(frame_cnt), bpm_(SystemConfiguration::getInstance().bpm_) {}

    bool LRUKReplacer::isEvictable(frame_id_t frame_id) {
        if (bpm_.reverse_position_map_.find(frame_id) != bpm_.reverse_position_map_.end()) {
            PageId pid = bpm_.reverse_position_map_.at(frame_id);
            return !bpm_.position_map_.at(pid).pinned_;
        }
        return true;
    }
}

