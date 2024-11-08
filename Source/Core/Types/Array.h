#ifndef _ARRAY_H__
#define _ARRAY_H__

#include "Error.h"
#include "Option.h"
#include "Result.h"

#include <assert.h>

namespace MARTe {
/**
  @brief dynamic array or type T

  This class permits to manage a dynanic array without
  using a linked list but instead using a resizible
  buffer.

  If the array grows bigger than the buffer, a new
  bigger buffer will be created and data copied.

  @param T type of the contained value
**/
template <typename T> class Array {
public:
  /**
    @brief empty array with an initialized empty buffer.
    @param init_size intial buffer size
  **/
  inline Array(uint32 init_size = step) : size_(0u), buffsize_(init_size) {
    arr_ = new T[buffsize_];
  }

  /**
    @brief copy constructor
  **/
  inline Array(const Array &other) {
    buffsize_ = other.size_;
    size_ = other.size_;
    arr_ = new T[size_];
    copy(arr_, other.arr_, size_);
  }

  /**
    @brief create a dynamic array from a standard array
    @param array array to be copied
    @param size size of the array
  **/
  inline Array(const T array[], const uint32 size)
      : size_(size), buffsize_(size + step) {
    arr_ = new T[buffsize_];
    copy(arr_, array, size);
  }

  /**
    @brief destructor, clean the buffer
  **/
  inline ~Array() { delete[] arr_; }

  /**
    @brief add item to the array
  **/
  inline void add(T item) {
    if (size_ + 1u > buffsize_) {
      T *arr = new T[buffsize_ + step];
      copy(arr, arr_, buffsize_);
      delete[] arr_;
      arr_ = arr;
      buffsize_ += step;
    }
    arr_[size_++] = item;
  }

  /**
    @brief copy an standard array inside the dyn array
    @param arr standard array to be copied
    @param size size of the input array
  **/
  inline void set(const T *arr, const uint32 size) {
    size_ = size;
    if (size > buffsize_) {
      buffsize_ = size + step;
      delete[] arr_;
      arr_ = new T[buffsize_];
    }
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
  **/
  inline T &operator[](int32 i) const {
    int j = i + (i >= 0 ? 0 : static_cast<int>(size_));
    assert(j >= 0 && j < static_cast<int>(size_));
    return arr_[j];
  }

  /**
    @brief get the elements at i-th position

    @param i index of the element to be extracted
    @return element if the index is valid
    @return an error (E_OUT_OF_BOUND) if index is
      not valid.
  **/
  inline T &operator[](uint32 i) const {
    assert(i < size_);
    return arr_[i];
  }

  /**
    @brief equality operator for two arrays
    @param other second array
    @return true if all elements match
  **/
  inline bool operator==(const Array &other) const {
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
  inline bool operator!=(const Array &other) const {
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
  inline Array &operator=(const Array &other) {
    if (other.size_ > buffsize_) {
      T *arr = new T[other.size_ + step];
      delete[] arr_;
      arr_ = arr;
      buffsize_ = other.size_ + step;
    }
    size_ = other.size_;
    copy(arr_, other.arr_, other.size_);
    return *this;
  }

  /**
    @brief add element  operator
    @param other element to be added
    @return itself
  **/
  inline Array &operator+=(const T other) {
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
  inline uint32 mem_size() const { return buffsize_; }

  /**
    @brief reset the size counter
  **/
  inline void clear() { size_ = 0; }
  /**
    @brief reduce the size of the buffer to match
    the number of data
  **/
  inline void reduce() {
    uint32 nsize = ((size_ / step) + 1) * step;
    T *arr = new T[nsize];
    copy(arr, arr_, size_);
    delete[] arr_;
    arr_ = arr;
    buffsize_ = nsize;
  }

private:
  inline static void copy(T *res, const T *src, uint32 size) {
    // memcpy(res, src, size * sizeof(T));
    for (uint32 i = 0u; i < size; i++) {
      res[i] = src[i]; // To work with non trivial types
    }
  }
  static const uint32 step = 16;
  T *arr_;
  uint32 size_;
  uint32 buffsize_;
};
} // namespace MARTe

#endif
