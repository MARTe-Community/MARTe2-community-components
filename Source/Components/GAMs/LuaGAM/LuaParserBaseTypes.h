#ifndef LUA_PARSER_BASE_TYPES_H__
#define LUA_PARSER_BASE_TYPES_H__

#include "CompilerTypes.h"
#include "List.h"
#include "Rc.h"
#include "Str.h"

#define MAX_LENGTH 256
#define NUM_LUA_SYNTAX_ELEMENTS 63
#define NUM_UNOP 4
#define NUM_BINOP 21

namespace MARTe {
namespace LUA {
typedef List<Str> strList;
enum LuaToken {
  REQUIRE = 0,
  ENDCODE,
  ENDLINE,
  AND,
  BREAK,
  DO,
  ELSE,
  ELSEIF,
  END,
  FALSE,
  FOR,
  FUNCTION,
  GOTO,
  IF,
  IN,
  LOCAL,
  NIL,
  NOT,
  OR,
  REPEAT,
  RETURN,
  THEN,
  TRUE,
  UNTIL,
  WHILE,
  VARARGS,
  CONCAT,
  FLOOR,
  EQ,
  NEQ,
  LTEQ,
  GTEQ,
  COLCOL,
  BITRSHIFT,
  BITRLEFT,
  ADD,
  MINUS,
  MULT,
  DIV,
  MOD,
  POW,
  LENGTH,
  LT,
  GT,
  ASSIGN,
  BITAND,
  BITOR,
  BITNOT,
  OPAR,
  CPAR,
  OBRACE,
  CBRACE,
  OBRACK,
  CBRACK,
  COL,
  COMMA,
  DOT,
  SEMCOL,
  SPACE,
  ID,
  STRING,
  COMM,
  NUM
};

const char8 lua_syntax_elements[][16] = {
    "require", "endcode", "endline", "and",   "break",    "do",     "else",
    "elseif",  "end",     "false",   "for",   "function", "goto",   "if",
    "in",      "local",   "nil",     "not",   "or",       "repeat", "return",
    "then",    "true",    "until",   "while", "...",      "..",     "//",
    "==",      "~=",      "<=",      ">=",    "::",       ">>",     "<<",
    "+",       "-",       "*",       "/",     "%",        "^",      "#",
    "<",       ">",       "=",       "&",     "|",        "~",      "(",
    ")",       "{",       "}",       "[",     "]",        ":",      ",",
    ".",       ";",       " ",       "ID",    "STRING",   "COMM",   "NUM",
};

const char8 lua_syntax_element_names[][16] = {
    "REQUIRE", "ENDCODE", "ENDLINE", "AND",       "BREAK",    "DO",
    "ELSE",    "ELSEIF",  "END",     "FALSE",     "FOR",      "FUNCTION",
    "GOTO",    "IF",      "IN",      "LOCAL",     "NIL",      "NOT",
    "OR",      "REPEAT",  "RETURN",  "THEN",      "TRUE",     "UNTIL",
    "WHILE",   "VARARGS", "CONCAT",  "FLOOR",     "EQ",       "NEQ",
    "LTEQ",    "GTEQ",    "COLCOL",  "BITRSHIFT", "BITRLEFT", "ADD",
    "MINUS",   "MULT",    "DIV",     "MOD",       "POW",      "LENGTH",
    "LT",      "GT",      "ASSIGN",  "BITAND",    "BITOR",    "BITNOT",
    "OPAR",    "CPAR",    "OBRACE",  "CBRACE",    "OBRACK",   "CBRACK",
    "COL",     "COMMA",   "DOT",     "SEMCOL",    "SPACE",    "ID",
    "STRING",  "COMM",    "NUM",
};

const uint8 stat_list[] = {
    NOT,
    MINUS,
    LENGTH,
    BITNOT,
};

const uint8 unop_list[] = {
    NOT,
    MINUS,
    LENGTH,
    BITNOT,
};

const MARTe::uint8 binop_list[] = {
    AND,  OR,        CONCAT,   FLOOR, EQ,     NEQ,   LTEQ,
    GTEQ, BITRSHIFT, BITRLEFT, ADD,   MINUS,  MULT,  DIV,
    MOD,  POW,       LT,       GT,    BITAND, BITOR, BITNOT,
};

enum LuaNode {
  BLOCK,
  STAT,
  ATTNAMELIST,
  ATTRIB,
  RETSTAT,
  LABEL,
  FUNCNAME,
  VARLIST,
  VAR,
  NAMELIST,
  EXPLIST,
  EXP,
  PREFIXEXP,
  FUNCTIONCALL,
  ARGS,
  FUNCTIONDEF,
  FUNCBODY,
  PARLIST,
  TABLECONSTRUCTOR,
  FIELDLIST,
  FIELD,
  FIELDSEP,
  BINOP,
  UNOP,
  NAME,
  NUMERAL,
  LITERALSTRING,
  LOCALFUNCTION,
  LOCALSTAT,
  UNDEFINED,
  MODULE,
  VALUE,
  COMMENT,
};

const MARTe::char8 lua_node_names[][32] = {
    "BLOCK",
    "STAT",
    "ATTNAMELIST",
    "ATTRIB",
    "RETSTAT",
    "LABEL",
    "FUNCNAME",
    "VARLIST",
    "VAR",
    "NAMELIST",
    "EXPLIST",
    "EXP",
    "PREFIXEXP",
    "FUNCTIONCALL",
    "ARGS",
    "FUNCTIONDEF",
    "FUNCBODY",
    "PARLIST",
    "TABLECONSTRUCTOR",
    "FIELDLIST",
    "FIELD",
    "FIELDSEP",
    "BINOP",
    "UNOP",
    "NAME",
    "NUMERAL",
    "LITERALSTRING",
    "LOCALFUNCTION",
    "LOCALSTAT",
    "UNDEFINED",
    "MODULE",
    "VALUE",
    "COMMENT",
};

/**
 * @brief Simple token class
 */
struct Token {

  /**
   * @brief Empty construcor
   */
  Token();

  /**
   * @brief Constructor
   * @param[in] str string with token name
   * @param[in] len token name length
   */
  Token(const char8 *str, uint32 len, uint32 row, uint32 col);

  /**
   * @brief Constructor
   * @param[in] str string with token name
   * @param[in] len token name length
   */
  Token(char8 *str, uint32 len, uint32 row, uint32 col);

  /**
   * @brief Destructor
   */
  ~Token();

  /**
   * @brief Print Token characteristics
   */
  Str toString(bool show_type = true, bool show_val = true);

  Str raw;     //!< Token name
  uint32 type; //!< Token type
  uint32 row;  //!< Row number of the token in the lua code
  uint32 col;  //!< Column number of the token in the lua code
  bool unop;   //!< flag to design unary operators
  bool binop;  //!< flag to design binary operators
};

typedef Rc<Token> Tokenp;
typedef List<Tokenp> TokenpList;
struct tokens_t {

  /**
   * @brief Add token to list
   * @param[in] t token
   */
  void add(Tokenp t);

  /**
   * @brief Add token to list
   * @param[in] t token name
   * @param[in] row row in code
   * @param[in] col column in code
   */
  void add(Str t, uint32 row, uint32 col);

  /**
   * @brief Add token to list
   * @param[in] t token name
   * @param[in] row row in code
   * @param[in] col column in code
   * @param[in] len token length
   */
  void add(const char8 *t, uint32 row, uint32 col, uint32 len);

  /**
   * @brief Add token to list
   * @param[in] t token name
   * @param[in] row row in code
   * @param[in] col column in code
   * @param[in] len token length
   */
  void add(char8 *t, uint32 row, uint32 col, uint32 len);

  /**
   * @brief Add long brackets token
   */
  void add_long_bracket_token(strList code_lines, uint32 &line_index,
                              uint32 &line_pos, uint32 &char_pos,
                              bool is_string, bool &ok);

  /**
   * @brief Get token list len
   * @return token list len
   */
  uint32 len();

  /**
   * @brief Print tokens
   */
  void print();

  /**
   * @brief Access token list methods
   **/
  TokenpList *operator->();

  /**
   * @brief Access token list elements
   **/
  Tokenp &operator[](const uint32 &ind) const;

  /**
   * Token list
   */
  TokenpList toks;

  /**
   * Tokens iterator
   */
  typedef TokenpList::iterator iterator;
};

/**
 * @brief Node class for ast
 */
class Node {

public:
  typedef Rc<Node> Nodep;

  /**
   * @brief Empty onstrucor
   */
  Node();

  /**
   * @brief Construcor
   */
  Node(LuaNode n, Tokenp t);

  /**
   * @brief Copy construcor
   */
  Node(LuaNode n);

  /**
   * @brief Destructor
   */
  ~Node();

  /**
   * @brief Add subnode
   */
  void append(Nodep n);

  /**
   * @brief Add subnode
   * @param[in] tok_ token
   * @param[in] type_ node type
   */
  void append(Tokenp tok_, LuaNode type_);

  /**
   * @brief Print node characteristics
   */
  Str toString(uint32 indent = 0, uint32 indent_level = 4,
               bool show_values = false);

  /**
   * @brief Check if the node has a token or subnodes
   * @return true if the token is empty
   */
  bool empty();

  /**
   * @brief Delete subnodes
   */
  void del();

  Tokenp tok;            //!< Token linked to this node
  LuaNode type;          //!< Node type
  List<Nodep> sub_nodes; //!< List of children nodes
};

typedef Rc<Node> Nodep;
typedef List<Nodep> NodepList;
} // namespace LUA
} // namespace MARTe

#endif
