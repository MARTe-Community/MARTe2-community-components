#ifndef _EC_SET_H__
#define _EC_SET_H__

#include "basetypes.h"
#include "error.h"
#include "optional.h"
#include "result.h"

namespace ect {
/**
  @brief set or type T

  This class implements an set of
  non-duplicate items, resized every time
  an item is added/removed

  @param T type of the contained value
**/
template <typename T> class Set {
public:
  /**
    @brief Empty set with one element.
  **/
  inline Set() : size_(0u) {
    set_ = new T[1u];
  }

  /**
    @brief Copy constructor
    @param[in] other set to copy
  **/
  inline Set(const Set &other) {
    size_ = other.size_;
    set_ = new T[size_];
    copy(set_, other.set_, size_);
  }

  /**
    @brief Create set from array
    @details Discard duplicated items
    @param[in] array array to be copied
    @param[in] size array size
  **/
  inline Set(const T *array, const u32 size) : size_(0u) {
    set_ = new T[1u];
    for(u32 i=0u; i<size; i++) {
      add(array[i]);
    }
  }

  /**
    @brief Destructor.
    @details Deallocate set.
  **/
  inline ~Set() { delete[] set_; }

  /**
    @brief Add item to the set.
    @details Add item only if not already present
    @param[in] item item to addv
    @return true if added
  **/
  inline bool add(T item) {
    if (contains(item) == -1) {
      resize();
      set_[size_-1u] = item;
      return true;
    }
    return false;
  }

  /**
    @brief Add array to the set.
    @details Add elements only if not already present
    @param[in] items array to add
    @param[in] size array size
    @return true if at least one element is added
  **/
  inline bool add(const T *items, const u32 size) {
    u32 init_size = size_;
    for (u32 i=0u; i<size; i++) {
      add(items[i]);
    }
    return (init_size<size_);
  }

  /**
    @brief Pop lastitem from set
    @param[in] i index to pop
    @return true if popped
  **/
  inline bool pop() {
    if (size_ > 0) {
      pop_(size_-1);
      return true;
    }
    return false;
  }

  /**
    @brief Pop i-th item from set
    @param[in] i index to pop
    @return true if popped
  **/
  inline bool pop(u32 i) {
    if (i < size_) {
      pop_(i);
      return true;
    }
    return false;
  }

  /**
    @brief Remove item from set
    @details Remove item if present in set
    @param[in] item to pop
    @return true if removed
  **/
  inline i32 remove(T item) {
    for (u32 i=0u; i<size_; i++) {
      if (set_[i] == item) {
        pop_(i);
        return (i32) i;
      }
    }
    return -1;
  }

  /**
    @brief Remove array from set
    @details Remove elements if present in set
    @param[in] items array to remove
    @param[in] size array size
    @return true if at least one element is removed
  **/
  inline bool remove(const T *items, const u32 size) {
    u32 init_size = size_;
    for (u32 i=0u; i<size; i++) {
      remove(items[i]);
    }
    return (init_size>size_);
  }

  /**
    @brief Check if item is in set
    @param[in] item item to check
    @return index of element in set if found, -1 otherwise
  **/
  inline i32 contains(T item) {
    i32 sz = (i32) size_;
    for (i32 i=0u; i<sz; i++) {
      if (set_[i] == item) {
        return i;
      }
    }
    return -1;
  }

  /**
    @brief Check if array is in set
    @param[in] items array to check
    @param[in] size array size
    @return true all elements are in set
  **/
  inline bool contains(T *items, const u32 size) {
    for (u32 i=0u; i<size; i++) {
      if (contains(items[i])==-1) {
        return false;
      }
    }
    return true;
  }

  /**
    @brief Get set size
  **/
  inline u32 size() const { return size_; }

  /**
    @brief Return the elements at i-th position in set.
    @details Return first element in case of out of bounds.
    @param[in] i element index
  **/
  inline T &operator[](u32 i) const {
    if (i < size_) {
      return set_[i];
    }
    throw ERROR(E_OUT_OF_BOUNDS);
  }

  /**
    @brief Equality operator for two sets
    @param other second set
    @return true if all elements match
  **/
  inline bool operator==(const Set &other) const {
    if (other.size_ != size_) {
      return false;
    }
    for (u32 i=0; i<size_; i++) {
      if (set_[i] != other.set_[i]) {
        return false;
      }
    }
    return true;
  }

  /**
    @brief Disequality operator for two sets
    @param other second set
    @return true if sizes are different or at least one element mismatch
  **/
  inline bool operator!=(const Set &other) const {
    if (other.size_ != size_) {
      return true;
    }
    for (u32 i=0; i<size_; i++) {
      if (set_[i] != other.set_[i]) {
        return true;
      }
    }
    return false;
  }

  /**
    @brief Equality function for array
    @param arr array to compare
    @param size array size
    @return true if all elements match
  **/
  inline bool equals(const T *arr, const u32 size) {
    if (size != size_) {
      return false;
    }
    for (u32 i=0; i<size_; i++) {
      if (set_[i] != arr[i]) {
        return false;
      }
    }
    return true;
  }

private:
  /**
    @brief Copy source set into target set
    @param[in] trg target set
    @param[in] src source set
    @param[in] size_ set size
  **/
  inline static void copy(T *trg, const T *src, u32 size) {
    for (u32 i=0u; i<size; i++) {
      trg[i] = src[i];
    }
  }

  /**
    @brief Resize set
    @details Increment dimension of 1 unit.
    @param[in] dim resize dimension
    @param[in] add flag for increasing/deacrising size
  **/
  inline void resize(u32 dim=1u, bool add=true) {
    if (size_ == 0u) {
      delete[] set_;
      size_ = dim;
      set_ = new T[size_];
    } else {
      T *arr;
      u32 copy_size;
      u32 new_size;
      if (add) {
        copy_size = size_;
        new_size = size_ + dim;
      } else {
        copy_size = size_ - dim;
        new_size = copy_size;
      }
      arr = new T[new_size];
      copy(arr, set_, copy_size);
      delete[] set_;
      set_ = arr;
      size_ = new_size;
    }
  }

  /**
    @brief Pop i-th item from set without check
    @param[in] i index to pop
  **/
  inline void pop_(u32 i) {
    for (u32 j=i; j<size_-1; j++) {
      set_[j] = set_[j+1];
    }
    resize(1, false);
  }
  
  /**
    Array of type T
  **/
  T *set_;
  
  /**
    Array size
  **/
  u32 size_;
};

}

#endif