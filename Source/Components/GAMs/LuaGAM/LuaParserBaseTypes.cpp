/**
 * @file LuaParserBaseTypes.h
 * @brief Header file for class Lua Parser Base Types
 * @date 17/06/2024
 * @author Andrea Antonione
 * @author Martino Ferrari
 */

#include "LuaParserBaseTypes.h"
#include "StringHelper.h"
#include <stdio.h>
#include <string.h>

namespace MARTe {
namespace LUA {

#define TOKEN_LOG(token)                                                       \
  printf("TOKEN_LOG [%s:%d]:\t", __FILE__, __LINE__);                          \
  printf("Line:%i, Col:%i, Token:`%s`, Type:`%s`\n", token->row, token->col,   \
         token->raw, lua_syntax_element_names[token->type]);

#define DEBUG_LOG(...)                                                         \
  printf("DEBUG_LOG [%s:%d]\t", __FILE__, __LINE__);                           \
  printf(__VA_ARGS__);

/**
 * @brief Check if a character is a decimal number
 */
bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }

/**
 * @brief Add token type
 */
uint32 tok_type(const char *str, const uint32 len) {
  if (len > 0) {
    if (str[0] == '"' || str[0] == '\'') { // Check if it is a string
      return STRING;
    } else if (len >= 2 && str[0] == '-' && str[1] == '-') {
      return COMM;
    } else if (is_digit(str[0])) {
      return NUM;
    } else {
      for (uint8 i = 0; i < NUM_LUA_SYNTAX_ELEMENTS; i++) {
        if (StringHelper::Length(lua_syntax_elements[i]) == len &&
            StringHelper::CompareN(str, lua_syntax_elements[i], len) == 0) {
          return i;
        }
      }
    }
  }
  return ID;
}

/**
 * @brief Check if a token type is a unary operator
 */
bool is_unop(uint32 type) {
  for (uint8 i = 0; i < NUM_UNOP; i++) {
    if (type == unop_list[i]) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Check if a token type is a binary operator
 */
bool is_binop(uint32 type) {
  for (uint8 i = 0; i < NUM_BINOP; i++) {
    if (type == binop_list[i]) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Check if a variable is present in the node and its subnodes.
 * @param[in] node node search the variable in
 * @param[in] name variable name
 * @param[in] node_type type of node containing the variable
 * @param[out] row variable line in Lua code
 * @param[out] col variable columne in Lua code
 * @return true if the variable is found and it is contained in the correct
 * node
 */
Token::Token() {
  raw = NULL_PTR(char8 *);
  type = ID;
  row = 0;
  col = 0;
  unop = false;
  binop = false;
}

Token::Token(const char8 *str, uint32 len, uint32 row, uint32 col)
    : raw(str), type(tok_type(str, len)), row(row), col(col),
      unop(is_unop(type)), binop(is_binop(type)) {}

Token::Token(char8 *str, uint32 len, uint32 row, uint32 col)
    : raw(str), type(tok_type(str, len)), row(row), col(col),
      unop(is_unop(type)), binop(is_binop(type)) {}

Token::~Token() {}

Str Token::toString(bool show_type, bool show_val) {
  char buff[1024];
  Str s;
  if (show_type) {
    sprintf(buff, "tok:%s", lua_syntax_element_names[type]);
    s = s + buff;
  }
  if (raw != NULL_PTR(char8 *) && show_val) {
    if (show_type) {
      s = s + ' ';
    }
    sprintf(buff, "val:`%s`", raw.cstr());
    s = s + buff;
  }
  return s;
}

void tokens_t::add(Tokenp t) { toks.append(t); }

void tokens_t::add(Str t, uint32 row, uint32 col) {
  toks.append(new Token(t.cstr(), t.len(), row, col));
}

void tokens_t::add(const char8 *t, uint32 row, uint32 col, uint32 len) {
  toks.append(new Token(t, len, row, col));
}

void tokens_t::add(char8 *t, uint32 row, uint32 col, uint32 len) {
  toks.append(new Token(t, len, row, col));
}

void tokens_t::add_long_bracket_token(strList code_lines, uint32 &line_index,
                                      uint32 &line_pos, uint32 &char_pos,
                                      bool is_string, bool &ok) {
  uint32 len_token = 0;
  uint32 add_tok_len = 1;
  Str line = code_lines[line_index];
  if (!is_string) {
    add_tok_len = 3;
  }
  Str closing_long_bracket = "]";
  Str long_token = "";
  uint32 long_bracket_level = 0;
  while (ok) {
    if (line[line_pos + char_pos + long_bracket_level + add_tok_len] == '=') {
      long_bracket_level++;
      closing_long_bracket = closing_long_bracket + '=';
    } else {
      if (line[line_pos + char_pos + long_bracket_level + add_tok_len] != '[') {
        ok = false;
      }
      closing_long_bracket = closing_long_bracket + ']';
      break;
    }
  }
  if (ok) {
    Str line_str = line;
    uint32 init_line_index = line_index;
    int32 end_index = line_str.find(closing_long_bracket);
    if (end_index < 0 && line_index + 1 < code_lines.len()) {
      long_token = long_token + line_str;
      line_index++;
      line_str = code_lines[line_index];
      end_index = line_str.find(closing_long_bracket);
    }
    while (end_index < 0 && line_index + 1 < code_lines.len()) {
      long_token = long_token + '\n';
      long_token = long_token + line_str;
      line_index++;
      line_str = code_lines[line_index];
      end_index = line_str.find(closing_long_bracket);
    }
    if (line_index > init_line_index && long_token[-1] != '\n') {
      long_token = long_token + '\n';
    }
    if (end_index > 0) {
      long_token = long_token + line_str.substr(0, end_index);
    }
    long_token = long_token + closing_long_bracket;
    add(long_token.cstr(), init_line_index, line_pos + char_pos,
        long_token.len());
  }
  line_index++;
}

uint32 tokens_t::len() { return toks.len(); }

void tokens_t::print() {
  for (TokenpList::iterator *it = toks.iterate(); it; it = it->next()) {
    printf("%s\n", it->value()->toString().cstr());
  }
}

TokenpList *tokens_t::operator->() { return &toks; }

Tokenp &tokens_t::operator[](const uint32 &ind) const { return toks[ind]; }

Node::Node() {
  type = UNDEFINED; // by default
}

Node::Node(LuaNode n, Tokenp t) : tok(t), type(n) {}

Node::Node(LuaNode n) : type(n) {}

Node::~Node() {}

void Node::append(Nodep n) {
  if (n) {
    sub_nodes.append(n);
  }
}

void Node::append(Tokenp tok_, LuaNode type_) {
  if (!tok_.isNull()) {
    sub_nodes.append(new Node(type_, tok_));
  }
}

Str Node::toString(uint32 level, uint32 indent_level, bool show_val) {
  char *indent = new char[level * indent_level + 1];
  memset(indent, ' ', level * indent_level);
  indent[level * indent_level] = 0;
  char buff[1024];
  sprintf(buff, "%s(%s", indent, lua_node_names[type]);
  Str s = buff;
  if (!tok.isNull()) {
    sprintf(buff, " tok:%s", lua_syntax_element_names[tok->type]);
    s = s + buff;
    if (show_val) {
      sprintf(buff, " val:`%s`", tok->raw.cstr());
      s = s + buff;
    }
  }
  if (sub_nodes.len() > 0) {
    s = s + "\n";
    for (uint32 i = 0; i < sub_nodes.len(); i++) {
      s = s + sub_nodes[i]->toString(level + 1, indent_level, show_val);
    }
    s = s + indent;
  }
  s = s + ")\n";
  delete[] indent;
  return s;
}

bool Node::empty() {
  if (!tok.isNull() && sub_nodes.len() == 0) {
    return true;
  }
  return false;
}

void Node::del() {
  for (uint32 i = 0; i < sub_nodes.len(); i++) {
    sub_nodes[i]->del();
  }
  for (uint32 i = 0; i < sub_nodes.len(); i++) {
    sub_nodes.pop();
  }
}
} // namespace LUA
} // namespace MARTe
