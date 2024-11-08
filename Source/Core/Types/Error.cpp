#include "error.h"
#include <stdio.h>

namespace ect {

ExtendedError::ExtendedError(const int ec, const char *path, const int ln,
                             const char *msg)
    : Error(ec), file_(path), msg_(msg), line_(ln) {}

ExtendedError::ExtendedError()
    : Error(NO_ERROR), file_(""), msg_(""), line_(0){};

ExtendedError::ExtendedError(const ExtendedError &e)
    : Error(e.code()), file_(e.file_), msg_(e.msg_), line_(e.line_) {}

ExtendedError::~ExtendedError() { msg_.clear(); }

char *ExtendedError::what() const {
  char* msg = new char[256];
  sprintf(msg, "Error %d found at %s:%d : `%s`\n", code(), file_, line_,
          msg_.cstr());
  return msg;
}

const char *ExtendedError::file() const { return file_; }

const char *ExtendedError::msg() const { return msg_.cstr(); }

int ExtendedError::line() const { return line_; }

Error::Error() : code_(0) {}
Error::Error(const int code) : code_(code) {}
Error::~Error() {}

int Error::code() const { return code_; }
int Error::error_code() const { return code_; }

char *Error::what() const {
  char *msg = new char[256];
  sprintf(msg, "Error %d", code());
  return msg;
}
} // namespace ect
