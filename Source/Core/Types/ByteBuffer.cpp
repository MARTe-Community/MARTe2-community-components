#include "bytebuffer.h"
#include <string.h>

namespace ect {

ByteBuffer::ByteBuffer(u32 byteSize, u32 size) {
  create(byteSize, size);
  head_ = buffer_;
  tail_ = buffer_;
  increment_next_ = false;
}

ByteBuffer::ByteBuffer(const ByteBuffer &other) { copy(other); }

ByteBuffer::~ByteBuffer() { delete[] buffer_; }

void ByteBuffer::write(void *array, u32 size) {
  if (size > 0u) {
    u32 bsz = size*byteSize_;
    memcpy(head_, reinterpret_cast<u8*>(array), bsz); 
    increment(&head_, size);
    if (head_ == tail_) {
      increment(&tail_, size);
    } else if (increment_next_) {
      increment_next_ = false;
      increment(&tail_, size);
    }
  }
}

void* ByteBuffer::read() {
  void *ret = tail_;
  if (tail_ != head_-byteSize_) {
    increment(&tail_);
  } else if (increment_next_) {
    increment_next_ = false;
    increment(&tail_);
  } else {
    increment_next_ = true;
  }
  return ret;
}

void* ByteBuffer::read(u32 i) {
  u8 *ret = head_ - (i+1)*byteSize_;
  if (ret < buffer_) {
    ret = end_ - (buffer_-ret) + byteSize_;
  }
  return reinterpret_cast<void *>(ret);
}

bool ByteBuffer::readLast(u8* array, u32 num) {
  if (num==0) {
    return false;
  }
  u32 copy_size = num*byteSize_;
  u32 head_len = head_-buffer_;
  if (copy_size <= head_len) {
    u32 delta = head_len - copy_size;
    memcpy(array, buffer_ + delta, copy_size);
  } else {
    u32 delta = copy_size - head_len;
    memcpy(array, end_ - delta + byteSize_, delta);
    memcpy(array, buffer_, head_len);
  }
  return true;
}

u32 ByteBuffer::len() {
  u32 ret;
  u32 delta = head_ - tail_;
  if (delta == 0) {
    ret = delta/byteSize_;
  } else {
    ret = (end_-tail_) + (head_-buffer_);
    ret = ret/byteSize_;
  }
  return ret;
}

u32 ByteBuffer::size() const { return size_; }

u32 ByteBuffer::byteSize() const { return byteSize_; }

u32 ByteBuffer::memorySize() const { return memSize_; }

ByteBuffer &ByteBuffer::operator=(const ByteBuffer &other) {
  delete[] buffer_;
  copy(other);
  return *this;
}

bool ByteBuffer::operator==(const ByteBuffer &other) const {
  if (memSize_ != other.memSize_ ||
      head_ == tail_ ||
      other.head_ == other.tail_ ) {
    return false;
  }
  return memcmp(buffer_, other.buffer_, memSize_)==0;
}

bool ByteBuffer::operator!=(const ByteBuffer &other) const { return !(*this == other); }

void ByteBuffer::increment(u8 **pos, u32 step) {
  u32 bsz = step==0 ? 1u : step;
  bsz *= byteSize_;
  *pos += bsz;
  while (*pos > end_) {
    *pos = buffer_ + (*pos - end_ - byteSize_);
  }
}

void ByteBuffer::create(const u32 byteSize, const u32 size) {
  byteSize_ = byteSize;
  size_ = size;
  memSize_ = byteSize_*size_;
  buffer_ = new u8[memSize_]();
  end_ = buffer_ + memSize_ - byteSize_;
}

void ByteBuffer::copy(const ByteBuffer &other) {
  create(other.byteSize_, other.size_);
  memcpy(buffer_, other.buffer_, memSize_);
  increment_next_ = other.increment_next_;
  head_ = buffer_ + (other.head_ - other.buffer_);
  tail_ = buffer_ + (other.tail_ - other.buffer_);
}

} // namespace ect
