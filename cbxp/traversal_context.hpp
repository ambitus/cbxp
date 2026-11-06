#ifndef __CBXP_TRAVERSAL_CONTEXT_H_
#define __CBXP_TRAVERSAL_CONTEXT_H_

#include <memory>
#include <nlohmann/json.hpp>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "cbxp_types.hpp"
#include "control_blocks/control_block.hpp"

namespace CBXP {

// Hash for (address, cb_name) pairs used as VisitedMap keys.
struct PairHash {
  std::size_t operator()(
      const std::pair<const void*, std::string>& p) const noexcept {
    std::size_t h1 = std::hash<const void*>{}(p.first);
    std::size_t h2 = std::hash<std::string>{}(p.second);
    return h1 ^ (h2 << 32 | h2 >> 32);
  }
};

// Result JSON per enqueued (address, cb_type): false = in progress (cycle
// break), null = empty/unreachable, object/array = fully resolved.
using VisitKey    = std::pair<const void*, std::string>;
using VisitedMap  = std::unordered_map<VisitKey, nlohmann::json, PairHash>;
using VisitKeySet = std::unordered_set<VisitKey, PairHash>;
using VisitKeyMap = std::unordered_map<VisitKey, VisitKey, PairHash>;

class ControlBlockNode;

// One item in the BFS work-queue.
struct PendingNode {
  const void* p_address;
  std::string cb_name;
  cbxp_options_t options;
  // True for nodes seeded by run() (roots), false for children; traverse uses
  // it to decide whether missing filters make the reachability check refuse.
  bool is_root = false;
  // Roots only: the node run() built while validating options, reused by
  // traverse().  Null for children.
  std::shared_ptr<ControlBlockNode> prepared = nullptr;
};

// All shared, mutable traversal state.  Passed by reference into every
// ControlBlockNode so nodes never own traversal state themselves.
struct TraversalContext {
  const std::unordered_map<std::string, ControlBlock>& control_blocks;
  bool skip_buffer_length_check;
  VisitedMap visited;
  std::queue<PendingNode> queue;
  // Nodes rejected by their own filters (visited holds null); lets a parent
  // tell this apart from inaccessible / eyecatcher mismatch.
  VisitKeySet filter_rejected;
  // Included child name -> whether it has filters (can reject its parent);
  // used by TraversalEngine::resolvePlaceholders.
  std::unordered_map<VisitKey, std::unordered_map<std::string, bool>, PairHash>
      child_filtered;
};

}  // namespace CBXP

#endif
