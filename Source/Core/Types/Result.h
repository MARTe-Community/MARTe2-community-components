#ifndef RESULT_H__
#define RESULT_H__
#include "ErrorType.h"
#include <assert.h>

namespace MARTe {

/**
  @brief Result<T> is a container class for elegant handling of operation that
  can generate exceptions.

  If an operation can results in an exception the type Result<T> can be used
  insted of throwing the exception, if the operation is successful the value
  will be stored and will be accessible via both the operator (T) or the method
  val(). In the other case the `Error` struct can be retrived via the operator
  (Error) or the method err(). To check if the operation is succeded (and so if
  the value or the error as to be checked) both the operator (bool) or the
  method succeded() can be used. Finally if the value (both by the operator or
  method) is accessed when not defined the error will be thrown as exception.

  Below an example of usage can be found:
  <pre>
  Result<float> safe::sqrt(float x) {
    if (x < 0) return Result<float>::Fail(RuntimeError);
    return math::sqrt(x);
  }
  </pre>
  Then the usage of such function could be something as:
  <pre>
    Result<float> res = safe::sqrt(x);
    if (res) {
      // process the result
    } else {
      // manage the error
    }
  </pre>
*/
template <typename T, typename E = ErrorManagement::ErrorType> class Result {
public:
  inline static Result<T, E> Succ(const T &value) {
    Result r;
    r.ok_ = true;
    r.res_ = new T(value);
    return r;
  }
  inline static Result<T, E> Fail(const E &error) {
    Result r;
    r.ok_ = false;
    r.res_ = new E(error);
    return r;
  }
  /**
    @brief copy constructor
    @param other result to be copied
  **/
  inline Result(const Result &other) : ok_(other.ok_), res_(NULL_PTR(void *)) {
    if (other.res_ != NULL_PTR(void *)) {
      if (ok_) {
        res_ = new T(*(T *)other.res_);
      } else {
        res_ = new E(*(E *)other.res_);
      }
    }
  }

  inline Result(const T &value) : ok_(true) { res_ = new T(value); }
  /**
    @brief destructor
  **/
  inline ~Result() { clear_res(); }

  /**
    @brief Method to access the value returned.

    @return the value returned from the successful operation
    */
  inline T val() const {
    assert(ok_);
    return *(T *)res_;
  }

  /**
    @brief Method to access the error returned.
    @return the Error of the unsuccessful operation
    @return NO_ERROR if the operation was successful.
    */
  inline const E err() const {
    assert(!ok_);
    return *(E *)res_;
  }

  /**
    @brief Method to check if the operation failed or not
    @return true if failed
    @return false if succeded
  **/
  inline bool failed() const { return !ok_; }

  /**i
    @brief Check if the operation succeded or not.
    @return true if the operation succeded and a value is stored in the Result
    object
    @return false if the operation failed and an Error is stored in the Result
    object
  */
  inline bool succeded() const { return ok_; }

  /**
    @brief checks if operation did not succeded
  or fail.
    @return false if succeded
    @return true if failed
  **/
  inline bool operator!() const { return !ok_; }

  /**
    @brief gets the value of the Result
    @return the value if succeded
  **/
  inline T operator+() const {
    assert(ok_);
    return *(T *)res_;
  }

  /**
    @brief gets the  error of the Result
    @return the error if failed
    @return NO_ERROR error if scucceded.
  **/
  inline const E operator-() const {
    assert(!ok_ && res_ != NULL_PTR(void *));
    return *(E *)res_;
  }

  /**
    @brief assign operator
    @param other result to copied
    @result updated result
  **/
  inline Result &operator=(const Result &other) {
    if (&other != this) {
      ok_ = other.ok_;
      clear_res();
      if (other.res_ != NULL_PTR(void *)) {
        if (ok_) {
          res_ = new T(*(T *)other.res_);
        } else {
          res_ = new E(*(E *)other.res_);
        }
      }
    }

    return *this;
  }

private:
  bool ok_;
  void *res_;
  inline void clear_res() {

    if (res_ != NULL_PTR(void *)) {
      if (ok_) {
        delete (T *)res_;
      } else {
        delete (E *)res_;
      }
    }
    res_ = NULL_PTR(void *);
  }

  inline Result() : ok_(false), res_(NULL_PTR(void *)) {}
};
} // namespace MARTe
#endif
