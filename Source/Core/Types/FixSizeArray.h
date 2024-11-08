#ifndef FIX_ARRAY_H__
#define FIX_ARRAY_H__
#include "Error.h"
#include "Option.h"
#include "Result.h"
#include <assert.h>

namespace MARTe {
/**
  @brief fix array or type T

  This class permits to manage a dynanic array without
  using a linked list but instead using a resizible
  buffer.

  If the array grows bigger than the buffer, a new
  bigger buffer will be created and data copied.

  @param T type of the contained value
**/
template <typename T, uint32 size> class FixArray {
public:
  /**
    @brief empty array with an initialized empty buffer.
    @param init_size intial buffer size
  **/
  inline FixArray() : size_(0u), buffsize_(size) { arr_ = new T[size]; }

  /**
    @brief copy constructor
  **/
  inline FixArray(const FixArray &other) {
    assert(other.buffsize_ == size);
    buffsize_ = size;
    size_ = other.size_;
    arr_ = new T[size];
    copy(arr_, other.arr_, size_);
  }

  /**
    @brief copy constructor
  **/
  template <uint32 asize> inline FixArray(const FixArray<T, asize> &other) {
    assert(other.size_ <= size);
    buffsize_ = size;
    size_ = other.size_;
    arr_ = new T[size];
    copy(arr_, other.arr_, size_);
  }

  /**
    @brief create a dynamic array from a standard array
    @param array array to be copied
    @param size size of the array
  **/
  inline FixArray(const T array[], const uint32 arr_size)
      : size_(arr_size), buffsize_(size) {
    assert(arr_size <= size);
    arr_ = new T[size];
    copy(arr_, array, arr_size);
  }

  /**
    @brief destructor, clean the buffer
  **/
  inline ~FixArray() { delete[] arr_; }

  /**
    @brief add item to the array
  **/
  inline void add(T item) {
    assert(size_ + 1 < size);
    arr_[size_++] = item;
  }

  /**
    @brief copy an standard array inside the dyn array
    @param arr standard array to be copied
    @param size size of the input array
  **/
  inline void set(const T *arr, const uint32 a_size) {
    assert(a_size <= size);
    size_ = a_size;
    copy(arr_, arr, size);
  }

  /**
    @brief remove the i-th element of the array
    @param i index of the element to remove
    @return true if the removal was succesffult
    @return false if not
  **/
  inline bool remove(int32 i) {
    int j = i + (i >= 0 ? 0 : (int32)size_);
    if (j >= 0 && j < (int32)size_) {
      copy(arr_ + j, arr_ + j + 1, (int32)size_ - (j + 1));
      size_--;
      return true;
    }
    return false;
  }

  /**
    @brief find the index of an item
    @param item item to be find
    @param from start index for the research
    @return index (uint32) of the item if found
    @return empty optional if not found
  **/
  inline Option<uint32> find(T item, uint32 from = 0) {
    for (uint32 i = from; i < size_; i++) {
      if (arr_[i] == item)
        return i;
    }
    return Option<uint32>();
  }

  /**
    @brief check if an element is inside the array or not
    @param item element to be checked
    @param from starting index for the research
    @return true if the element is inside the array
    @return false if not
  **/
  inline bool contains(T item, uint32 from = 0) {
    return !find(item, from).empty();
  }

  /**
    @brief get the elements at i-th position

    if the index is negative will decount form
    the last element of the array.
    @param i index of the element to be extracted
    @return element if the index is valid
    @return an error (E_OUT_OF_BOUND) if index is
      not valid.
  **/
  inline Result<T> at(int32 i) {
    int j = i + (i >= 0 ? 0 : (int32)size_);
    if (j >= 0 && j < (int32)size_)
      return arr_[j];
    return ERROR(E_OUT_OF_BOUNDS);
  }

  /**
    @brief get the elements at i-th position

    if the index is negative will decount form
    the last element of the array.
    @param i index of the element to be extracted
    @return element if the index is valid
    @throw an error (E_OUT_OF_BOUND) if index is
      not valid.
  **/
  inline T &operator[](int32 i) const {
    int j = i + (i >= 0 ? 0 : static_cast<int>(size_));
    if (j >= 0 && j < static_cast<int>(size_))
      return arr_[j];
    throw ERROR(E_OUT_OF_BOUNDS);
  }

  /**
    @brief get the elements at i-th position

    @param i index of the element to be extracted
    @return element if the index is valid
    @return an error (E_OUT_OF_BOUND) if index is
      not valid.
  **/
  inline T &operator[](uint32 i) const {
    if (i < size_)
      return arr_[i];
    throw ERROR(E_OUT_OF_BOUNDS);
  }

  /**
    @brief equality operator for two arrays
    @param other second array
    @return true if all elements match
  **/
  template <uint32 asize>
  inline bool operator==(const FixArray<T, asize> &other) const {
    if (other.size_ != size_) {
      return false;
    }
    for (uint32 i = 0; i < size_; i++) {
      if (arr_[i] != other.arr_[i]) {
        return false;
      }
    }
    return true;
  }

  /**
    @brief disequality operator for two arrays
    @param other second array
    @return true if any of the elements or length does not match
  **/
  template <uint32 asize>
  inline bool operator!=(const FixArray<T, asize> &other) const {
    if (other.size_ != size_) {
      return true;
    }
    for (uint32 i = 0; i < size_; i++) {
      if (arr_[i] != other.arr_[i]) {
        return true;
      }
    }
    return false;
  }

  /**
    @brief assign  operator for two arrays
    @param other second array
    @return itself
  **/
  inline FixArray &operator=(const FixArray &other) {
    size_ = other.size_;
    copy(arr_, other.arr_, other.size_);
    return *this;
  }

  /**
    @brief assign  operator for two arrays
    @param other second array
    @return itself
  **/
  template <uint32 asize>
  inline FixArray &operator=(const FixArray<T, asize> &other) {
    assert(other.size_ < size);
    size_ = other.size_;
    copy(arr_, other.arr_, other.size_);
    return *this;
  }

  /**
    @brief add element  operator
    @param other element to be added
    @return itself
  **/
  inline FixArray &operator+=(const T other) {
    this->add(other);
    return *this;
  }

  /**
    @brief length of the array
    @return number of elements inside the array
  **/
  inline uint32 len() const { return size_; }
  /**
    @brief size of the buffer
    @return total number of elements of the buffer
  **/
  inline uint32 mem_size() const { return size; }

  /**
    @brief reset the size counter
  **/
  inline void clear() { size_ = 0; }

private:
  inline static void copy(T *res, const T *src, uint32 asize) {
    // memcpy(res, src, size * sizeof(T));
    for (uint32 i = 0u; i < asize; i++) {
      res[i] = src[i]; // To work with non trivial types
    }
  }
  static const uint32 step = 16;
  T *arr_;
  uint32 size_;
  uint32 buffsize_;
};
} // namespace ect

#endif
