#ifndef __CBXP_SAFE_READ_H_
#define __CBXP_SAFE_READ_H_

#include <cstddef>

namespace CBXP {

// Copies n bytes from storage that may be unreadable (fetch-protected,
// unmapped, ...). Returns false instead of abending with S0C4 if the copy
// raises a hardware exception. The contents of dst are indeterminate on
// failure.
bool safeCopy(void* dst, const void* src, size_t n);

}  // namespace CBXP

#endif
