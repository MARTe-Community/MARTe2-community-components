#ifndef _LUAGAM_VERIFIER_H__
#define _LUAGAM_VERIFIER_H__

#include "AST.h"
#include "LuaParserBaseTypes.h"

#define GAM_FN "GAM"
namespace MARTe {

namespace LUA {
namespace Verifier {

class LuaGAMValidator {
public:
  LuaGAMValidator(const ast_t &ast);

  bool validate_input_signal(const char8 *name);
  bool validate_output_signal(const char8 *name, uint32 max_lines);
  bool check_variables_initialisation(const char8 **names, const uint32 len);
  bool check_gam();
  bool check_only_gam();

private:
  const ast_t &ast;
  const NodepList variables;
};

} // namespace Verifier
} // namespace LUA
} // namespace MARTe

#endif
