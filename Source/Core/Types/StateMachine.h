#ifndef _EC_STATE_MACHINE_H__
#define _EC_STATE_MACHINE_H__

#include "array.h"
#include "optional.h"
#include "pair.h"
#include "ptr.h"
#include "result.h"

namespace ect {

/**
  @brief Link class represent the link relation between two
  object.

  If the condition is verified then the target node is returned.
**/
template <typename nodeT, typename argT> class Link {
  typedef bool (*fn_t)(const argT); // typedef of all callback types
public:
  /**
    @brief Constructor of a simple link object.
    @param node is the target node of this link
    @param condition is the condition that will be verified to enable the link.
           If null the condition will be always true.
    @param prio is the priority of this link
  **/
  inline Link(nodeT node, fn_t condition = null(fn_t), u32 prio = 0)
      : node_(node), condition_(condition), prio_(static_cast<i32>(prio)) {}

  /**
    @brief empty constructor.
  **/
  inline Link() : node_(), condition_(null(fn_t)), prio_(0) {}
  /**
    @brief evaluate link condition and return the target if condition is met.
    @param arg is the input argument used to evaluate the condition
    @return target node if condition is null or valid
    @return empty optional otherwise
  **/
  inline Optional<nodeT> operator()(argT arg) const {
    return (condition_ == null(fn_t) || condition_(arg))
               ? Optional<nodeT>(node_)
               : Optional<nodeT>();
  }

  inline nodeT node() const { return node_; }

  /**
    @brief get link priority
    @return node priority
  **/
  inline i32 prio() const { return (i32)prio_; }
  /**
    @brief set link priority
    @param prio is the new priority
    @return the link itself
  **/
  inline Link &Priority(const u32 prio) {
    prio_ = static_cast<i32>(prio);
    return *this;
  }

  /**
    @brief equality operator
    @return true if all fields match
  **/
  inline bool operator==(const Link &other) const {
    return other.prio_ == prio_ && other.node_ == node_ &&
           other.condition_ == condition_;
  }

  /**
    @brief trivial disequality operator
    @return true if any fields do not match
  **/
  inline bool operator!=(const Link &other) const {
    return other.prio_ != prio_ || other.node_ != node_ ||
           other.condition_ != condition_;
  }

  // inline const nodeT target() { return node_; }
  // inline const fn_t condition() { return condition_; }
  /**
   @brief set condition of the link
  @param condition to set
  **/
  inline void set_condition(fn_t condition) { condition_ = condition; }
  inline Link &GoTo(nodeT node) {
    node_ = node;
    return *this;
  }

private:
  nodeT node_;     // Target node
  fn_t condition_; // link condition
  i32 prio_;       // link priority
};

/**
  @brief Class representing a state of a state machine

  A state can contains an entry function, a cycle function and an exit function.
  Additionally a state contains a list of links to other states.
  At every cycle the cycle function is evaluated as well as all the links of the
  state.
**/
template <typename argT> class State {
  typedef bool (*cond_t)(const argT);
  typedef Result<argT> (*fn_t)(const argT); // typedef of callback functions
  typedef Link<ptr<State>, argT> link_t; // typedef of links for this State type
  typedef ptr<State> State_p;
  typedef Pair<argT, State_p>
      res_next;  // typedef of resultin pair
                 // containing the result of the processing
                 // and the next state to evaluate
  fn_t entry_fn; // function called when entering in the state
  fn_t cycle_fn; // state function (called at every cycle)
  fn_t exit_fn;  // function called when exiting from the state

  Array<link_t> links_; // list of links of this state

  u32 id_;

public:
  /**
   @brief State constructor
   @param callback is the function to be used at every state cycle.
  **/
  inline State(u32 id = 0, fn_t callback = null(fn_t))
      : entry_fn(null(fn_t)), cycle_fn(callback), exit_fn(null(fn_t)), links_(),
        id_(id) {}

  /**
    @brief operator for adding a link to the state
    @param link is the new link to append
    @return the state itself
  **/
  inline link_t &When(cond_t condition) {
    link_t link;
    link.set_condition(condition);
    links_ += link;

    return links_[links_.len() - 1];
  }

  /**
    @brief operator for setting the exit callback
    @param callback is the function that will be used as exit callback
    @return the state itself
  **/
  inline State &After(fn_t callback) {
    exit_fn = callback;
    return *this;
  }

  /**
    @brief operator for setting the entry callback
    @param callback is the function that will be used as entry callback
    @return the state itself
  **/
  inline State &Before(fn_t callback) {
    entry_fn = callback;
    return *this;
  }

  inline Result<argT> Entry(argT arg) {
    if (entry_fn != null(fn_t)) {
      return entry_fn(arg);
    }
    return arg;
  }

  inline Result<argT> Exit(argT arg) {
    if (exit_fn != null(fn_t)) {
      return exit_fn(arg);
    }
    return arg;
  }

  /**
    @brief Execute the state cycle
    @param arg is the input to the state evaluation
    @return a pair container containing the result of the cycle evaluation and
    the next state to execute if the execution was succesfull
    @return an error if the execution failed
  **/
  inline Result<res_next> Eval(argT arg) const {
    Result<argT> res = cycle_fn != null(fn_t) ? cycle_fn(arg) : arg;
    if (!res)
      return res.err();
    ptr<State> next;
    i32 prio = -1;
    for (u32 i = 0; i < links_.len(); i++) {
      if (links_[i].prio() >= prio) {
        Optional<State_p> linked = links_[i](res.val());
        if (!linked.empty()) {
          prio = links_[i].prio();
          next = linked.val();
        }
      }
    }

    if (!next.isNull()) {
      res = exit_fn != null(fn_t) ? exit_fn(res.val()) : res;
      if (!res)
        return res.err();
      res = next->entry_fn != null(fn_t) ? next->entry_fn(res.val()) : res;
      if (!res)
        return res.err();
    } else {
      next = ptr<State>(*this);
    }
    return res_next(res.val(), next);
  }

  /**
    @brief operator for executing a state cycle
    @param arg is the input to the state evaluation
    @return a pair container containing the result of the cycle evaluation and
    the next state to execute if the execution was succesfull
    @return an error if the execution failed
  **/
  inline Result<res_next> operator()(argT arg) const { return Eval(arg); }

  /**
    @brief Gets the number of links of a state
    @return length of the link list
  **/
  inline u32 numberOfLinks() const { return links_.len(); }

  /**
    @brief Gets the list of links of a state
    @return link list
  **/
  inline Array<link_t> links() const { return links_; }

  /**
    @brief Gets the state id
    @return the id
  **/
  inline u32 id() const { return id_; }

  /**
    @brief Equality operator for two states
  **/
  inline bool operator==(const State &other) const {
    return other.id_ == id_ && other.links_ == links_;
  }
  /**
    @brief Disequality operator for two states
  **/
  inline bool operator!=(const State &other) const {
    return other.id_ != id_ || other.links_ != links_;
  }
};

}; // namespace ect

#endif
