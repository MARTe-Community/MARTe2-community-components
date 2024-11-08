#ifndef HY_ERROR_H__
#define HY_ERROR_H__

#include "StreamString.h"

#define ERROR(EC) MARTe::ExtendedError(EC, __FILE__, __LINE__)
#define ERRMSG(EC, MSG) MARTe::ExtendedError(EC, __FILE__, __LINE__, MSG)

namespace MARTe {

/**
 @brief Generic error codes
**/
enum ErrorCode {
  NO_ERROR = 0,        // No error code
  E_GENERIC,           // Generic error
  E_INPUT,             // Input related error
  E_OUTPUT,            // Output related error
  E_MATH,              // Mathematical related erro
  E_NULLPTR,           // Null pointer error
  E_OUT_OF_MEM,        // Out of memory error
  E_COMM,              // Communication error
  E_TIME_OUT,          // Time out error
  E_OUT_OF_BOUNDS,     // Out of bounds error
  E_SM_PROCESS = 0xa0, // Error during state processing
  E_SM_POST = 0xa1,    // Error during post prrocessing
  E_SM_PRE = 0xa2,     // Error duirng pre processing
  E_SM_UNKNOW = 0xaa   // Should never heppen
};

class Error {
public:
  /**
    @brief empty constructor, set the error code to 0
  **/
  Error();
  /**
    @brief error constructor
    @param code is the error code to be stored
  **/
  Error(const int code);
  /**
    @brief default destructor
  **/
  virtual ~Error();

  /**
    @brief get the error code
    @return the error code
  **/
  int code() const;
  /**
    @deprecated
  **/
  int error_code() const;

  /**
    @brief get a simple but complete error description as the following:
    "Error ERROR_CODE"

    @return what is the cause of the error
  **/
  virtual char *what() const;

private:
  const int code_;
};
/**
  @brief simple error class

  It contains an error code as well as the error source pointer (file and line
  number) as well as an optional text message
**/
class ExtendedError : public Error {
public:
  /**
    @brief full constructor
    @param code error code
    @param path file path
    @param line number
    @param msg additional information as a message
  **/
  ExtendedError(const int code, const char *path, const int line,
                const char *msg = "");
  /**
    @brief copy constructor
    @param error to be copied
    **/
  ExtendedError(const ExtendedError &);
  /**
    @brief empty error constructor.
  **/
  ExtendedError();

  virtual ~ExtendedError();
  /**
    @brief get a simple but complete error description as the following:
    "Error ERROR_CODE found at FILE:LINE_NUMBER : `MSG`"

    @return what is the cause of the error
  **/
  virtual char *what() const;

  /**
    @brief getter for the error message
    @return message string
  **/
  const char *msg() const;
  /**
    @brief getter for the file origin of the error
    @return file name
  **/
  const char *file() const;
  /**
    @brief getter for line number origin of the error
    @return line number
  **/
  int line() const;

private:
  const char *file_;
  StreamString msg_;
  int line_;
};

// class ErrorManager {
//   static const u32 MAX_ERR_BUFF = 256;
//   static ErrorManager *inst_;

//   ErrorManager(bool verbose=true);
//   Error *buffer_[MAX_ERR_BUFF];
//   u32 counter_;
//   bool verbose_;

// public:
//   static u32 push(Error err);
//   static Error pop();

//   static bool empty();
//   static u32 numberOfErrors();

//   static bool verbose();
//   static bool enableVerbosity(bool enabled);
// };

}; // namespace MARTe
#endif
