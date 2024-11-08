#ifndef OPTION_H__
#define OPTION_H__
#include "CompilerTypes.h"
#include <assert.h>

namespace MARTe {

/**
  @brief Option<T> class is a container used for
  when an operation can return a null value.

  If an operation can result in a `NULL` using Option<T> should
  be safer.
  It is then possible to return the value by returning `Option(VALUE)` or
  the empty `Option()`.

  It is possible to check if the optional is empty or not with the `empty()`
  method or casting it to `bool` but in this case is the opposite. To access the
  value stored inside it is possible to use the method `val()` or casting it to
  `T`.

*/

template <typename T> class Option {
  T *mem;

public:
  static inline Option<T> maybe(T (*fn)(T), const Option<T> &x) {
    if (x.empty()) {
      return x;
    }
    return fn(x.const_val());
  }
  /**
    @brief Constructor of a non empty Optional
    container.

    @param val the value to be stored.
  */
  inline Option(const T &val) : mem(new T(val)) {}
  /**
    @brief Constructor of an empty Optional.
  */
  inline Option() : mem(NULL_PTR(T *)) {}
  /**
    @brief Copy constructor.
    **/
  inline Option(const Option &other) {
    if (other.mem == NULL_PTR(T *)) {
      mem = NULL_PTR(T *);
    } else {
      mem = new T(*other.mem);
    }
  }

  inline ~Option() {
    if (mem != NULL_PTR(T *)) {
      delete mem;
      mem = NULL_PTR(T *);
    }
  }

  /**
    @brief Checks if the Optional container is
    empty or not.
    @return true if it is empty.
    @return false if it is full.
  */
  inline bool empty() const { return mem == NULL_PTR(T *); }

  /**
    @brief Retrives the value contained in the
    container.
    @return the value stored.
  */
  inline T &val() {
    assert(mem != NULL_PTR(T *));
    return *mem;
  }
  inline const T const_val() const {
    assert(mem != NULL_PTR(T *));
    return *mem;
  }
  /**
    @brief Checks if the optional container is full
    or not.
    @return false if full
    @return true if empty
  */
  inline bool operator!() const { return mem == NULL_PTR(T *); }

  /**
    @brief Retrieves the value contained.
    @return the value using the method val()
  */
  inline T &operator~() { return val(); }

  /**
    @brief assign operator
    @param value to be copied inside the container
    @return updated optional value
  **/
  inline Option &operator=(const T &other) {
    if (mem == NULL_PTR(T *)) {
      mem = new T(other);
    } else {
      *mem = other;
    }
    return *this;
  }

  /**
    @brief convert optional to bool, by checking if it is assigned or not.
    @return true if is not empty.
  **/
  inline operator bool() const { return mem != NULL_PTR(T *); }

  /**
    @brief assign operator
    @param other optional value to be copied
    @return updated optional value
  **/
  inline Option &operator=(const Option &other) {
    if (mem != NULL_PTR(T *)) {
      delete mem;
      mem = NULL_PTR(T *);
    }
    if (other.mem != NULL_PTR(T *)) {
      mem = new T(*other.mem);
    }
    return *this;
  }
  /**
   * @brief access internal methods/field of the pointer.
   **/
  inline T *operator->() {
    assert(mem != NULL_PTR(T *));
    return mem;
  }
};

} // namespace MARTe
#endif
