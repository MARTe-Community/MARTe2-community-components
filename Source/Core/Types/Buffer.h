#ifndef _EC_BUFFER_H__
#define _EC_BUFFER_H__

#include "basetypes.h"
#include <assert.h>

namespace ect {
/**
  @brief Circular buffer
  @param[in] T type of the contained value
**/
template <typename T> class Buffer {
public:
  /**
    @brief Empty buffer
    @param[in] size buffer size (defaults to 100)
  **/
  inline Buffer(u32 size = 100) { create(size); }

  /**
    @brief Copy constructor
    @param[in] other buffer to copy
  **/
  inline Buffer(const Buffer &other) { copy(other); }

  /**
    @brief Constructor
    @details Convert array into circular_ buffer
    @param[in] array array to convert
    @param[in] size array size
  **/
  inline Buffer(const T *other, const u32 size) {
    create(size);
    write(other, size_);
  }

  /**
    @brief Destructor
    @details Deallocate internal buffer
  **/
  inline ~Buffer() { delete[] buffer_; }

  /**
    @brief Write item in buffer
    @param[in] item item to write
  **/
  inline void write(T item) {
    if (circular_ && head_ == tail_) {
      increment(&tail_);
    }
    *head_ = item;
    increment(&head_);
    if (head_ <= tail_) {
      circular_ = true;
    }
  }

  /**
    @brief Write an array of items in buffer
    @param[in] items array of items
    @param[in] size array size
  **/
  inline void write(const T *items, u32 size) {
    for (u32 i = 0u; i < size; i++) {
      write(items[i]);
    }
  }

  /**
    @brief Read item from buffer
    @return item
  **/
  inline T read() {
    T ret = *tail_;
    if (circular_ || (tail_ < head_)) {
      increment(&tail_);
    }
    if (tail_ < head_) {
      circular_ = false;
    }
    return ret;
  }

  /**
    @brief Read last i-th added item from buffer
    @param[in] i item index (from head_ backward)
    @return last i-th added item
  **/
  inline T read(u32 i) {
    assert(i <= size_);
    T *ret = head_ - i - 1;
    if (ret < buffer_) {
      ret = end_ - (buffer_ - ret) + 1;
    }
    return *ret;
  }

  /**
    @brief Read array of items from buffer
    @param[out] array array to fill
    @param[in] num number of items to read
  **/
  inline bool read(T *array, u32 num) {
    bool ok = (len() > 0) && (num > 0);
    for (u32 i = 0u; i < num; i++) {
      array[i] = read();
    }
    return ok;
  }

  /**
    @brief Read last num added value
    @param[out] array array to fill
    @param[in] num number of items to read
  **/
  inline bool readLast(T *array, u32 num) {
    if (num == 0) {
      return false;
    }
    T *pos = head_ - 1;
    for (u32 i = 0u; i < num; i++) {
      array[num - i - 1] = *pos;
      decrement(&pos);
    }
    return true;
  }

  /**
    @brief Return current buffer length
  **/
  inline u32 len() {
    u32 ret;
    if (circular_) {
      ret = (end_ - tail_) + (head_ - buffer_);
    } else {
      ret = head_ - tail_;
    }
    return ret;
  }

  /**
    @brief Return current buffer size
  **/
  inline u32 size() const { return size_; }

  /**
    @brief Assign operator for two buffers
    @param[in] other second array
    @return itself
  **/
  inline Buffer &operator=(const Buffer &other) {
    delete[] buffer_;
    copy(other);
    return *this;
  }

private:
  /**
    @brief Decrement pointer inside the buffer
    @param[out] pos pointer to pointer
  **/
  inline void increment(T **pos) {
    *pos += 1u;
    if (*pos > end_) {
      *pos = buffer_;
    }
  }

  /**
    @brief Increment (decrement) pointer inside the buffer
    @param[out] pos pointer to pointer
  **/
  inline void decrement(T **pos) {
    *pos -= 1u;
    if (*pos < buffer_) {
      *pos = end_;
    }
  }

  /**
    @brief Create empty buffer
    @param[out] size buffer size
  **/
  inline void create(const u32 size) {
    size_ = size;
    buffer_ = new T[size_]();
    head_ = buffer_;
    tail_ = buffer_;
    end_ = buffer_ + size_ - 1;
    circular_ = false;
  }

  /**
    @brief Copy buffer
    @param[out] other buffer to copy
  **/
  inline void copy(const Buffer &other) {
    create(other.size_);
    write(other.buffer_, size_);
    head_ = buffer_ + (other.head_ - other.buffer_);
    tail_ = buffer_ + (other.tail_ - other.buffer_);
    circular_ = other.circular_;
  }

  /**
    Buffer size
  **/
  u32 size_;

  /**
    Buffer
  **/
  T *buffer_;

  /**
    Pointer to last element added
  **/
  T *head_;

  /**
    Pointer to last element read
  **/
  T *tail_;

  /**
    Pointer to buffer[size_-1]
  **/
  T *end_;

  /**
    Flag when head is behind tail
  **/
  bool circular_;
};

} // namespace ect

#endif
