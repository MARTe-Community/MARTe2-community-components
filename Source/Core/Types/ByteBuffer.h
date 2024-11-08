#ifndef _EC_BYTEBUFFER_H__
#define _EC_BYTEBUFFER_H__

#include "basetypes.h"

namespace ect {
/**
  @brief Circular buffer
  @details ByteBuffer that deals directly with memory. Types are not considered
**/
class ByteBuffer {
public:
  /**
    @brief Empty buffer
    @param byteSize byte size (defaults to 1)
    @param size buffer size (defaults to 100)
  **/
  ByteBuffer(u32 byteSize = 1u, u32 size = 100);

  /**
    @brief Copy constructor
    @param[in] other buffer to copy
  **/
  ByteBuffer(const ByteBuffer &other);
  /**
    @brief Destructor
    @details Deallocate internal buffer
  **/
  ~ByteBuffer();

  /**
    @brief Write array in buffer
    @param[in] item array to write
    @param[in] size size in case of an array (defaults to 1)
  **/
  void write(void *array, u32 size=1u);

  /**
    @brief Read item from buffer
  **/
  void* read();

  /**
    @brief Read last i-th added item from buffer
    @param[in] i index of item to read (starting from head backward)
  **/
  void* read(u32 i);

  /**
    @brief Read last num added value
    @param[out] array array to fill
    @param[in] num number of items to read
  **/
  bool readLast(u8* array, u32 num);

  /**
    @brief Get current buffer length
    @return buffer length
  **/
  u32 len();

  /**
    @brief Get buffer size
    @return buffer size
  **/
  u32 size() const;

  /**
    @brief Get byte size
    @return buffer byte size
  **/
  u32 byteSize() const;

  /**
    @brief Get total memory occupied by the buffer
    @return buffer memory size
  **/
  u32 memorySize() const;

  /**
    @brief Assign operator for two buffers
    @param other second array
    @return itself
  **/
  ByteBuffer &operator=(const ByteBuffer &other);

  /**
    @brief equality operator for two bytebuffers
    @param other bytebuffer to be compared
    @return true if the two bytebuffers are equal
  **/
  bool operator==(const ByteBuffer &other) const;

  /**
    @brief inequality operator for two bytebuffers
    @param other bytebuffer to be compared
    @return true if the two bytebuffers are not equal
  **/
  bool operator!=(const ByteBuffer &other) const;

private:
  /**
    @brief Increment pointer inside the buffer
    @param[out] pos pointer to pointer
    @param[out] step increment step (defaults to 1)
  **/
  void increment(u8 **pos, u32 step=1u);
  
  /**
    @brief Create empty buffer
    @param[out] size buffer size
  **/
  void create(const u32 byteSize, const u32 size);
  
  /**
    @brief Copy buffer
    @param[out] other buffer to copy
  **/
  void copy(const ByteBuffer &other);

  /**
    Byte size of sitred variable type
  **/
  u32 byteSize_;

  /**
    ByteBuffer size
  **/
  u32 size_;

  /**
    Byte size of sitred variable type
  **/
  u32 memSize_;

  /**
    ByteBuffer
  **/
  u8 *buffer_;

  /**
    Pointer to last element added
  **/
  u8 *head_;

  /**
    Pointer to last element read
  **/
  u8 *tail_;

  /**
    Pointer to buffer[size_-1]
  **/
  u8 *end_;

  /**
    Flag to keep track of edge case when tail reaches head
  **/
  bool increment_next_;
};

} // namespace ect

#endif
