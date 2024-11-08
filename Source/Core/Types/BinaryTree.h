#ifndef _BTREE_H__
#define _BTREE_H__

#include "CompilerTypes.h"
#include "List.h"
#include "Option.h"
#include "Pair.h"
namespace MARTe {

template <typename K, typename V> class BinaryTree {
public:
  class node_t {
  public:
    inline node_t(const K &key, const V &value)
        : key(key), value(value), left(NULL_PTR(node_t *)),
          right(NULL_PTR(node_t *)), size_left(0), size_right(0) {}
    inline node_t(const K &key)
        : key(key), value(), left(NULL_PTR(node_t *)),
          right(NULL_PTR(node_t *)), size_left(0), size_right(0) {}
    inline ~node_t() {
      if (right) {
        delete right;
        size_right = 0;
      }
      if (left) {
        delete left;
        size_left = 0;
      }
    }

    inline uint32 len() { return size_left + size_right; }

    inline bool contains(const K &k) const {
      if (key == k) {
        return true;
      }
      if (key < k) {
        return left ? left->contains(k) : false;
      }
      return right ? right->contains(k) : false;
    }
    inline node_t &operator=(const V &v) {
      value = v;
      return *this;
    }
    inline operator V &() { return value.val(); }
    inline Option<V> &val() { return value; }

  protected:
    inline node_t *get(const K &k) {
      if (key == k) {
        return this;
      }
      if (key < k) {
        return left ? left->get(k) : NULL_PTR(node_t *);
      }
      return right ? right->get(k) : NULL_PTR(node_t *);
    }

    inline node_t *get_or_add(const K &k) {
      bool added = false;
      return get_or_add_with_count(k, added);
    }
    inline node_t *set_or_add(const K &k, const V &v) {
      bool added = false;
      return set_or_add_with_count(k, v, added);
    }

  private:
    K key;
    Option<V> value;
    node_t *left;
    node_t *right;
    uint32 size_left;
    uint32 size_right;

    inline node_t *get_or_add_with_count(const K &k, bool &added) {
      if (key == k) {
        added = false;
        return this;
      }
      if (key < k) {
        if (left) {
          node_t *n = left->get_or_add_with_count(k, added);
          size_left += added ? 1 : 0;
          return n;
        }
        left = new node_t(k);
        size_left = 1;
        added = true;
        return left;
      }
      if (right) {
        node_t *n = right->get_or_add_with_count(k, added);
        size_right += added ? 1 : 0;
        return n;
      }
      right = new node_t(k);
      size_right = 1;
      added = true;
      return right;
    }
    inline node_t *set_or_add_with_count(const K &k, const V &v, bool &added) {
      if (key == k) {
        this->value = v;
        added = false;
        return this;
      }
      if (key < k) {
        if (left) {
          node_t *n = left->set_or_add_with_count(k, v, added);
          size_left += added ? 1 : 0;
          return n;
        }
        left = new node_t(k, v);
        added = true;
        size_left = 1;
        return left;
      }
      if (right) {
        node_t *n = right->set_or_add_with_count(k, v, added);
        size_right += added ? 1 : 0;
        return n;
      }
      right = new node_t(k, v);
      added = true;
      size_right = 1;
      return right;
    }
  };

  inline BinaryTree() : root(NULL_PTR(node_t *)) {}
  inline ~BinaryTree() {
    if (root) {
      delete root;
    }
  }
  inline node_t &operator[](const K &k) {
    if (root) {
      return *root->get_or_add(k);
    }
    root = new node_t(k);
    return *root;
  }

  inline uint32 len() const {
    if (root) {
      return 1 + root->len();
    }
    return 0;
  }

  inline Option<V> get(const K &k) {
    node_t *n = NULL_PTR(node_t *);
    if (root) {
      n = root->get(k);
    }
    if (n != NULL_PTR(node_t *)) {
      return n->val();
    }
    return Option<V>();
  }
  inline void set(const K &k, const V &v) {
    if (root) {
      root->set_or_add(k, v);
    } else {
      root = new node_t(k, v);
    }
  }
  inline bool contains(const K &k) const {
    if (root) {
      return root->contains(k);
    }
    return false;
  }

  inline List<K> keys() {
    List<K> list;
    if (root) {
      fill_key_list(root, list);
    }
    return list;
  }
  inline List<Pair<K, V &>> items() {
    List<Pair<K, V &>> list;
    if (root) {
      fill_items_list(root, list);
    }
    return list;
  }

private:
  node_t *root;

  inline void fill_key_list(node_t *n, List<K> &list) {
    list.append(n->key);
    if (n->left) {
      fill_key_list(n->left, list);
    }
    if (n->right) {
      fill_key_list(n->right, list);
    }
  }
  inline void fill_items_list(node_t *n, List<Pair<K, V &>> &list) {
    list.append(Pair<K, V &>(n->key, n->val().val()));
    if (n->left) {
      fill_items_list(n->left, list);
    }
    if (n->right) {
      fill_items_list(n->right, list);
    }
  }
};
} // namespace MARTe

#endif
