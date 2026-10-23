#ifndef __CBXP_TRAVERSAL_ENGINE_H_
#define __CBXP_TRAVERSAL_ENGINE_H_

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "cbxp_types.hpp"
#include "control_blocks/control_block.hpp"
#include "traversal_context.hpp"

namespace CBXP {

// Drives the BFS traversal of a control block tree.
//
// Usage:
//   TraversalEngine engine(control_blocks, skip_buffer_length_check);
//   nlohmann::json result = engine.run(cb_name, options, address, buf_len);
//
// For extract operations pass address=nullptr; the engine calls
// findControlBlockPointer to resolve the starting address(es).
// For format operations pass the caller-supplied address directly.
class TraversalEngine {
 public:
  explicit TraversalEngine(
      const std::unordered_map<std::string, ControlBlock>& control_blocks,
      bool skip_buffer_length_check);

  // Run the BFS traversal rooted at `cb_name`.
  // Returns the assembled JSON, wrapped under the control-block name key
  // (or name+"s" for multiple root instances), matching the contract
  // previously provided by ExplorerOptionsMap::getControlBlockData().
  nlohmann::json run(const std::string& cb_name, const cbxp_options_t& options,
                     const void* address, size_t buffer_length);

 private:
  TraversalContext ctx_;

  // Resolve the starting address(es) for `cb` by walking its pointedToBy
  // chain from a fixed anchor address.  Mirrors the old findControlBlockPointer
  // logic, now a plain private method with no mutable-member side-effects.
  std::vector<const void*> findRootAddresses(const ControlBlock& cb) const;

  // Process all nodes currently on ctx_.queue until it is empty.
  // Stores fully resolved JSON into ctx_.visited for each node.
  void drainQueue();

  // Breadth-first walk of the parsed graph from `root_key`.  Maps every
  // reachable node to the parent that first reached it, i.e. its shallowest
  // parent (ties go to the first one discovered).  The root maps to itself.
  VisitKeyMap computeOwners(const VisitKey& root_key) const;

  // After drainQueue(), walk `json` (the parsed node `node_key`) and replace
  // raw-pointer hex placeholders with the resolved child JSON from
  // ctx_.visited.  Children are resolved recursively.  An unfiltered child is
  // expanded only under its owner (see computeOwners), so nodes appear at
  // their shallowest position; every other reference (including cycles) is
  // left as a pointer placeholder, which keeps the walk linear.  `expanded`
  // holds the nodes already expanded for this root.
  // A child that was rejected by its filters rejects its parent, and
  // rejected entries of a repeated pointer field are dropped.  Returns false
  // if `node_key` itself must be rejected.
  bool resolvePlaceholders(nlohmann::json& json, const VisitKey& node_key,
                           VisitKeySet& expanded,
                           const VisitKeyMap& owners) const;

  // Returns true if addr falls in the z/OS PSA fetch-protected range
  // 0x7FFFF000–0x7FFFFFFF.  Any access to this range from a non-zero key
  // will produce a 0C4 before C++ exception handling can intervene.
  static bool isPsaProtectedAddress(const void* addr);

  // Check storage-attribute reachability for the root control block.
  // Throws CbxpReachBlockError if the block cannot be accessed.
  static void checkReachability(const ControlBlock& cb, bool has_filters);
};

}  // namespace CBXP

#endif
