#ifndef _EC_PTR_H__
#define _EC_PTR_H__
#include "CompilerTypes.h"
#include <assert.h>

namespace MARTe {

/**
 @brief a simple smart pointer.

 This template permits to incapsulate a pointer or
 a value in form of a smart pointer.
 It uses a reference counter to auto free itself when
 nomore references exists.

 In order to avoid cyclic references it the reference
 can be hard (default) or weak (optional).
 When weak the reference counter is not increased but
 instead a weak counter is increased.

 An additional boolean pointer shared between hard and weak
 pointer is used to verify if the pointer has been
 nullified or not.
**/
template <typename T> class Rc {
private:
  T *pointer_;
  uint32 *refCounter_;
  uint32 *weakCounter_;
  bool *isNull_;
  bool isWeak_;

  /**
   * @brief free weak memory
   *
   * free the additional weak pointer information once are not needed
   * any more.
   *
   * The fileds to delete are:
   *  - `isNull_`
   *  - `weakCounter_`
   **/
  inline void free_weak() {
    delete weakCounter_;
    delete isNull_;
    weakCounter_ = NULL_PTR(uint32 *);
    isNull_ = NULL_PTR(bool *);
    pointer_ = NULL_PTR(T *);
  }

  /**
   * @brief dereferanciate the current pointer.
   *
   * This method will automatically called when the smart pointer is out of
   *scope, if it is an hard link it will automatically decount the reference
   *counter and if it reaches 0 it will free the memory.
   **/
  inline void dereferenciate() {
    if (!isWeak_ && refCounter_ != NULL_PTR(uint32 *)) {
      *refCounter_ = *refCounter_ - 1;
      if (*refCounter_ <= 0) {
        delete refCounter_;
        delete pointer_;
        pointer_ = NULL_PTR(T *);
        refCounter_ = NULL_PTR(uint32 *);
        *isNull_ = true;
      }
    } else if (isWeak_ && weakCounter_ != NULL_PTR(uint32 *)) {
      *weakCounter_ = *weakCounter_ - 1;
    }

    if (weakCounter_ != NULL_PTR(uint32 *) && *weakCounter_ <= 0 &&
        (isNull_ == NULL_PTR(bool *) || *isNull_)) {
      free_weak();
    }
  }

public:
  /**
   * @brief Create a null pointer.
   **/
  inline Rc()
      : pointer_(NULL_PTR(T *)), refCounter_(NULL_PTR(uint32 *)),
        weakCounter_(NULL_PTR(uint32 *)), isNull_(NULL_PTR(bool *)), isWeak_(false) {}
  /**
   * @brief Create a smart pointer from a regular pointer.
   *
   * The owenership of the pointer will be moved to the smart pointer.
   **/
  inline Rc(T *p)
      : pointer_(p), refCounter_(p == NULL_PTR(T *) ? NULL_PTR(uint32 *) : new uint32(1)),
        weakCounter_(p == NULL_PTR(T *) ? NULL_PTR(uint32 *) : new uint32(0)),
        isNull_(p == NULL_PTR(T *) ? NULL_PTR(bool *) : new bool(false)),
        isWeak_(false) {}
  /**
   * @brief Create a smart pointer from a value.
   **/
  inline Rc(const T &v) {
    pointer_ = new T(v);
    refCounter_ = new uint32(1);
    isWeak_ = false;
    weakCounter_ = new uint32(0);
    isNull_ = new bool(false);
  }
  /**
   * @brief Copy constructor, if not null will increase the counter.
   **/
  inline Rc(const Rc &other)
      : pointer_(other.pointer_), refCounter_(other.refCounter_),
        weakCounter_(other.weakCounter_), isNull_(other.isNull_),
        isWeak_(other.isWeak_) {
    if (other.refCounter_ != NULL_PTR(uint32 *) && !other.isWeak_ && this != &other) {
      *other.refCounter_ = *other.refCounter_ + 1;
    } else if (other.isWeak_ && !other.isNull()) {
      *other.weakCounter_ = *other.weakCounter_ + 1;
    }
  }
  /**
   * @brief dereferenciate pointer.
   **/
  inline ~Rc() { dereferenciate(); }

  /**
   * @brief check if pointer is null.
   **/
  inline bool isNull() const { return isNull_ == NULL_PTR(bool *) || *isNull_; }
  /**
   * @brief check if pointer is weak.
   **/
  inline bool isWeak() const { return isWeak_; }

  /**
   * @brief bool operator returns if the pointer is not null.
   **/
  inline operator bool() const { return !isNull(); }

  /**
   * @brief assign operator with a value of T will change the value of the
   *pointer if possible.
   **/
  inline Rc &operator=(const T other) {
    if (!isNull()) {
      *pointer_ = other;
    } else {
      dereferenciate();
      pointer_ = new T(other);
      refCounter_ = new uint32(1);
      isWeak_ = false;
      weakCounter_ = new uint32(0);
      isNull_ = new bool(false);
    }
    return *this;
  }

  /**
   * @brief assign operator with another smart pointer will dereferenciate the
   *current ptr and referenciate the other.
   **/
  inline Rc &operator=(const Rc &other) {
    if (other.pointer_ == pointer_) {
      return *this;
    }
    dereferenciate();
    if (other.isNull()) {
      pointer_ = NULL_PTR(T *);
      refCounter_ = NULL_PTR(uint32 *);
      weakCounter_ = NULL_PTR(uint32 *);
      isNull_ = NULL_PTR(bool *);
    } else {
      pointer_ = other.pointer_;
      refCounter_ = other.refCounter_;
      weakCounter_ = other.weakCounter_;
      isNull_ = other.isNull_;
      isWeak_ = other.isWeak_;
      if (other.isWeak_) {
        *weakCounter_ = *weakCounter_ + 1;
      } else {
        *refCounter_ = *refCounter_ + 1;
      }
    }
    return *this;
  }

  /**
   * @brief weak assign operator with another smart pointer will dereferenciate
   *the current ptr and create a weak reference to the other.
   **/
  inline Rc &operator%=(const Rc &other) {
    if (other.pointer_ == pointer_) {
      return *this;
    }
    dereferenciate();
    pointer_ = other.pointer_;
    refCounter_ = other.refCounter_;
    isNull_ = other.isNull_;
    weakCounter_ = other.weakCounter_;
    isWeak_ = true;
    if (!other.isNull()) {
      *weakCounter_ = *weakCounter_ + 1;
    }
    return *this;
  }

  /**
   * @brief check if the internal pointer is the same of the other pointer
   **/
  inline bool same(const T *other) const { return other == pointer_; }
  /**
   * @brief check if the internal pointer is the same of the other pointer
   **/
  inline bool same(const Rc &other) const { return other.pointer_ == pointer_; }

  /**
   * @brief Pointer equality operator
   **/
  inline bool operator==(const T *other) const { return pointer_ == other; }
  /**
   * @brief Pointer disequality operator
   **/
  inline bool operator!=(const T *other) const { return other != pointer_; }

  /**
   * @brief Pointer equality operator
   **/
  inline bool operator==(const Rc &other) const {
    return other.pointer_ == pointer_;
  }
  /**
   * @brief Pointer disequality operator
   **/
  inline bool operator!=(const Rc &other) const {
    return other.pointer_ != pointer_;
  }

  /**
   * @brief Value equality operator
   **/
  inline bool operator==(const T &other) const {
    if (isNull()) {
      return false;
    }
    return *pointer_ == other;
  }
  /**
   * @brief Value disequality operator
   **/
  inline bool operator!=(const T &other) const {
    if (isNull()) {
      return true;
    }
    return *pointer_ != other;
  }

  inline bool operator!() const { return isNull(); }

  /**
   * @brief Create a weak pointer from an hard (or weak) pointer.
   **/
  inline Rc<T> operator~() const {
    if (isNull()) {
      return Rc<T>();
    }
    Rc<T> res;
    res.pointer_ = pointer_;
    res.refCounter_ = refCounter_;
    res.isWeak_ = true;
    res.weakCounter_ = weakCounter_;
    res.isNull_ = isNull_;
    *weakCounter_ = *weakCounter_ + 1;
    return res;
  }

  /**
   * @brief Create a hard pointer from an hard (or weak) pointer.
   **/
  inline Rc<T> operator+() const {
    if (isNull()) {
      return Rc<T>();
    }
    Rc<T> res;
    res.pointer_ = pointer_;
    res.refCounter_ = refCounter_;
    res.isWeak_ = false;
    res.weakCounter_ = weakCounter_;
    res.isNull_ = isNull_;
    *refCounter_ = *refCounter_ + 1;
    return res;
  }

  /**
   * @brief access internal methods/field of the pointer.
   **/
  inline T *operator->() const {
    assert(!isNull());
    return pointer_;
  }
  /**
   * @brief access pointer value.
   **/
  inline T &operator*() const {
    assert(!isNull());
    return *pointer_;
  }

  /**
   * @brief Manually nullify pointer.
   **/
  inline void del() {
    if (!isNull()) {
      dereferenciate();
      pointer_ = NULL_PTR(T *);
      refCounter_ = NULL_PTR(uint32 *);
      weakCounter_ = NULL_PTR(uint32 *);
      isNull_ = NULL_PTR(bool *);
    }
  }

  /**
   * @brief access pointer value.
   **/
  inline T &val() const {
    assert(!isNull());
    return *pointer_;
  }

  /**
   * @brief get reference count.
   **/
  inline uint32 referencesCount() const {
    if (isNull()) {
      return 0u;
    }
    return *refCounter_;
  }

  /**
   * @brief get number of weak references.
   **/
  inline uint32 weakReferencesCount() const {
    if (isNull()) {
      return 0u;
    }
    return *weakCounter_;
  }
};

} // namespace MARTe

#endif
