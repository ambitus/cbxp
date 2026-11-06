#ifndef __CONTROL_BLOCK_ERROR_H_
#define __CONTROL_BLOCK_ERROR_H_

namespace CBXP {
enum Error {
  BadControlBlock  = 1,
  BadInclude       = 2,
  BadFilter        = 3,
  DataTooSmall     = 4,
  NullDataPtr      = 5,
  BadCbxpPath      = 6,
  BlockUnreachable = 7,
  JsonFailParse    = 8,
  JsonFailSchema   = 9,
  JsonBadValue     = 10,
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

class CbxpBlockUnreachableError : public CBXPError {
 public:
  CbxpBlockUnreachableError() : CBXPError(Error::BlockUnreachable) {}
};

class CbxpJsonParseError : public CBXPError {
 public:
  CbxpJsonParseError() : CBXPError(Error::JsonFailParse) {}
};

class CbxpSchemaValidationError : public CBXPError {
 public:
  CbxpSchemaValidationError() : CBXPError(Error::JsonFailSchema) {}
};

class CbxpJsonValueError : public CBXPError {
 public:
  CbxpJsonValueError() : CBXPError(Error::JsonBadValue) {}
};

}  // namespace CBXP

#endif
