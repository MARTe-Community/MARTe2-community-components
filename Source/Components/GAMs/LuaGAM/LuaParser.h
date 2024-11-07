/**
 * @file LuaParser.h
 * @brief Header file for class Lua Parser
 * @date 13/02/2024
 * @author Andrea Antonione
 * @author Martino Ferrari
 */

#ifndef LUA_PARSER_H
#define LUA_PARSER_H

#include "AST.h"
#include "CompilerTypes.h"
#include "LuaParserBaseTypes.h"
#include "lua.hpp"

namespace MARTe {

namespace LUA {
/**
 * @brief Tokenize Lua code.
 * @param[in] code Lua code
 * @param[out] ok flag for correct tokenization
 * @return list of tokens
 */
tokens_t scan(const char8 *code, bool &ok);

/**
 * @brief Parse the Lua code
 * @param[in] code lua code
 * @param[out] ok flag for correct tokenization
 * @return true if code has no errors.
 */
ast_t parse(const MARTe::char8 *code, bool &ok);
} // namespace LUA
} // namespace MARTe
#endif /* LUA_PARSER_H */
