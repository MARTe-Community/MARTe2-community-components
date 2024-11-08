#ifndef LIST_H__
#define LIST_H__

#include <assert.h>
#include "CompilerTypes.h"

namespace MARTe {

/**
 @brief Simple double linked list container class
**/
template <typename T> class List {

public:
  /**
    @brief simple double linked iterator class for linked list of type T
  **/
  class iterator {

    T val_;          // value
    iterator *next_; // next element
    iterator *prev_; // previous element

  public:
    /**
     @brief constructor
     @param v is the value to be stored
     @param prev is the previous element of the list
    **/
    inline iterator(T v, iterator *prev = NULL_PTR(iterator *))
        : val_(v), next_(), prev_(prev) {}

    /**
      @brief destructor
    **/
    inline ~iterator() {
      prev_ = NULL_PTR(iterator *);
      next_ = NULL_PTR(iterator *);
    }

    /**
     @brief add element to the end of the list
     @parem element to be added
    **/
    inline void append(iterator *elem) {
      if (!next_) {
        next_ = elem;
      } else {
        next_->append(elem);
      }
    }

    /**
     @brief set this element as last of the list
    **/
    inline void last() { next_ = NULL_PTR(iterator *); }
    /**
     @brief set this element as first of the list
    **/
    inline void first() { prev_ = NULL_PTR(iterator *); }

    /**
     @brief get next item
     @return next item iterator pointer
    **/
    inline iterator *next() const { return next_; }
    /**
     @brief get previous item
     @return previous item iterator pointer
    **/
    inline iterator *prev() const { return prev_; }

    /**
     @brief get value inside the iterator
     @return value
    **/
    inline T &value() { return val_; }

    /**
     @brief assign operator
     @param new_val to be stored in the iterator
    **/
    inline iterator &operator=(const T &new_val) {
      val_ = new_val;
      return *this;
    }
  };

  /**
    @brief create an empty list.
  **/
  inline List() : head(), tail(), size(0) {}
  /**
    @brief copy constructor
    @param other list to be copied
  **/
  inline List(const List &other)
      : head(NULL_PTR(iterator *)), tail(NULL_PTR(iterator *)), size(0) {
    for (iterator *it = other.head; it; it = it->next()) {
      append(it->value());
    }
  }
  /**
    @brief destructor taking care of the iterators
  **/
  inline ~List() { clear(); }

  /**
    @brief get length of the list
    @return size of the list
  **/
  inline uint32 len() const { return size; }

  /**
    @brief i-th acecess operator for unsigned index
    @param i index to access
    @return item if index is valid
  **/
  inline T &operator[](const uint32 &i) const {
    assert(i < size);
    uint32 counter = 0;
    for (iterator *it = head; it || counter == i; it = it->next()) {
      if (counter++ == i) {
        return it->value();
      }
    }
    assert(false);
  }

  /**
    @brief i-th acecess operator for signed index
    @param i index to access (if negative decounted from last)
    @return item if index is valid
  **/
  inline T &operator[](const int32 &ind) const {
    int32 i = ind >= 0 ? ind : ((int32)size + ind);
    assert (i < (int32)size && i >= 0);
    int32 counter = 0;
    for (iterator *it = head; it; it = it->next()) {
      if (counter++ == i) {
        return it->value();
      }
    }
    assert(false);
  }

  /**
    @brief get iterator in order to iterate the linked list
    @return first iterator of the list
  **/
  inline iterator *iterate() const { return head; }

  /**
    @brief append an element to the last element of the list
    @pram val value to be added
  **/
  inline void append(T val) {
    iterator *next = new iterator(val, tail);
    if (!tail) {
      head = next;
    } else {
      tail->append(next);
    }
    size += 1;
    tail = next;
  }

  inline void extend(const List<T> &other) {
    for (iterator *it = other.iterate(); it; it = it->next()) {
      append(it->value());
    }
  }

  /**
    @brief pop out the last element of the list
    @return last element of the value
    @thorw E_OUT_OF_BOUNDS if no elements left in the list
  **/
  inline T pop() {
    iterator *last = tail;
    assert(size > 0 && last != NULL_PTR(iterator *));
    tail = last->prev();
    size--;
    if (size == 0) {
      head = NULL_PTR(iterator *);
      tail = NULL_PTR(iterator *);
    } else {
      tail->last();
    }
    T value = last->value();
    delete last;
    return value;
  }

  /**
    @brief pop out the first element of the list
    @return first element of the value
    @thorw E_OUT_OF_BOUNDS if no elements left in the list
  **/
  inline T pop_first() {
    iterator *first = head;
    assert(size > 0 && head != NULL_PTR(iterator *));
    head = first->next();
    size--;
    if (size == 0) {
      head = NULL_PTR(iterator *);
      tail = NULL_PTR(iterator *);
    } else {
      head->first();
    }
    T value = first->value();
    delete first;
    return value;
  }

  /**
    @brief operator to add element in the list
    @param val value to be added in the list
    @return itself
  **/
  inline List &operator+=(T val) {
    append(val);
    return *this;
  }

  /**
    @brief operator to extend list with another list
    @param list to append
    @return itself
  **/
  inline List &operator+=(const List<T> &other) {
    extend(other);
    return *this;
  }

  /**
   @brief clear list
  **/
  inline List &clear() {
    iterator *it = head;
    while (it != NULL_PTR(iterator *)) {
      iterator *to_destroy = it;
      it = it->next();
      delete to_destroy;
    }
    size = 0;
    head = NULL_PTR(iterator *);
    head = NULL_PTR(iterator *);
    tail = NULL_PTR(iterator *);
    tail = NULL_PTR(iterator *);
    return *this;
  }

  /**
    @brief assign operator
    @param other list to be copied
    @return itself
  **/
  inline List &operator=(const List &other) {
    if (head) {
      clear();
    }
    head = NULL_PTR(iterator *);
    tail = NULL_PTR(iterator *);
    size = 0;
    for (iterator *it = other.head; it; it = it->next()) {
      append(it->value());
    }
    return *this;
  }

  /**
    @brief equality operator for two lists
    @param other list to be compared
    @return true if the two list are equal
  **/
  inline bool operator==(const List &other) const {
    if (size != other.size)
      return false; // trivial false case
    if (size == 0)
      return true; // trivial true case
    iterator *it_a = head;
    iterator *it_b = other.head;
    while (it_a && it_b) {
      if (it_a->value() != it_b->value())
        return false;
      it_a = it_a->next();
      it_b = it_b->next();
    }
    return true;
  }

  /**
    @brief disequality operator for two lists
    @param other list to be compared
    @return true if the two list are not equal
  **/
  inline bool operator!=(const List &other) const { return !(*this == other); }

  /**
    @brief check if element is contained in the list
    @param element to be checked
    @return true if the element exists elese false
  **/
  inline bool contains(T element) const {
    for (iterator *it = head; it; it = it->next())
      if (it->value() == element)
        return true;
    return false;
  };

private:
  iterator *head; // first element of the list
  iterator *tail; // last element of the list
  uint32 size;       // size of the list
};
}; // namespace ect

#endif
