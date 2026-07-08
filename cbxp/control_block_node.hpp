#ifndef __CBXP_CONTROL_BLOCK_NODE_H_
#define __CBXP_CONTROL_BLOCK_NODE_H_

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

#include "cbxp_types.hpp"
#include "control_blocks/control_block.hpp"
#include "filter_set.hpp"
#include "include_resolver.hpp"
#include "traversal_context.hpp"

namespace CBXP {

// Result returned by ControlBlockNode::parse().
struct NodeParseResult {
  // The parsed JSON for this node, or null if the eyecatcher mismatched.
  nlohmann::json json;
  // True when this node's own filter matched (or there were no filters).
  // False when the filter rejected the node.
  bool filter_passed;
  // True when at least one child entry was rejected by a child-level filter
  // and no child entries survived.  The engine uses this to also reject the
  // parent.
  bool child_filter_failed;
};

// Parses a single control block at a known address.
//
// ControlBlockNode is intentionally thin:
//  - It reads fields from `address` using the schema in `cb_name`.
//  - For pointer fields that appear in `options.include_patterns`, it emits
//    the raw hex pointer as a placeholder and pushes the target onto
//    ctx.queue (BFS) instead of recursing.
//  - It never modifies ctx.visited — that is the engine's responsibility.
class ControlBlockNode {
 public:
  ControlBlockNode(TraversalContext& ctx, const cbxp_options_t& options,
                   const std::string& cb_name, const void* address,
                   size_t buffer_length = 0);

  // Returns a copy of this node bound to `address`.  The include and filter
  // lists are copied, not rebuilt, so nothing is re-parsed or re-logged.
  ControlBlockNode withAddress(const void* address) const;

  NodeParseResult parse();

  // Included child control block name -> true if that child has filters.
  std::unordered_map<std::string, bool> childFilterMap() const;

 private:
  TraversalContext& ctx_;
  const ControlBlock& cb_;
  const void* address_;
  size_t buffer_length_;
  std::unordered_map<std::string, cbxp_options_t> child_options_;
  FilterSet filter_set_;
  // Children discovered while parsing fields; enqueued only if this node's
  // own filters pass.
  std::vector<PendingNode> pending_children_;

  bool eyecatcherMatches() const;
  void checkDataLength(ptrdiff_t offset) const;
  ptrdiff_t getReadableLength() const;

  // Read one field from memory and return its JSON value.
  // For pointer fields whose target is in child_options_, returns the raw
  // pointer hex as a placeholder and pushes a PendingNode onto ctx_.queue.
  nlohmann::json fieldToJson(const control_block_field_t& field,
                             ptrdiff_t extra_offset = 0);

  // Push pending_children_ onto ctx_.queue (skipping already-visited ones).
  void enqueuePendingChildren();

  // Build the full JSON object for this node's fields (non-repeated then
  // repeated).  child_filter_failed is set to true if a repeated pointer
  // field's entries were all rejected by child filters.
  nlohmann::json parseFields(bool& child_filter_failed);
};

}  // namespace CBXP

#endif
