/**
 * @file LuaParser.cpp
 * @brief Source file for the LuaParser class
 * @date 13/02/2024
 * @author Andrea Antonione
 * @author Martino Ferrari
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing,
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class LuaParser (public, protected, and private). Be aware
 * that some methods, such as those inline could be defined on the header file,
 * instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "AdvancedErrorManagement.h"
#include "Architecture/x86_gcc/CompilerTypes.h"
#include "LuaParser.h"
#include "StringHelper.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

#define TOKEN_ERROR(line, line_i, pos, error)                                  \
  REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,                    \
                      "[Line:%i, Token:`%s`]\t%s", line_i,                     \
                      line.substr(pos, 0).cstr(), error);

#define DEBUG_LOG(...)                                                         \
  printf("DEBUG_LOG [%s:%d]\t", __FILE__, __LINE__);                           \
  printf(__VA_ARGS__);

#define DBG DEBUG_LOG("Debug\n");

namespace MARTe {
namespace LUA {

/**
 * @brief Check if a character is a decimal
 * @param[in] character character
 * @return true if character is a decimal
 */
bool is_decimal(char8 character) {
  return character >= '0' && character <= '9';
}

/**
 * @brief Check if a character is a decimal
 * @param[in] character character
 * @return true if character is a decimal
 */
bool is_decimal(char8 *character) { return is_decimal(*(char8 *)character); }

/**
 * @brief Check if a character is a decimal
 * @param[in] character character
 * @return true if character is a decimal
 */
bool is_decimal(const char8 *character) {
  return is_decimal(*(char8 *)character);
}

/**
 * @brief Consume number in index or floating point
 * @param[in] num number string
 * @param[in] num_len number string length
 * @return true if character is an exadedimal letter
 */
void consume_number(char8 *num, uint32 &num_len) {
  num_len++;
  if (num[num_len] == '+' || num[num_len] == '-') {
    num_len++;
  }
  while (is_decimal(num + num_len)) {
    num_len++;
  }
}

/**
 * @brief Consume number in index or floating point
 * @param[in] num number string
 * @param[in] num_len number string length
 * @return true if character is an exadedimal letter
 */
void consume_number(const char8 *num, uint32 &num_len) {
  consume_number((char8 *)num, num_len);
}

/**
 * @brief Check if a character is an exadedimal letter
 * @param[in] character character
 * @return true if character is an exadedimal letter
 */
bool is_hexadecimal_letter(char8 *character) {
  return (*character >= 'a' && *character <= 'f') ||
         (*character >= 'A' && *character <= 'F');
}

/**
 * @brief Check if a character is an exadedimal letter
 * @param[in] character character
 * @return true if character is an exadedimal letter
 */
bool is_hexadecimal_letter(const char8 *character) {
  return is_hexadecimal_letter((char8 *)character);
}

/**
 * @brief Check if a string is a Lua number
 * @param[in] string string
 * @param[out] number_len length of the number as a character
 * @return true if correct parsing
 */
bool check_hexadecimal(char8 *string, uint32 &number_len) {
  number_len = 0;
  bool ok = true;
  bool found_dot = false;
  if (string[0] == '0' && (string[1] == 'x' || string[1] == 'X')) {
    number_len += 2;
  } else {
    ok = false;
  }

  if ((is_hexadecimal_letter(string + number_len)) ||
      (is_decimal(string + number_len))) {
    number_len++;
  } else {
    REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                        "Expecting number after `x` or `X`.");
    ok = false;
  }

  while (ok) {
    if ((is_hexadecimal_letter(string + number_len)) ||
        (is_decimal(string + number_len))) {
      number_len++;
    } else if (string[number_len] == '.') {
      if (found_dot) {
        REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                            "Wrong number punctuation.");
        ok = false;
      }
      found_dot = true;
      number_len++;
      if ((!is_decimal(string + number_len))) {
        REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                            "Expecting integer after dot (`.`) character.");
        ok = false;
      }
      number_len++;
    } else if (string[number_len] == 'p' || string[number_len] == 'P') {
      consume_number(string, number_len);
      break;
    } else {
      break;
    }
  }
  return ok;
}

/**
 * @brief Check if a string is a Lua number
 * @param[in] string string
 * @param[out] number_len length of the number as a character
 * @return true if correct parsing
 */
bool check_hexadecimal(const char8 *string, uint32 &number_len) {
  return check_hexadecimal((char8 *)string, number_len);
}

/**
 * @brief Check if a string is a Lua decimal number
 * @param[in] string string
 * @param[out] number_len length of the number as a character
 * @return true if correct parsing
 */
bool check_decimal(char8 *character, uint32 &number_len) {
  number_len = 0;
  bool ok = true;
  bool found_dot = false;

  while (ok) {
    if (is_decimal(character + number_len)) {
      number_len++;
    } else if (character[number_len] == '.') {
      if (found_dot) {
        REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                            "Wrong number punctuation.");
        ok = false;
      }
      found_dot = true;
      number_len++;
    } else if (character[number_len] == 'E' || character[number_len] == 'e') {
      consume_number(character, number_len);
      break;
    } else {
      break;
    }
  }

  return ok;
}

/**
 * @brief Check if a string is a Lua decimal number
 * @param[in] string string
 * @param[out] number_len length of the number as a character
 * @return true if correct parsing
 */
bool check_decimal(const char8 *string, uint32 &number_len) {
  return check_decimal((char8 *)string, number_len);
}

/**
 * @brief Check if a string is Lua token
 * @param[in] string string
 * @param[out] len_token lua_token_length
 * @return true if string is a Lua token
 */
bool is_lua_token(char8 *string, uint32 &len_token) {
  len_token = 0;
  bool ok = false;
  for (uint8 lua_tokens_index = VARARGS;
       (lua_tokens_index < SPACE + 1) && (!ok); lua_tokens_index++) {
    len_token = StringHelper::Length(lua_syntax_elements[lua_tokens_index]);
    ok = StringHelper::CompareN(string, lua_syntax_elements[lua_tokens_index],
                                len_token) == 0;
  }
  if (!ok)
    len_token = 0;
  return ok;
}

/**
 * @brief Check if a string is Lua token
 * @param[in] string string
 * @param[out] len_token lua_token_length
 * @return true if string is a Lua token
 */
bool is_lua_token(const char8 *string, uint32 &len_token) {
  return is_lua_token((char8 *)string, len_token);
}

/**
 * @brief Check if a string is a Lua number
 * @param[in] string string
 * @param[out] number_len length of the number as a character
 * @return true if correct parsing
 */
bool check_number(char8 *string, uint32 &number_len) {
  number_len = 0;
  bool ok = true;
  if (string[0] == '0' && (string[1] == 'x' || string[1] == 'X')) {
    ok = check_hexadecimal(string, number_len);
  } else if (is_decimal(string)) {
    ok = check_decimal(string, number_len);
  }

  return ok;
}

/**
 * @brief Check if a string is a Lua number
 * @param[in] string string
 * @param[out] number_len length of the number as a character
 * @return true if correct parsing
 */
bool check_number(const char8 *string, uint32 &number_len) {
  return check_number((char8 *)string, number_len);
}

/**
 * @brief Check if a variable is present in the node and its subnodes.
 * @param[in] node node search the variable in
 * @param[in] name variable name
 * @param[in] node_type type of node containing the variable
 * @param[out] line_i variable line in Lua code
 * @param[out] col_i variable columne in Lua code
 * @return true if the variable is found and it is contained in the correct
 * node
 */
bool check_variable(Nodep node, const char8 *name, LuaNode node_type,
                    uint32 &line_i, uint32 &col_i) {
  bool found = false;
  if (node->type == node_type) {
    for (Rc<NodepList::iterator> it = node->sub_nodes.iterate(); it && !found;
         it = it->next()) {
      for (Rc<NodepList::iterator> jt = it->value()->sub_nodes.iterate();
           jt && !found; jt = jt->next()) {
        if (!jt->value()->tok.isNull() && jt->value()->tok->raw == name) {
          found = true;
          line_i = jt->value()->tok->row;
          col_i = jt->value()->tok->col;
        }
      }
    }
  }
  if (!found) {
    for (Rc<NodepList::iterator> it = node->sub_nodes.iterate(); it;
         it = it->next()) {
      if (check_variable(it->value(), name, node_type, line_i, col_i)) {
        found = true;
        break;
      }
    }
  }
  return found;
}

/**
 * @brief Divide Lua code in lines.
 * @param[in] code Lua code
 * @param[out] ok flag for correct linearization
 * @return list of lines.
 */
strList linearize(const char8 *code) {
  strList lines;
  Str code_s = code;
  uint32 code_pos = 0;
  code_s = code_s + '\n';
  Option<uint32> maybe_eol_i = code_s.find("\n");
  while (!maybe_eol_i.empty()) {
    uint32 eol_i = maybe_eol_i.val();
    uint32 nl = eol_i == 0 ? (eol_i + 1) : eol_i;
    Str line = code_s.substr(0, nl);
    if (line.len() > 0) {
      lines.append(line);
    }
    code_s = code_s.substr(eol_i + 1, 0);
    maybe_eol_i = code_s.find("\n");
  }
  return lines;
}

/**
 * @brief Add long brackets token
 * @param[in] code_lines Lua code lines list
 * @param[in] line_index current line index
 * @param[in] line_pos current line position
 * @param[in] char_pos current character position
 * @param[in] is_string expected string flag
 * @param[in] ok flag for correct tokenization
 */
Tokenp get_long_bracket_token(strList code_lines, uint32 &line_index,
                              uint32 &line_pos, uint32 &char_pos,
                              bool is_string, bool &ok) {
  Tokenp t;
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
    Option<uint32> end_index = line_str.find(closing_long_bracket);
    if (end_index.empty() && line_index + 1 < code_lines.len()) {
      long_token = long_token + line_str;
      line_index++;
      line_str = code_lines[line_index];
      end_index = line_str.find(closing_long_bracket);
    }
    while (end_index.empty() && line_index + 1 < code_lines.len()) {
      long_token = long_token + '\n';
      long_token = long_token + line_str;
      line_index++;
      line_str = code_lines[line_index];
      end_index = line_str.find(closing_long_bracket);
    }
    if (line_index > init_line_index && long_token[-1] != '\n') {
      long_token = long_token + '\n';
    }
    if (!end_index.empty() && end_index.val() > 0) {
      long_token = long_token + line_str.substr(0, end_index.val());
    }
    long_token = long_token + closing_long_bracket;
    t = new Token(long_token.cstr(), long_token.len(), init_line_index,
                  line_pos + char_pos);
  }
  line_index++;
  return t;
}

tokens_t scan(const char8 *code, bool &ok) {
  strList lines = linearize(code);
  tokens_t tokens;
  uint32 line_pos = 0;
  uint32 char_pos = 0;
  uint32 len_token = 0;
  uint32 line_index = 0;

  while (line_index < lines.len()) {
    Str line = lines[line_index];
    line_pos = 0;
    char_pos = 0;

    // Iterate through lines
    while (line_pos + char_pos < line.len() && ok && line_index < lines.len()) {
      len_token = 0;

      // Skip whitespace
      if (line[line_pos + char_pos] == ' ' ||
          line[line_pos + char_pos] == '\t') {
        if (char_pos > 0) {
          tokens.add(line.substr(line_pos, char_pos + line_pos), line_index,
                     line_pos);
          line_pos += char_pos;
          char_pos = 0;
        }
        line_pos++;
      }

      // Check for string
      else if (line[line_pos + char_pos] == '\"' ||
               line[line_pos + char_pos] == '\'') {
        char8 apex = line[line_pos + char_pos];
        uint32 closing_quotes_index = 0;
        while (true) {
          Option<uint32> end_string =
              line.find(apex, line_pos + char_pos + closing_quotes_index + 1);
          if (end_string.empty()) {
            TOKEN_ERROR(line, line_index, line_pos + char_pos,
                        "String not closed.");
            ok = false;
            break;
          }
          if (line[end_string.val() - 1] != '\\') {
            tokens.add(line.substr(line_pos + char_pos, end_string.val() + 1),
                       line_index, line_pos + char_pos);
            line_pos = end_string.val() + 1;
            char_pos = 0;
            break;
          } else {
            closing_quotes_index = end_string.val();
          }
        }
      } else if (line.substr(line_pos + char_pos, 0).len() > 2 &&
                 line[line_pos + char_pos] == '[' &&
                 ((line[line_pos + char_pos + 1] == '[') ||
                  (line[line_pos + char_pos + 1] == '='))) {
        tokens.add(get_long_bracket_token(lines, line_index, line_pos, char_pos,
                                          true, ok));
        if (line_index == lines.len()) {
          break;
        } else {
          line = lines[line_index];
        }
        if (!ok) {
          TOKEN_ERROR(line, line_index, line_pos + char_pos,
                      "Error in long bracket.");
        }
      }

      // Check for comments -> after --
      else if (line.substr(line_pos + char_pos, 0).len() >= 2 &&
               line[line_pos + char_pos] == '-' &&
               line[line_pos + char_pos + 1] == '-') {
        if (line.substr(line_pos + char_pos + 1, 0).len() > 2 &&
            line[line_pos + char_pos + 2] == '[' &&
            (line[line_pos + char_pos + 3] == '[' ||
             line[line_pos + char_pos + 3] == '=')) {
          tokens.add(get_long_bracket_token(lines, line_index, line_pos,
                                            char_pos, false, ok));
          if (line_index == lines.len()) {
            break;
          } else {
            line = lines[line_index];
          }
          if (!ok) {
            TOKEN_ERROR(line, line_index, line_pos + char_pos,
                        "Error in long bracket.");
          }
        } else {
          len_token = line.substr(line_pos + char_pos, line.len()).len();
          tokens.add(line.substr(line_pos + char_pos, 0), line_index,
                     line_pos + char_pos);
          char_pos += len_token + 1;
        }
      }

      // Number
      else if (is_decimal(line.substr(line_pos + char_pos, 0).cstr()) &&
               char_pos == 0) {
        if (check_number(line.substr(line_pos + char_pos, 0).cstr(),
                         len_token)) {
          if (len_token > 0) {
            tokens.add(line.substr(line_pos + char_pos,
                                   line_pos + char_pos + len_token),
                       line_index, line_pos + char_pos);
          }
        } else {
          TOKEN_ERROR(line, line_index, line_pos + char_pos, "Wrong number.");
          ok = false;
        }
        line_pos += len_token;
      }

      // Check for Lua tokens
      else if (is_lua_token(line.substr(line_pos + char_pos, 0).cstr(),
                            len_token)) {
        if (char_pos == 0) {
          tokens.add(line.substr(line_pos, line_pos + len_token).cstr(),
                     line_index, line_pos, len_token);
        } else {
          tokens.add(line.substr(line_pos, line_pos + char_pos).cstr(),
                     line_index, line_pos, char_pos);
          tokens.add(
              line.substr(line_pos + char_pos, line_pos + char_pos + len_token)
                  .cstr(),
              line_index, line_pos + char_pos, len_token);
        }
        line_pos += char_pos + len_token;
        char_pos = 0;
      }

      // End of line
      else if (line_pos + char_pos == line.len() - 1 &&
               line[line_pos + char_pos] != '\n') {
        len_token = line.substr(line_pos, 0).len();
        tokens.add(line.substr(line_pos, 0), line_index, line_pos + char_pos);
        line_pos += char_pos + len_token;
        char_pos = 0;
      }

      else {
        char_pos++;
      }
    }
    line_index++;
  }
  if (ok) {
    tokens.add("endcode", line_index, line_pos + char_pos, 7);
  }
  return tokens;
}

ast_t parse(const MARTe::char8 *code, bool &ok) {
  bool ret = true;
  tokens_t tokens;
  ast_t ast;
  tokens = scan(code, ret);
  if (ret) {
    ast = generate_ast(tokens, ret);
  }
  ok &= ret;
  return ast;
}
} // namespace LUA
} // namespace MARTe
