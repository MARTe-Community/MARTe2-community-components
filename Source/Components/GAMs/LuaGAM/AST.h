#ifndef _LUAGAM_AST_H__
#define _LUAGAM_AST_H__

#include "LuaParserBaseTypes.h"

namespace MARTe {
namespace LUA {

struct ast_t {

  /**
   * @brief Add node to ast
   * @param[in] n node
   */
  void add(Nodep n);

  /**
   * @brief Addition assignment operator
   * @see add
   **/
  ast_t &operator+=(const Nodep &n);

  /**
   * @brief Access ast methods
   **/
  NodepList *operator->();

  /**
   * @brief Access token list elements
   **/
  Nodep &operator[](const uint32 &ind) const;

  /**
   * @brief Print ast
   */
  void print();
  NodepList nodes; //!< List of nodes representing the Abstract syntax tree
};

/**
 * @brief Generate abstract syntax tree
 * @param[out] ok control flag reference for correct ast generation
 * @return ast
 */
ast_t generate_ast(tokens_t tokens, bool &ok);
} // namespace LUA
} // namespace MARTe
#endif
