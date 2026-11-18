#include "util/safe_read.hpp"

#include <__le_api.h>
#include <setjmp.h>

#include <cstring>

namespace CBXP {

namespace {

struct Recovery {
  jmp_buf env;
};

// Language Environment abends the thread/process if an exception handler
// returns, so this only ever leaves via longjmp. The token is the Recovery
// object of the safeCopy() frame that registered the handler.
void onStorageException(struct __cib*, void* token) {
  longjmp(static_cast<Recovery*>(token)->env, 1);
}

}  // namespace

// Keep this frame free of C++ objects: longjmp does not run destructors.
bool safeCopy(void* p_dst, const void* p_src, size_t n) {
  Recovery rec;
  if (setjmp(rec.env) != 0) {
    // The handler is still registered in this frame, so remove it.
    __reset_exception_handler();
    return false;
  }
  if (__set_exception_handler(onStorageException, &rec) != 0) {
    // Fail closed: never read without protection in place.
    return false;
  }
  std::memcpy(p_dst, p_src, n);
  __reset_exception_handler();
  return true;
}

}  // namespace CBXP
