#ifndef LINKED_H__
#define LINKED_H__

#include "CompilerTypes.h"

namespace MARTe {
/**
  @brief class conteiner representing a linked value, that can be
  read or write to a specific address.

  The typical use it is within a GAM or a DataSource where the synchronization
  of the value with the register or the physical layer is done at the beggining
  or at the end of the computation cycle.
**/
template <typename T> class linked {
  T value_;    // actual value
  T *pointer_; // pointer to the register

public:
  /**
    @brief linked value constructor
    @param val initial value
    @param pointer pointer to the linked register
  **/
  inline linked(const T val, T *pointer = NULL_PTR(T *))
      : value_(val), pointer_(pointer) {}

  /**
    @brief empty linked value constructor
  **/
  inline linked() : value_(), pointer_(NULL_PTR(T *)) {}

  /**
    @brief assign operator
    @param value to be assigned
    @return itself
  **/
  inline linked &operator=(const T value) {
    value_ = value;
    return *this;
  }

  /**
    @brief conversion operator
    @return the value contained
  **/
  inline operator const T &() const { return value_; }

  /**
    @brief link operation
    @param pointer to the register to be linked with the
    value
    @return itself
  **/
  inline linked &operator()(T *pointer) {
    pointer_ = pointer;
    return *this;
  }

  /**
    @brief link with generic void pointer operation
    @param pointer to the register to be linked with the
    value
    @return itself
  **/
  inline linked &operator()(void *pointer) {
    pointer_ = (T *)pointer;
    return *this;
  }
  /**
    @brief method to check if the linked value is actually linked
    @return true if the pointer is assinged
    @return false otherwise
  **/
  inline bool isLinked() const { return pointer_ != NULL_PTR(T *); }

  /**
    @brief write (copy) the value stored to the actual register
    @return itself
  **/
  inline linked &write() {
    if (pointer_ != NULL_PTR(T *)) {
      *pointer_ = value_;
    }
    return *this;
  }

  /**
    @brief read (copy) the value from the actual register to the local
    value
    @return itself
  **/
  inline linked &read() {
    if (pointer_ != NULL_PTR(T *)) {
      value_ = *pointer_;
    }
    return *this;
  }
};
} // namespace ect

#endif
