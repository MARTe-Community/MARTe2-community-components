#include "Str.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define INIT_SIZE 16
#define STEP 16
namespace MARTe {

Str::Str() : size_(INIT_SIZE), len_(0) {
  mem_ = new char[size_];
  mem_[0] = 0;
}

Str::Str(const char *str) : size_(INIT_SIZE) {
  len_ = strlen(str);
  if (len_ + 1 > size_) {
    size_ = len_ + 1;
  }
  mem_ = new char[size_];
  if (!memccpy(mem_, str, 0, len_)) {
    mem_[len_] = 0;
  }
}

Str::Str(const Str &other) : size_(other.size_), len_(other.len_) {
  if (other.size_ > 0) {
    mem_ = new char[size_];
    if (!memccpy(mem_, other.mem_, 0, other.len_)) {
      mem_[other.len_] = 0;
    }
  } else {
    mem_ = NULL_PTR(char *);
  }
}

Str::~Str() {
  if (mem_ != NULL_PTR(char *)) {
    delete[] mem_;
    mem_ = NULL_PTR(char *);
  }
}

uint32 Str::len() const { return len_; }

const char *Str::cstr() const { return (const char *)mem_; }

void Str::clear() {
  mem_[0] = 0;
  len_ = 0;
}

void Str::extend(uint32 new_l) {
  if (new_l >= size_) {
    uint32 new_size = new_l + 1;
    char *buff = new char[new_size];
    if (!memccpy(buff, mem_, 0, len_)) {
      buff[len_] = 0;
    }
    delete[] mem_;
    mem_ = buff;
    size_ = new_size;
  }
}

Str Str::operator+(const Str &other) const {
  size_t l = other.len_ + len_ + 1;
  Str s;
  s.extend(l);
  s.len_ = other.len_ + len_;
  char *ptr = (char *)memccpy(s.mem_, mem_, 0, len_);
  if (ptr == NULL_PTR(char *)) {
    ptr = s.mem_ + len_;
  } else {
    ptr = ptr - 1;
  }

  if (!memccpy(ptr, other.mem_, 0, other.len_)) {
    s.mem_[l] = 0;
  };
  return s;
}

Str Str::operator+(const char *other) const {
  size_t l = strlen(other) + len_ + 1;
  Str s;
  s.extend(l);

  s.len_ = l - 1;
  char *ptr = (char *)memccpy(s.mem_, mem_, 0, size_);
  assert(ptr != NULL_PTR(char *));
  if (!memccpy(ptr - 1, other, 0, l)) {
    s.mem_[l] = 0;
  };

  return s;
}

Str Str::operator+(const char &ch) const {
  size_t l = len_ + 2;
  Str s;
  s.extend(l);

  s.len_ = l - 1;
  char *ptr = (char *)memccpy(s.mem_, mem_, 0, size_);
  assert(ptr != NULL_PTR(char *));
  s.mem_[len_] = ch;
  s.mem_[len_ + 1] = 0;
  return s;
}

Str &Str::operator=(const Str &other) {
  if (other.len_ >= size_) {
    extend(other.len());
  }
  if (!memccpy(mem_, other.mem_, 0, other.len())) {
    mem_[other.len()] = 0;
  }
  len_ = other.len();
  return *this;
}

Str &Str::operator+=(const char other) {
  if (other != 0) {
    extend(len_ + 1);
    mem_[len_++] = other;
    mem_[len_] = 0;
  }

  return *this;
}

bool Str::operator==(const Str &other) const {
  if (len_ != other.len_) {
    return false;
  }
  return strcmp(mem_, other.mem_) == 0;
}

bool Str::operator==(const char *other) const {
  if (other == NULL_PTR(const char *)) {
    return len_ == 0;
  }
  uint32 l = strlen(other);
  return l == len_ && strcmp(mem_, other) == 0;
}

bool Str::operator!=(const Str &other) const {
  if (len_ != other.len_) {
    return true;
  }
  return strcmp(mem_, other.mem_) != 0;
}
bool Str::operator!=(const char *other) const {
  if (other == NULL_PTR(const char *)) {
    return len_ != 0;
  }
  uint32 l = strlen(other);
  return l != len_ || strcmp(mem_, other) != 0;
}

bool Str::operator>(const Str &other) const {
  if (len_ == 0 && other.len_ != 0) {
    return false;
  }
  if (other.len_ == 0) {
    return len_ != 0;
  }
  uint32 min = len_ < other.len_ ? len_ : other.len_;
  for (uint32 i = 0u; i < min; i++) {
    if (mem_[i] > other.mem_[i]) {
      return true;
    } else if (mem_[i] < other.mem_[i]) {
      return false;
    }
  }
  return len_ > other.len_;
}

bool Str::operator<(const Str &other) const {
  if (len_ == 0 && other.len_ != 0) {
    return true;
  }
  if (other.len_ == 0) {
    return false;
  }
  uint32 min = len_ < other.len_ ? len_ : other.len_;
  for (uint32 i = 0u; i < min; i++) {
    if (mem_[i] > other.mem_[i]) {
      return false;
    } else if (mem_[i] < other.mem_[i]) {
      return true;
    }
  }
  return len_ < other.len_;
}

char Str::operator[](int i) const {
  int j = i >= 0 ? i : (len_ + i);
  assert(j >= 0 && (uint32)j < len_);
  return mem_[j];
}

Str &Str::set(const int &i, const char &c) {
  int j = i >= 0 ? i : (len_ + i);
  mem_[j] = c;
  return *this;
}

Str Str::substr(const int32 end) const {
  int32 len = end <= 0 ? ((int32)len_ + end) : end;
  if (len < 0)
    return "";
  if ((uint32)len >= len_)
    return Str(*this);
  Str a;
  a.extend((uint32)len);
  a.len_ = (uint32)len;
  if (!memccpy(a.mem_, mem_, 0, a.len_)) {
    a.mem_[a.len_] = 0;
  }
  return a;
}

Str Str::substr(const int32 start, const int32 end) const {
  int32 i0, ie;
  i0 = start < 0 ? ((int32)len_ + start) : start;
  ie = end <= 0 ? ((int32)len_ + end) : end;

  if (ie > (int32)len_) {
    ie = len_;
  }

  if (i0 >= (int32)len_ || ie <= i0 || ie < 0 || i0 < 0) {
    return Str();
  }
  uint32 len = ie - i0;
  Str a;
  a.extend(len);
  a.len_ = len;
  if (!memccpy(a.mem_, &mem_[start], 0, len)) {
    a.mem_[len] = 0;
  }
  return a;
}

int32 Str::find(Str str, int32 start) const {
  if (start + str.len() > len())
    return -1;
  uint32 i = 0;
  uint32 m = 0;
  for (uint32 n = start; n < len_; n++) {
    if (mem_[n] == str.mem_[m]) {
      if (m == 0) {
        i = n;
      }
      if (++m == str.len()) {
        return i;
      }
    } else {
      m = 0;
    }
  }
  return -1;
}

uint32 Str::hash() const {
  uint32 hash_ = 0;
  if (mem_ != NULL_PTR(char *)) {
    for (uint32 i = 0; i < len_; i++) {
      hash_ += mem_[i];
      hash_ += (hash_ << 10);
      hash_ ^= (hash_ >> 6);
    }
    hash_ += (hash_ << 3);
    hash_ ^= (hash_ >> 11);
    hash_ += (hash_ << 15);
  }
  return hash_;
}

} // namespace MARTe
