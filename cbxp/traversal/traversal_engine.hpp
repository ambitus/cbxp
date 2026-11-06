#ifndef __CBXP_TRAVERSAL_ENGINE_H_
#define __CBXP_TRAVERSAL_ENGINE_H_

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "cbxp_types.hpp"
#include "control_blocks/control_block.hpp"
#include "traversal/traversal_context.hpp"

namespace CBXP {

// Drives the BFS traversal of a control block tree.  Pass address=nullptr to
// extract from live memory, or a caller-supplied address to format.
class TraversalEngine {
 public:
  explicit TraversalEngine(
      const std::unordered_map<std::string, ControlBlock>& control_blocks,
      bool skip_buffer_length_check);

  // Runs the traversal rooted at `cb_name`: returns the root object, an array
  // for multiple roots, or null if rejected / no root found.
  nlohmann::json run(const std::string& cb_name, const cbxp_options_t& options,
                     const void* p_address, size_t buffer_length);

 private:
  TraversalContext ctx_;

  // Resolves the root address(es) of `cb` by walking its pointedToBy chain
  // from a fixed anchor address.
  std::vector<const void*> findRootAddresses(const ControlBlock& cb) const;

  // BFS loop: pops each node, parses it and stores its JSON in ctx_.visited;
  // parsing enqueues children, so the queue grows until the graph is visited.
  void traverse();

  // BFS from `root_key` mapping each node to its shallowest parent (its
  // owner); the root maps to itself.
  VisitKeyMap computeOwners(const VisitKey& root_key) const;

  // Replaces pointer placeholders in `json` with child JSON from ctx_.visited;
  // returns false if `node_key` must be rejected by its filters.  `owners` and
  // `expanded` limit each node to one expansion, under its shallowest parent.
  // Rejected entries of a repeated pointer field are dropped; the node is
  // rejected only if every entry was.
  bool resolvePlaceholders(nlohmann::json& json, const VisitKey& node_key,
                           VisitKeySet& expanded,
                           const VisitKeyMap& owners) const;

  // Check storage-attribute reachability for the root control block.
  // Throws CbxpBlockUnreachableError if the block cannot be accessed.
  static void checkReachability(const ControlBlock& cb, bool has_filters);
};

}  // namespace CBXP

#endif
