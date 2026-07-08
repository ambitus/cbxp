#pragma once

#include <cstdint>
#include <unordered_map>

// IBM z/OS Storage Subpool attributes
// Source: https://www.ibm.com/docs/en/zos/3.1.0?topic=summary-storage-subpools
// Columns captured: Subpool number, Location (Common vs Private), Fetch
// Protection

namespace zos {

// ---------------------------------------------------------------------------
// Whether the subpool belongs to the Common or Private address space.
// ---------------------------------------------------------------------------
enum class SubpoolOwnership : uint8_t {
  Private,
  Common,
};

// ---------------------------------------------------------------------------
// The named storage region within the owning address space.
//
//  Low   = below the 16 MB line (private only)
//  High  = above the 16 MB line (private only)
//  LSQA  = Local System Queue Area          (private only)
//  SQA   = System Queue Area                (common only)
//  CSA   = Common Service Area              (common only)
// ---------------------------------------------------------------------------
enum class SubpoolRegion : uint8_t {
  Low,   // Private: below 16 MB line
  High,  // Private: above 16 MB line
  LSQA,  // Private: Local System Queue Area
  SQA,   // Common:  System Queue Area
  CSA,   // Common:  Common Service Area
};

// ---------------------------------------------------------------------------
// Whether the subpool spans only the base (non-extended) region, only the
// Extended region (above 2 GB bar), or Both.
//
//  Base     – traditional 31-bit region only  (e.g. "SQA"  / "LSQA")
//  Extended – 64-bit extended region only     (e.g. "ESQA" / "ELSQA")
//  Both     – subpool covers base AND extended (e.g. "SQA/ESQA", "LSQA/ELSQA")
// ---------------------------------------------------------------------------
enum class SubpoolExtent : uint8_t {
  Base,
  Extended,
  Both,
};

// ---------------------------------------------------------------------------
// Full descriptor for a subpool entry.
// ---------------------------------------------------------------------------
struct SubpoolLocation {
  SubpoolOwnership ownership;
  SubpoolRegion region;
  SubpoolExtent extent;
};

struct SubpoolInfo {
  SubpoolLocation location;
  bool fetch_protected;
};

// ---------------------------------------------------------------------------
// Convenience construction aliases — keeps the map readable.
// ---------------------------------------------------------------------------
namespace detail {
using O = SubpoolOwnership;
using R = SubpoolRegion;
using E = SubpoolExtent;

// Private shorthands
static constexpr SubpoolLocation PrivLowBase{O::Private, R::Low, E::Base};
static constexpr SubpoolLocation PrivHighBase{O::Private, R::High, E::Base};
static constexpr SubpoolLocation PrivLSQABase{O::Private, R::LSQA, E::Base};
static constexpr SubpoolLocation PrivLSQAExt{O::Private, R::LSQA, E::Extended};
static constexpr SubpoolLocation PrivLSQABoth{O::Private, R::LSQA, E::Both};

// Common shorthands
static constexpr SubpoolLocation CommSQABoth{O::Common, R::SQA, E::Both};
static constexpr SubpoolLocation CommSQAExt{O::Common, R::SQA, E::Extended};
static constexpr SubpoolLocation CommCSABoth{O::Common, R::CSA, E::Both};
static constexpr SubpoolLocation CommCSAExt{O::Common, R::CSA, E::Extended};
}  // namespace detail

// ---------------------------------------------------------------------------
// Map from subpool decimal number → SubpoolInfo.
// Subpools 0–127 are handled as a range in subpool_info_for(); they are NOT
// stored individually here.
// ---------------------------------------------------------------------------
// clang-format off
inline const std::unordered_map<uint32_t, SubpoolInfo> kSubpoolMap = {
    //  SP  │ location (ownership, region, extent)  │ fetch_protected
    //  ────┼────────────────────────────────────────┼────────────────

    // ── 129–134 ─────────────────────────────────────────────────────────────
    //   IBM table: "Private (low)" — standard 31-bit private storage.
    //   Notes 24 on 133/134 indicate possible ELPA-range behavior but the
    //   table still lists them as Private low.
    { 129, { detail::PrivLowBase,  true  } },  // 0x81
    { 130, { detail::PrivLowBase,  false } },  // 0x82
    { 131, { detail::PrivLowBase,  true  } },  // 0x83
    { 132, { detail::PrivLowBase,  false } },  // 0x84
    { 133, { detail::PrivLowBase,  true  } },  // 0x85  (see IBM note 24)
    { 134, { detail::PrivLowBase,  false } },  // 0x86  (see IBM note 24)

    // ── 203–205 ─────────────────────────────────────────────────────────────
    //   IBM table: "Private ELSQA" — Extended LSQA only, no base counterpart.
    { 203, { detail::PrivLSQAExt,  false } },  // 0xCB
    { 204, { detail::PrivLSQAExt,  false } },  // 0xCC
    { 205, { detail::PrivLSQAExt,  false } },  // 0xCD

    // ── 213–215 ─────────────────────────────────────────────────────────────
    { 213, { detail::PrivLSQAExt,  true  } },  // 0xD5
    { 214, { detail::PrivLSQAExt,  true  } },  // 0xD6
    { 215, { detail::PrivLSQAExt,  true  } },  // 0xD7

    // ── 223–225 ─────────────────────────────────────────────────────────────
    { 223, { detail::PrivLSQAExt,  true  } },  // 0xDF
    { 224, { detail::PrivLSQAExt,  true  } },  // 0xE0
    { 225, { detail::PrivLSQAExt,  true  } },  // 0xE1

    // ── 226–231 ─────────────────────────────────────────────────────────────
    //   IBM table "SQA/ESQA"  → Common SQA, Both extents.
    //   IBM table "CSA/ECSA"  → Common CSA, Both extents.
    { 226, { detail::CommSQABoth,  false } },  // 0xE2  SQA/ESQA
    { 227, { detail::CommCSABoth,  true  } },  // 0xE3  CSA/ECSA
    { 228, { detail::CommCSABoth,  false } },  // 0xE4  CSA/ECSA
    { 229, { detail::PrivHighBase, true  } },  // 0xE5  Private high
    { 230, { detail::PrivHighBase, false } },  // 0xE6  Private high
    { 231, { detail::CommCSABoth,  true  } },  // 0xE7  CSA/ECSA

    // ── 233–237 ─────────────────────────────────────────────────────────────
    //   IBM table "LSQA/ELSQA" → Private LSQA, Both extents.
    { 233, { detail::PrivLSQABoth, false } },  // 0xE9  LSQA/ELSQA
    { 234, { detail::PrivLSQABoth, false } },  // 0xEA  LSQA/ELSQA
    { 235, { detail::PrivLSQABoth, false } },  // 0xEB  LSQA/ELSQA
    { 236, { detail::PrivHighBase, false } },  // 0xEC  Private high
    { 237, { detail::PrivHighBase, false } },  // 0xED  Private high

    // ── 239 ─────────────────────────────────────────────────────────────────
    { 239, { detail::CommSQABoth,  true  } },  // 0xEF  SQA/ESQA

    // ── 240–255 ─────────────────────────────────────────────────────────────
    { 240, { detail::PrivLowBase,  true  } },  // 0xF0
    { 241, { detail::CommCSABoth,  false } },  // 0xF1  CSA/ECSA
    { 242, { detail::CommCSABoth,  false } },  // 0xF2  CSA/ECSA  [change marker]
    { 243, { detail::CommCSABoth,  false } },  // 0xF3  CSA/ECSA  [change marker]
    { 244, { detail::PrivLowBase,  false } },  // 0xF4
    { 245, { detail::CommSQABoth,  false } },  // 0xF5  SQA/ESQA
    { 246, { detail::PrivLowBase,  false } },  // 0xF6           [change marker]
    //   IBM table "ESQA" (no base SQA) → Common SQA, Extended only.
    { 247, { detail::CommSQAExt,   true  } },  // 0xF7  ESQA only
    { 248, { detail::CommSQAExt,   false } },  // 0xF8  ESQA only
    { 249, { detail::PrivHighBase, false } },  // 0xF9  Private high
    { 250, { detail::PrivLowBase,  true  } },  // 0xFA
    { 251, { detail::PrivLowBase,  true  } },  // 0xFB
    { 252, { detail::PrivLowBase,  false } },  // 0xFC
    { 253, { detail::PrivLSQABoth, false } },  // 0xFD  LSQA/ELSQA
    { 254, { detail::PrivLSQABoth, false } },  // 0xFE  LSQA/ELSQA
    { 255, { detail::PrivLSQABoth, false } },  // 0xFF  LSQA/ELSQA
};
// clang-format on

// ---------------------------------------------------------------------------
// Predicates
// ---------------------------------------------------------------------------
[[nodiscard]] inline bool is_common(const SubpoolLocation& loc) noexcept {
  return loc.ownership == SubpoolOwnership::Common;
}

// cppcheck-suppress unusedFunction
[[nodiscard]] inline bool is_private(const SubpoolLocation& loc) noexcept {
  return loc.ownership == SubpoolOwnership::Private;
}

// cppcheck-suppress unusedFunction
[[nodiscard]] inline bool is_extended(const SubpoolLocation& loc) noexcept {
  return loc.extent == SubpoolExtent::Extended ||
         loc.extent == SubpoolExtent::Both;
}

// cppcheck-suppress unusedFunction
[[nodiscard]] inline bool is_base(const SubpoolLocation& loc) noexcept {
  return loc.extent == SubpoolExtent::Base;
}

// ---------------------------------------------------------------------------
// Primary lookup.  Handles the 0–127 range inline (all: Private Low Base,
// fetch-protected).  Returns nullptr for any subpool not in the IBM table.
// ---------------------------------------------------------------------------
[[nodiscard]] inline const SubpoolInfo* subpool_info_for(uint32_t sp) noexcept {
  static constexpr SubpoolInfo k0to127{detail::PrivLowBase, true};
  if (sp <= 127) {
    return &k0to127;
  }
  auto it = kSubpoolMap.find(sp);
  if (it == kSubpoolMap.end()) {
    return nullptr;
  }
  return &it->second;
}

}  // namespace zos
