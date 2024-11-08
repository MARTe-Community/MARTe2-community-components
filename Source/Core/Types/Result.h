#ifndef RESULT_H__
#define RESULT_H__
#include "Error.h"
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
    if (x < 0) return Result(ERROR(E_MATH));
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
template <typename T, typename E = ExtendedError> class Result {
public:
  /**
    @brief Constructor of the `Result` object when
    the operation is succesful.

    @param val the result of the operation to be returned
  */
  Result(const T &val) : ok_(true) {
    res_ = new T(val);
    count_ = new uint32(1);
  }

  /**
    @brief Constructor of the `Result` object
    when the operation is unsuccesful.

    @param e the Error to be returned.
  */
  Result(const E &e) : ok_(false) {
    res_ = new E(e);
    count_ = new uint32(1);
  }
  /**
    @brief copy constructor
    @param other result to be copied
  **/
  Result(const Result &other)
      : ok_(other.ok_), res_(other.res_), count_(other.count_) {
    (*count_)++;
  }

  /**
    @brief destructor
  **/
  ~Result() { deref(); }

  /**
    @brief Method to access the value returned.

    @return the value returned from the successful operation
    */
  T val() const {
    assert(ok_);
    return *(T *)res_;
  }

  /**
    @brief Method to access the error returned.
    @return the Error of the unsuccessful operation
    @return NO_ERROR if the operation was successful.
    */
  const E err() const {
    if (!ok_)
      return *(E *)res_;
    return E();
  }

  /**
    @brief Method to check if the operation failed or not
    @return true if failed
    @return false if succeded
  **/
  bool failed() const { return !ok_; }

  /**i
    @brief Check if the operation succeded or not.
    @return true if the operation succeded and a value is stored in the Result
    object
    @return false if the operation failed and an Error is stored in the Result
    object
  */
  bool succeded() const { return ok_; }

  /**
    @brief checks if operation did not succeded
  or fail.
    @return false if succeded
    @return true if failed
  **/
  bool operator!() const { return !ok_; }

  /**
    @brief gets the value of the Result
    @return the value if succeded
  **/
  T operator+() const {
    assert(ok_);
    return *(T *)res_;
  }

  /**
    @brief gets the  error of the Result
    @return the error if failed
    @return NO_ERROR error if scucceded.
  **/
  const E operator-() const {
    assert(!ok_);
    return *(E *)res_;
  }

  /**
    @brief assign operator
    @param other result to copied
    @result updated result
  **/
  Result &operator=(const Result &other) {
    if (&other != this && other.count_ != count_) {
      deref();
      res_ = other.res_;
      ok_ = other.ok_;
      count_ = other.count_;
      (*count_)++;
    }

    return *this;
  }

private:
  bool ok_;
  void *res_;
  uint32 *count_;

  void deref() {
    if (count_ == NULL_PTR(uint32 *))
      return;
    (*count_)--;
    if (*count_ == 0) {
      if (ok_) {
        delete (T *)res_;
      } else {
        delete (E *)res_;
      }
      delete count_;
      count_ = NULL_PTR(uint32 *);
      res_ = NULL_PTR(void *);
    }
  }
};
} // namespace MARTe
#endif
