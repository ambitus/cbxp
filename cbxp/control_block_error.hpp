#ifndef __CONTROL_BLOCK_ERROR_H_
#define __CONTROL_BLOCK_ERROR_H_

namespace CBXP {
enum Error { BadControlBlock = 1, BadInclude };
class CBXPError : public std::exception {
 private:
  Error error_code_;

 public:
  explicit CBXPError(const Error& rc) : error_code_(rc) {}
  const int getErrorCode() const { return error_code_; }
};

class ControlBlockError : public CBXPError {
 public:
  ControlBlockError() : CBXPError(Error::BadControlBlock) {}
};

class IncludeError : public CBXPError {
 public:
  IncludeError() : CBXPError(Error::BadInclude) {}
};

}  // namespace CBXP

#endif
