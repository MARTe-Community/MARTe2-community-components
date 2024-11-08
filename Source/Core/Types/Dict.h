#ifndef __DICT_H__
#define __DICT_H__

#include "basetypes.h"
#include "optional.h"
#include "str.h"

namespace ect {

template <typename V, typename K = ect::Str, u32 size = 256> class dict {
public:
  class item_t {
  public:
    inline item_t(const K &k) : key_(k), value_() {}
    inline item_t(const K &k, const V &v) : key_(k), value_(v) {}
    inline item_t(const item_t &other) {
      key_ = other.key_;
      value_ = other.value_;
    }
    inline K key() const { return key_; }
    inline V &value() { return value_.val(); }
    inline operator V &() { return value_.val(); }
    inline item_t &operator=(const V &other) {
      value_ = other;
      return *this;
    }
    inline item_t &operator=(const item_t &other) {
      value_ = other.value_;
      key_ = other.key_;
      return *this;
    }

  private:
    K key_;
    ect::Optional<V> value_;
  };

  inline dict() : len_(0) {
    for (u32 i = 0; i < size; i++) {
      values[i] = null(iter_t *);
    }
  };
  inline dict(const dict &other) : len_(other.len_) {
    for (u32 i = 0; i < size; i++) {
      values[i] = iter_t::copy(other.values[i]);
    }
  }
  inline ~dict() {
    for (u32 i = 0; i < size; i++) {
      if (values[i] != null(iter_t *)) {
        delete values[i];
        values[i] = null(iter_t *);
      }
    }
  }

  inline dict &operator=(const dict &other) {
    for (u32 i = 0; i < size; i++) {
      if (values[i] != null(iter_t *)) {
        delete values[i];
        values[i] = null(iter_t *);
      }
      if (values[i] != null(iter_t *)) {
        values[i] = iter_t::copy(other.values[i]);
      }
    }
    len_ = other.len_;
  }

  inline item_t &operator[](const K &k) {
    u32 i = k.hash() % size;
    iter_t *a = values[i];
    if (a != null(iter_t *)) {
      bool added = false;
      a = a->find_or_add(k, added);
      len_ += added;
      return a->item();
    }
    a = new iter_t(k);
    values[i] = a;
    len_++;
    return a->item();
  }

  inline bool contains(const K &k) {
    iter_t *a = values[k.hash() % size];
    return a != null(iter_t *) && a->contains(k);
  }

  inline Optional<V> get(const K &k) {
    iter_t *a = values[k.hash() % size];
    if (a == null(iter_t *)) {
      return Optional<V>();
    }
    Optional<item_t> item = a->get(k);
    if (item.empty()) {
      return Optional<V>();
    }
    return item->value();
  }

  inline void set(const K k, const V &v) {
    u32 i = k.hash() % size;
    iter_t *a = values[i];
    if (a != null(iter_t *)) {
      bool added = false;
      a->set(k, v, added);
      if (added) {
        len_++;
      }
    } else {
      values[i] = new iter_t(k, v);
      len_++;
    }
  }

  inline u32 len() const { return len_; }

private:
  class iter_t {
  public:
    static iter_t *copy(const iter_t *it) {
      iter_t *c = new iter_t(it->item_);
      if (it->next_ != null(iter_t *)) {
        c->next_ = copy(it->next_);
      }
      return c;
    }
    iter_t(const item_t &item) : item_(item), next_(null(iter_t *)) {}
    iter_t(const K &k, const V &v) : item_(k, v), next_(null(iter_t *)) {}
    iter_t(const K &k) : item_(k), next_(null(iter_t *)) {}
    iter_t(const iter_t &other) : item_(other.item_), next_(other.next_) {
      other.next_ = null(iter_t *);
    }

    ~iter_t() {
      if (next_ != null(iter_t *)) {
        delete next_;
      }
      next_ = null(iter_t *);
    }

    item_t &item() { return item_; }
    iter_t *next() { return next_; }
    ect::Optional<item_t> get(const K &key) {
      if (key == item_.key()) {
        return item_;
      }
      if (next_ != null(iter_t *)) {
        return next_->get(key);
      }
      return ect::Optional<item_t>();
    }

    iter_t *set(const K &k, const V &v, bool &added) {
      if (k == item_.key()) {
        item_ = v;
        added = false;
        return this;
      }
      if (next_ != null(iter_t *)) {
        return next_->set(k, v, added);
      }
      iter_t *neo = new iter_t(k, v);
      next_ = neo;
      added = true;
      return neo;
    }

    iter_t *find_or_add(const K &k, bool &added) {
      if (k == item_.key()) {
        added = false;
        return this;
      }
      if (next_ != null(iter_t *)) {
        return next_->find_or_add(k, added);
      }
      added = true;
      iter_t *neo = new iter_t(k);
      next_ = neo;
      return neo;
    }

    bool contains(const K &k) {
      if (k == item_.key()) {
        return true;
      }
      if (next_ != null(iter_t *)) {
        return next_->contains(k);
      }
      return false;
    }

  private:
    item_t item_;
    iter_t *next_;
  };

  iter_t *values[size];
  u32 len_;
};

} // namespace ect

#endif
