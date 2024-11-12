#include "AST.h"
#include "AdvancedErrorManagement.h"
#include "ErrorType.h"
#include "LuaParserBaseTypes.h"
#include "Verifier.h"

namespace MARTe {
namespace LUA {
namespace Verifier {

bool check_(Nodep node, const char8 *name, LuaNode node_type, uint32 &row,
            uint32 &col) {
  bool found = false;
  if (node->type == node_type) {
    for (Rc<NodepList::iterator> it = node->sub_nodes.iterate(); it && !found;
         it = it->next()) {
      for (Rc<NodepList::iterator> jt = it->value()->sub_nodes.iterate();
           jt && !found; jt = jt->next()) {
        if ((!jt->value()->tok.isNull()) && (jt->value()->tok->raw == name)) {
          found = true;
          row = jt->value()->tok->row;
          col = jt->value()->tok->col;
        }
      }
    }
  }
  if (!found) {
    for (Rc<NodepList::iterator> it = node->sub_nodes.iterate(); it;
         it = it->next()) {
      if (check_(it->value(), name, node_type, row, col)) {
        found = true;
        break;
      }
    }
  }
  return found;
}

/**
 * @brief Retrieve element from node subnodes
 * @param[in] node node
 * @param[in] type node type
 * @param[out] list list to fill in
 */
void get_(Nodep node, LuaNode type, NodepList &list) {
  if (node && (node->type == type) && (node->sub_nodes.len() == 1) &&
      node->sub_nodes[0] && (!node->sub_nodes[0]->tok.isNull()) &&
      (node->sub_nodes[0]->tok->raw != NULL_PTR(char8 *))) {
    list.append(node);
  }
  for (Rc<NodepList::iterator> it = node->sub_nodes.iterate(); it;
       it = it->next()) {
    get_(it->value(), type, list);
  }
}

NodepList ExtractVariables(const ast_t &ast) {
  NodepList variables;
  for (Rc<NodepList::iterator> it = ast.nodes.iterate(); it; it = it->next()) {
    get_(it->value(), VAR, variables);
  }
  return variables;
}

bool Check(const ast_t &ast, const char8 *id, LuaNode type, uint32 &row,
           uint32 &col) {

  bool found = false;
  for (Rc<NodepList::iterator> it = ast.nodes.iterate(); it; it = it->next()) {
    if (check_(it->value(), id, type, row, col)) {
      found = true;
      break;
    }
  }
  return found;
}

LuaGAMValidator::LuaGAMValidator(const ast_t &ast)
    : ast(ast), variables(ExtractVariables(ast)) {}

bool LuaGAMValidator::validate_input_signal(const char8 *name) {
  bool ok = true;
  uint32 var_line, var_col; // dummies
  if (!Check(ast, name, EXP, var_line, var_col)) {
    REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                        "Input signal `%s` is not used.", name);
    ok = false;
  }
  if (Check(ast, name, VARLIST, var_line, var_col)) {
    REPORT_ERROR_STATIC(
        ErrorManagement::InitialisationError,
        "[Line:%i, Col:%i] Input signal `%s` is being reassigned.", var_line,
        var_col, name);
    ok = false;
  }
  if (Check(ast, name, LOCALSTAT, var_line, var_col)) {
    REPORT_ERROR_STATIC(
        ErrorManagement::Warning,
        "[Line:%i, Col:%i] Input signal `%s` is being reassigned as local.",
        var_line, var_col, name);
    ok = false;
  }
  return ok;
}

bool LuaGAMValidator::validate_output_signal(const char8 *name,
                                             uint32 max_lines) {
  bool ok = true;

  uint32 var_assign_line = max_lines;
  uint32 var_assign_col = var_assign_line;
  if (!Check(ast, name, VARLIST, var_assign_line, var_assign_col)) {
    REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                        "Output signal `%s` is not assigned.", name);
    ok = false;
  }
  uint32 var_use_line;
  uint32 var_use_col;
  if (Check(ast, name, EXP, var_use_line, var_use_col)) {
    if ((var_use_line < var_assign_line) ||
        ((var_use_line == var_assign_line) && (var_use_col < var_assign_col))) {
      REPORT_ERROR_STATIC(ErrorManagement::Warning,
                          "[Line:%i, Col:%i] Output signal `%s` is used before "
                          "assignment. Signals initial value is 0.",
                          var_use_line, var_use_col, name);
    }
  }
  uint32 var_local_line;
  uint32 var_local_col;
  if (Check(ast, name, LOCALSTAT, var_local_line, var_local_col)) {
    REPORT_ERROR_STATIC(
        ErrorManagement::InitialisationError,
        "[Line:%i, Col:%i] Output signal `%s` is being reassigned "
        "as local.",
        var_local_line, var_local_col, name);
    ok = false;
  }
  return ok;
}

const Str &var_name(Rc<NodepList::iterator> it) {
  return it->value()->sub_nodes[0]->tok->raw;
}

bool LuaGAMValidator::check_variables_initialisation(const char8 **names,
                                                     const uint32 len) {
  bool ok = true;
  for (Rc<NodepList::iterator> it = variables.iterate(); it; it = it->next()) {
    ok = false;
    for (uint32 i = 0; i < len; i++) {
      if (var_name(it) == names[i]) {
        ok = true;
        break;
      }
    }
    if (!ok) {
      REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                          "Variable `%s` is not initialized.",
                          var_name(it).cstr());
      break;
    }
  }
  return ok;
}

bool LuaGAMValidator::check_gam() {
  bool ok = false;
  if (ast.nodes.len() != 1) {
    REPORT_ERROR_STATIC(
        ErrorManagement::InitialisationError,
        "AST should contain only one node, instead it contains %d",
        ast.nodes.len());
    return false;
  }
  for (Rc<NodepList::iterator> it = ast[0]->sub_nodes.iterate(); it;
       it = it->next()) {
    if (it->value()->type == STAT &&
        it->value()->sub_nodes[0]->type == FUNCNAME &&
        it->value()->sub_nodes[0]->tok->raw == GAM_FN) {
      ok = true;
      break;
    }
  }
  if (!ok) {
    REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                        "No main `" GAM_FN "` function found");
  }
  return ok;
}
bool LuaGAMValidator::check_only_gam() {
  bool ok = ast[0]->sub_nodes.len() == 1;
  if (!ok) {
    REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                        "External code found outside `" GAM_FN "` function");
  }
  return ok;
}

} // namespace Verifier
} // namespace LUA
} // namespace MARTe
