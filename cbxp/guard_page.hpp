#ifndef __CBXP_GUARD_PAGE_H_
#define __CBXP_GUARD_PAGE_H_

#include <cstdint>

namespace CBXP {

// The PSA prefix area 0x7FFFF000-0x7FFFFFFF is fetch-protected, so any access
// from a non-zero key causes a 0C4 before C++ exception handling can
// intervene.  Pointers into it must be rejected before any memory access.
constexpr uintptr_t kGuardPageStart   = 0x7FFFF000u;
constexpr uintptr_t kGuardPageEnd     = 0x7FFFFFFFu;
constexpr const char* kGuardPageRange = "0x7FFFF000-0x7FFFFFFF";

inline bool isGuardPageAddress(const void* addr) {
  const uintptr_t p = reinterpret_cast<uintptr_t>(addr);
  return p >= kGuardPageStart && p <= kGuardPageEnd;
}

}  // namespace CBXP

#endif
