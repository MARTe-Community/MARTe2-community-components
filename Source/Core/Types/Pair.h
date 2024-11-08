#ifndef _PAIR_H__
#define _PAIR_H__

namespace MARTe {
/**
  @brief Container for pairs of values

  To use when you want to pack 2 value
  s of types T and Y togheter
**/
template <typename T, typename Y> struct Pair {
  T left;  // left value
  Y right; // right value


  /**
    @brief simple constructor
    @param left value
    @param right value
  **/
  inline Pair(T left, Y right) : left(left), right(right) {}

};
}; // namespace ect

#endif

