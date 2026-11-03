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
  // True if child-level filters rejected every entry; the engine then rejects
  // the parent too.
  bool child_filter_failed;
};

// Parses one control block at a known address; included pointer targets are
// queued on ctx.queue as hex placeholders.  Never modifies ctx.visited.
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

  // Reads one field and returns its JSON; included pointer targets are queued
  // on ctx_.queue and returned as raw pointer hex placeholders.
  nlohmann::json fieldToJson(const control_block_field_t& field,
                             ptrdiff_t extra_offset = 0);

  // Push pending_children_ onto ctx_.queue (skipping already-visited ones).
  void enqueuePendingChildren();

  // Builds the JSON for this node's fields; child_filter_failed is set when a
  // repeated pointer field's entries were all rejected by child filters.
  nlohmann::json parseFields(bool& child_filter_failed);
};

}  // namespace CBXP

#endif
