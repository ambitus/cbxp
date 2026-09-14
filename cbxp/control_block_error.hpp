#ifndef __CONTROL_BLOCK_ERROR_H_
#define __CONTROL_BLOCK_ERROR_H_

namespace CBXP {
enum Error {
  BadControlBlock = 1,
  BadInclude      = 2,
  BadFilter       = 3,
  DataTooSmall    = 4,
  NullDataPtr     = 5,
  BadCbxpPath     = 6,
  BadJsonFile     = 7,
  CantReachBlock  = 8,
};

class CBXPError : public std::exception {
 private:
  Error error_code_;

 public:
  explicit CBXPError(const Error& rc) : error_code_(rc) {}
  const unsigned int getErrorCode() const { return error_code_; }
};

class ControlBlockError : public CBXPError {
 public:
  ControlBlockError() : CBXPError(Error::BadControlBlock) {}
};

class IncludeError : public CBXPError {
 public:
  IncludeError() : CBXPError(Error::BadInclude) {}
};

class FilterError : public CBXPError {
 public:
  FilterError() : CBXPError(Error::BadFilter) {}
};

class DataLengthError : public CBXPError {
 public:
  DataLengthError() : CBXPError(Error::DataTooSmall) {}
};

class CbxpPathError : public CBXPError {
 public:
  CbxpPathError() : CBXPError(Error::BadCbxpPath) {}
};

class CbxpJsonError : public CBXPError {
 public:
  CbxpJsonError() : CBXPError(Error::BadJsonFile) {}
};

class CbxpReachBlockError : public CBXPError {
 public:
  CbxpReachBlockError() : CBXPError(Error::CantReachBlock) {}
};

}  // namespace CBXP

#endif
