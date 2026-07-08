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

// Stores the JSON result for every (address, cb_type) pair that has been
// enqueued.  Values follow a three-state protocol:
//   boolean false  — in-progress sentinel (cycle break)
//   null           — visited but legitimately empty (eyecatcher mismatch, etc.)
//   object/array   — fully resolved
using VisitKey    = std::pair<const void*, std::string>;
using VisitedMap  = std::unordered_map<VisitKey, nlohmann::json, PairHash>;
using VisitKeySet = std::unordered_set<VisitKey, PairHash>;
using VisitKeyMap = std::unordered_map<VisitKey, VisitKey, PairHash>;

class ControlBlockNode;

// One item in the BFS work-queue.
struct PendingNode {
  const void* address;
  std::string cb_name;
  cbxp_options_t options;
  // True only for nodes seeded directly by run() (root nodes).
  // False for all nodes enqueued by ControlBlockNode::fieldToJson (children).
  // Used by drainQueue to decide whether the reachability check should
  // treat missing filters as a reason to refuse access.
  bool is_root = false;
  // Root nodes only: a node already built by TraversalEngine::run() while it
  // validated the options.  drainQueue reuses it instead of rebuilding (and
  // re-logging) the include and filter lists.  Null for child nodes.
  std::shared_ptr<ControlBlockNode> prepared = nullptr;
};

// All shared, mutable traversal state.  Passed by reference into every
// ControlBlockNode so nodes never own traversal state themselves.
struct TraversalContext {
  const std::unordered_map<std::string, ControlBlock>& control_blocks;
  bool skip_buffer_length_check;
  VisitedMap visited;
  std::queue<PendingNode> queue;
  // Nodes whose own filters (or a repeated-child roll-up) rejected them.
  // visited holds null for these; this set lets a parent distinguish
  // "rejected by a filter" from "inaccessible / eyecatcher mismatch".
  VisitKeySet filter_rejected;
  // For every parsed node: included child name -> whether that child carries
  // filters (and can therefore reject its parent).  Used by
  // TraversalEngine::resolvePlaceholders.
  std::unordered_map<VisitKey, std::unordered_map<std::string, bool>, PairHash>
      child_filtered;
};

}  // namespace CBXP

#endif
