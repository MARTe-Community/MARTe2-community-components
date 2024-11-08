#include "Error.h"
#include "FastMath.h"
#include "Logging.h"
#include "LuaGAM.h"
#include "LuaGAMTest.h"
#include "LuaParser.h"
#include "TestMacros.h"
#include "Utils.h"
#include "dbutils.h"
#include "lua.hpp"
#include <execinfo.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define LOG(...) log::info(__VA_ARGS__);

#define DB_TEST "dbtest"

#define DEBUG_LOG(...)                                                         \
  printf("DEBUG_LOG [%s:%d]\t", __FILE__, __LINE__);                           \
  printf(__VA_ARGS__);

#define DBG(...) DEBUG_LOG("DBG");

/**
  Simple command mapper class that expose internal memory
**/
class LuaFriend : public MARTe::LuaGAM {
public:
  LuaFriend() : MARTe::LuaGAM() {}
  void *input_pointer(MARTe::uint32 i) { return this->GetInputSignalMemory(i); }
  void *output_pointer(MARTe::uint32 i) {
    return this->GetOutputSignalMemory(i);
  }
};

bool TestScanning(const char *code, const Str expeted_tokens[],
                  const uint32 len) {
  bool ok = true;
  T_ASSERT_TRUE(ok);
  LUA::tokens_t toks;
  try {
    toks = LUA::scan(code, ok);
  } catch (Error &e) {
    DEBUG_LOG("Test error:%s\n", e.what())
    ok = false;
  }
  T_ASSERT_TRUE(ok);
  T_ASSERT_EQ(toks.len() - 1, len)

  for (MARTe::uint32 i = 0; i < toks.len() - 1; i++) {
    T_ASSERT_EQ(toks[i]->raw, expeted_tokens[i]);
  }
  return ok;
}

#define SCANTEST(code_str, ...)                                                \
  const char *code = code_str;                                                 \
  Str tokens[] = {__VA_ARGS__};                                                \
  uint32 len = sizeof(tokens) / sizeof(Str);                                   \
  return TestScanning(code, tokens, len);

bool LuaParserTest::TestParserInitialization() {
  SCANTEST("function GAM(a)\n"
           "  test = 1\n"
           "end\n",
           "function", "GAM", "(", "a", ")", "test", "=", "1", "end");
}

bool LuaParserTest::TestParserTokenization() {
  SCANTEST(
      "abcd+z\n"
      "a  --comment test with spaces to check if it works\n"
      "\" string with space in the middle\"\n"
      "\' string with single quote\'\n"
      "do print while(1)\n",
      "abcd", "+", "z", "a", "--comment test with spaces to check if it works",
      "\" string with space in the middle\"", "\' string with single quote\'",
      "do", "print", "while", "(", "1", ")");
}

bool LuaParserTest::TestParserTokenizeNumber() {
  SCANTEST("3\n"
           "345\n"
           "0xff\n"
           "0xBEBADA\n"
           "3.0\n"
           "3.1416\n"
           "314.16e-2\n"
           "0.31416E1\n"
           "34e1\n"
           "0x0.1E\n"
           "0xA23p-4\n"
           "0X1.921FB54442D18P+1\n",
           "3", "345", "0xff", "0xBEBADA", "3.0", "3.1416", "314.16e-2",
           "0.31416E1", "34e1", "0x0.1E", "0xA23p-4", "0X1.921FB54442D18P+1");
}

bool LuaParserTest::TestParserTokenizeString() {
  SCANTEST("\'astr1\\n123\\\'\'\n"
           "\"astr2\\n456\\\"\"\n"
           "\'\\97str3\\10\\04923\"\'\n"
           "[[astr4789\"]]\n"
           "[[astr5\n123\"]]\n"
           "[=[astr6\n12\n3\"]=]\n"
           "[===[astr7\n12\n3\"]===]\n"
           "[==[\nastr8\n\n123\"]==]\n"
           "[===[astr9123\"]===]\n"
           "[=[[a\n\n]=]\n",
           "\'astr1\\n123\\\'\'", "\"astr2\\n456\\\"\"",
           "\'\\97str3\\10\\04923\"\'", "[[astr4789\"]]", "[[astr5\n123\"]]",
           "[=[astr6\n12\n3\"]=]", "[===[astr7\n12\n3\"]===]",
           "[==[\nastr8\n\n123\"]==]", "[===[astr9123\"]===]", "[=[[a\n\n]=]");
}

bool LuaParserTest::TestParserTokenizeComment() {
  SCANTEST("-- short comment\n"
           "--[ short comment\n"
           "-- [[ short comment\n"
           "--[[long comment]]\n"
           "--[=[long comment]=]\n"
           "--[===[long comment]===]\n"
           "--[[long\ncomment]]\n"
           "--[==[long\ncomment]==]\n"
           "--\n",
           "-- short comment", "--[ short comment", "-- [[ short comment",
           "--[[long comment]]", "--[=[long comment]=]",
           "--[===[long comment]===]", "--[[long\ncomment]]",
           "--[==[long\ncomment]==]", "--");
}
bool TestParse(const char *code) {
  bool ok = true;
  LUA::ast_t ast;
  try {
    ast = LUA::parse(code, ok);
  } catch (Error &e) {
    DEBUG_LOG("Test error:%s\n", e.what())
    ok = false;
  }
  T_ASSERT_TRUE(ok);
  return ok;
}

bool LuaParserTest::TestParserBuildBlock() {
  return TestParse(";\n"
                   "::lab::\n"
                   "goto lab\n"
                   "break\n");
}

bool LuaParserTest::TestParserDoBlock() {
  bool ok = TestParse("do\n"
                      "  ;\n"
                      "end\n");
  const char *code2 = "do\n"
                      "  ;\n";
  LUA::parse(code2, ok);
  T_ASSERT_FALSE(ok);
  return !ok;
}

bool LuaParserTest::TestParserWhileBlock() {
  return TestParse("while true do ; end\n"
                   "while false do ::a:: end\n");
}

bool LuaParserTest::TestParserRepeatBlock() {
  return TestParse("repeat ; until true\n");
}

bool LuaParserTest::TestParserIfBlock() {
  return TestParse("if true then ; elseif false then ; else ; end\n");
}

bool LuaParserTest::TestParserForBlock() {
  return TestParse("for a = true, true, false do ; end\n"
                   "for a,b in true,false do ; end\n");
}

bool LuaParserTest::TestParserFunctionBlock() {
  return TestParse("function f.g:h(b,c,...) ; end\n");
}

bool read_line(char *line, FILE *f, uint32 &len) {

  memset(line, 0, 256);
  len = 0;
  while (len < 256) {
    char ch = (char)fgetc(f);
    line[len++] = ch;
    if (ch == '\n') {
      if (len < 256) {
        line[len] = 0;
      }
      return true;
    }
    if (ch == EOF) {
      return false;
    }
  }
  return false;
}

bool startswith(const char *str, const char *pattern) {
  for (uint32 i = 0; pattern[i] != 0; i++) {
    if (str[i] == 0 || pattern[i] != str[i]) {
      return false;
    }
  }
  return true;
}

char *to_tag(FILE *f, const char *delim, bool &ok) {
  char *str = new char[256];
  memset(str, 0, 256);
  uint32 str_size = 256;
  uint32 str_len = 0;
  uint32 len = 0;
  char line[256];
  while (read_line(line, f, len)) {
    if (startswith(line, delim)) {
      if (str_len < str_size) {
        str[str_len] = 0;
      }
      ok = true;
      return str;
    } else {
      if (len + str_len >= str_size) {
        char *buff = new char[len + str_len + 1];
        memset(buff, 0, len + str_len + 1);
        strcpy(buff, str);
        delete[] str;
        str = new char[len + str_len + 128];
        str_size = len + str_len + 128;
        memset(str, 0, str_size);
        strcpy(str, buff);
        delete[] buff;
      }
      strcat(str, line);
      str_len += len;
    }
  }
  ok = false;
  return str;
}

List<Str> tolines(Str s) {
  List<Str> lines;
  uint32 i0 = 0;
  for (uint32 i = 0; i < s.len(); i++) {
    if (s[i] == '\n') {
      lines.append(s.substr(i0, i));
      i0 = i + 1;
    }
  }
  if (i0 != s.len()) {
    lines.append(s.substr(i0, s.len()));
  }
  return lines;
}

Str diff(const char *a, const char *b, uint32 tab = 0) {
  char *spaces = new char[tab * 4 + 1];
  memset(spaces, ' ', tab * 4);
  spaces[tab * 4] = 0;
  Str res;
  List<Str> line_a = tolines(a);
  List<Str> line_b = tolines(b);
  uint32 line = 0;
  for (List<Str>::iterator *it = line_a.iterate(); it; it = it->next()) {
    line++;
    if (!line_b.contains(it->value())) {
      char num[10];
      sprintf(num, "%d", line);
      res = res + spaces + "\e[32mexp:" + num + ">" + it->value() + "\e[0m\n";
    }
  }
  line = 0;
  char *div = new char[(tab + 2) * 4 + 2];
  memset(div, ' ', tab * 4);
  memset(&div[tab * 4], '=', 8);
  div[(tab + 2) * 4] = '\n';
  div[(tab + 2) * 4 + 1] = 0;
  res = res + div;
  for (List<Str>::iterator *it = line_b.iterate(); it; it = it->next()) {
    line++;
    if (!line_a.contains(it->value())) {
      char num[10];
      sprintf(num, "%d", line);
      res = res + spaces + "\e[31mres:" + num + ">" + it->value() + "\e[0m\n";
    }
  }

  delete[] spaces;
  delete[] div;
  return res;
}

bool run_corpus_test(const uint32 test_i, const char *path, const char *desc,
                     const char *code, const char *ast) {
  bool ok = true;
  LUA::ast_t ast_ = LUA::parse(code, ok);
  if (!ok || ast_[0]->empty()) {
    ok = false;
    printf("  [\e[31mFAIL   \e[0m] to parser test %s[%d]: `%s`\n", path, test_i,
           desc);
  } else {
    Str ast_computed;
    for (LUA::NodepList::iterator *it = ast_->iterate(); it; it = it->next()) {
      ast_computed = ast_computed + it->value()->toString(0, 4, true);
    }
    ok = strcmp(ast_computed.cstr(), ast) == 0;
    if (!ok) {
      printf("  [\e[31mFAIL   \e[0m] test %s[%d] `%s`: AST DID NOT MATCH\n",
             path, test_i, desc);
      printf("%s", diff(ast, ast_computed.cstr(), 1).cstr());
    } else {
      printf("  [\e[32mSUCCESS\e[0m] test %s[%d] `%s`\n", path, test_i, desc);
    }
  }

  return ok;
}

bool test_corpus(const char *path) {
  FILE *f = fopen(path, "r");
  T_ASSERT_TRUE(f != NULL_PTR(FILE *));
  char line[256];
  uint32 i = 0;
  uint32 len = 0;
  bool res = true;
  while (read_line(line, f, len)) {
    if (startswith(line, "==== TEST ====")) {
      i++;
      bool ok = true;
      char *desc = to_tag(f, "==== CODE ====", ok);
      if (!ok) {
        delete[] desc;
        T_LOG("Corpus: %s, test %d: failed to get description\n", path, i);
        return false;
      }
      desc[strlen(desc) - 1] = 0;
      char *code = to_tag(f, "==== AST  ====", ok);
      if (!ok) {
        T_LOG("Corpus: %s, test %d `%s`: failed to get code\n", path, i, desc);
        delete[] desc;
        delete[] code;
        return false;
      }
      char *ast = to_tag(f, "==== DONE ====", ok);
      if (!ok) {
        T_LOG("Corpus: %s, test %d `%s`: failed to get ast\n", path, i, desc);
        delete[] desc;
        delete[] code;
        delete[] ast;
        return false;
      }
      if (!run_corpus_test(i, path, desc, code, ast)) {
        res = false;
      }
      delete[] desc;
      delete[] code;
      delete[] ast;
    }
  }
  fclose(f);
  return res;
}

bool LuaParserTest::TestParserExpressions() {
  try {
    T_ASSERT_TRUE(test_corpus("./Test/Resources/LuaGAM/luaparser_exp.corpus"));
  } catch (ExtendedError &e) {
    printf("Error at %s:%d: '%s'\n", e.file(), e.line(), e.msg());
    return false;
  }
  return true;
}

bool LuaParserTest::TestParserStatements() {
  try {
    T_ASSERT_TRUE(test_corpus("./Test/Resources/LuaGAM/luaparser_stat.corpus"));
  } catch (ExtendedError &e) {
    printf("Error at %s:%d: '%s'\n", e.file(), e.line(), e.msg());
    return false;
  }
  return true;
}

bool LuaParserTest::TestParserDeclarations() {
  try {
    T_ASSERT_TRUE(test_corpus("./Test/Resources/LuaGAM/luaparser_decl.corpus"));
  } catch (ExtendedError &e) {
    printf("Error at %s:%d: '%s'\n", e.file(), e.line(), e.msg());
    return false;
  }
  return true;
}

bool LuaParserTest::TestParserComments() {
  try {
    T_ASSERT_TRUE(test_corpus("./Test/Resources/LuaGAM/luaparser_comm.corpus"));
  } catch (ExtendedError &e) {
    printf("Error at %s:%d: '%s'\n", e.file(), e.line(), e.msg());
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    return false;
  }
  return true;
}

bool LuaParserTest::TestParserLocalBlock() {
  return TestParse("local function x(a,b,...) ; end\n"
                   "local a<b>,c<d>\n"
                   "local e<f>,g<h> = true,false\n");
}

bool LuaParserTest::TestParserVarlistBlock() {
  return TestParse("a,b = true,false\n");
}

bool LuaParserTest::TestParserFunctioncallBlock() {
  return TestParse("a(c,d)\n");
}

bool LuaParserTest::TestParserStringAssignment() {
  return TestParse("function test()\n"
                   "  b = \"b\"\n"
                   "  y = x + 1\n"
                   "  y12 = x + 1\n"
                   "end\n");
}

bool LuaParserTest::TestParserTable() {
  return TestParse("function GAM()\n"
                   "  table = {a=1, \"b\", c}\n"
                   "  table = {\n"
                   "            a=1,\n"
                   "            b\n"
                   "          }\n"
                   "  c = table.a\n"
                   "  b = table[\"b\" ]\n"
                   "  z = {y[1]+1, x-1, y[2]*2}\n"
                   "end\n");
}

bool LuaParserTest::TestParserTreeBuilding() {
  return TestParse(
      "function GAM()\n"                           // function definition
      "  output_sign = input_sign + 1 + sqrt(1)\n" // assignment
      "  if output_sign>sqrt(3) then\n"    // if statement with binary operator
      "    print(output_sign)\n"           // function call
      "  elseif output_sign<=(1+2) then\n" // else if statement with binary
                                           // operator
      "    print(1)\n"                     // function call
      "  else\n"                           // else statement
      "    print(2) --random comment\n"
      "  end\n"
      "  do\n"         // do block statement
      "    print(3)\n" // function call
      "  end\n"
      "end\n"
      "local x\n"    // local definition
      "::label::\n"  // label definition
      "goto label\n" // goto statement
      "repeat\n"     // repeat until statement
      "  print(a) \n"
      "until 1\n"
      "while a~=1 do\n" // while do statement
      "  a = a-5\n"
      "end\n");
}

bool LuaParserTest::TestUnopBinop() {
  return TestParse("a = 3 - 2\n"

                   "b = -a\n"
                   "c = a ~ b\n"
                   "d = ~c\n");
}

bool LuaGAMTest::TestEmptyInitialisation() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_FALSE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_FALSE(luagam.Setup());
  return true;
}

bool LuaGAMTest::TestInitialisation() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "function GAM()\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.Setup());
  T_ASSERT_FALSE(luagam.IsCodeExternal());
  T_ASSERT_TRUE(luagam.CodePath().empty());
  return ok;
}

bool LuaGAMTest::TestInitialisationWithCode() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  char filepath[] = "file://./Test/Resources/LuaGAM/testfile.lua";
  MARTe::GAMDB::set_parameter(db, "Code", filepath);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.Setup());
  T_ASSERT_TRUE(luagam.IsCodeExternal());
  T_ASSERT_FALSE(luagam.CodePath().empty());
  return ok;
}

bool LuaGAMTest::TestWrongInitialisationWithCode() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  char filepath[] = "file://./Test/Resources/LuaGAM/no_file.lua";
  remove(filepath);
  MARTe::GAMDB::set_parameter(db, "Code", filepath);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_FALSE(luagam.Initialise(db));
  return ok;
}

bool LuaGAMTest::TestSameSignalName() {

  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  char code[] = "function GAM()\n"
                " X = math.cos(X)\n"
                "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::GAMDB::add_input(db, "X", "float32", "DDB1");
  MARTe::GAMDB::add_output(db, "X", "float32", "DDB1");
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_FALSE(luagam.Setup());
  return ok;
}

bool addInternalState(MARTe::ConfigurationDatabase &db, const char *name,
                      const char *value) {
  bool ok = true;
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(db.MoveAbsolute("InternalStates"));
  T_ASSERT_TRUE(db.Write(name, value));
  db.MoveToRoot();
  return ok;
}

void printInternalStates(MARTe::ConfigurationDatabase &db) {
  db.MoveToRoot();
  db.MoveAbsolute("InternalStates");
  for (uint32 i = 0u; i < db.GetNumberOfChildren(); i++) {
    db.MoveToChild(i);
    DB::print(db, db.GetName(), "    ");
    db.MoveToAncestor(1u);
  }
  db.MoveToRoot();
}

bool LuaGAMTest::TestInternals() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "function GAM() glob1=glob2 end";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  T_ASSERT_TRUE(db.CreateAbsolute("InternalStates"));
  T_ASSERT_TRUE(addInternalState(db, "glob1", "1.5"));
  T_ASSERT_TRUE(addInternalState(db, "glob2", "1"));
  db.MoveToRoot();
  // printInternalStates(db);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.Setup());
  return ok;
}

bool LuaGAMTest::TestWrongInternals() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "function GAM()\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  T_ASSERT_TRUE(db.CreateAbsolute("InternalStates"));
  T_ASSERT_TRUE(addInternalState(db, "glob1", "1a"));
  db.MoveToRoot();
  // printInternalStates(db);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_FALSE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_FALSE(luagam.Setup());
  return ok;
}

bool LuaGAMTest::TestInitMissingGAM() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "function test()\n"
                     "  y = x + 1\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::GAMDB::add_input(db, "x", "float32", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "float32", DB_TEST);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_FALSE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_FALSE(luagam.Setup());
  return ok;
}

bool LuaGAMTest::TestInitReassignVar() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "function GAM()\n"
                     "  local y = x + 1\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::GAMDB::add_input(db, "x", "float32", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "float32", DB_TEST);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_FALSE(luagam.Setup());
  return ok;
}

bool LuaGAMTest::TestInitUnusedSignal() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "function GAM()\n"
                     "  y = x + 1\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);

  MARTe::GAMDB::add_input(db, "x", "float32", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "float32", DB_TEST);
  MARTe::GAMDB::add_output(db, "z", "float32", DB_TEST);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_FALSE(luagam.Setup());
  return ok;
}

bool LuaGAMTest::TestInitOutputUsedBeforeAssignment() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "function GAM()\n"
                     "  y = x + 1\n"
                     "  x = x + 1\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::GAMDB::add_output(db, "x", "float32", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "float32", DB_TEST);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.Setup());
  return ok;
}

bool LuaGAMTest::TestInitMissingSignal() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "function GAM()\n"
                     "  y = x + 1\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::GAMDB::add_input(db, "x", "float32", DB_TEST);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.Setup()); // TODO: check for global unused var
  return ok;
}

bool LuaGAMTest::TestInitCodeWithExtraCode() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  const char *code = "local t = 10\n"
                     "function GAM()\n"
                     "  y = x + t\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::GAMDB::add_input(db, "x", "float32", DB_TEST);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_FALSE(luagam.Setup());
  return ok;
}

bool LuaGAMTest::TestExecution() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "int32", DB_TEST, 1);
  MARTe::GAMDB::add_input(db, "y", "int32", DB_TEST, 1, 2);
  MARTe::GAMDB::add_output(db, "z", "int32", DB_TEST, 1, 3);
  const char *code = "function GAM()\n"
                     "  z = {y[1]+1, x-1, y[2]*2}\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_TRUE(luagam.Setup());
  MARTe::int32 *x = (MARTe::int32 *)luagam.input_pointer(0);
  MARTe::int32 *y = (MARTe::int32 *)luagam.input_pointer(1);
  MARTe::int32 *z = (MARTe::int32 *)luagam.output_pointer(0);
  y[0] = 42;
  y[1] = 43;
  int N = 100;
  MARTe::uint64 duration = utils::utime();
  for (int i = 0; i < N; i++) {
    *x = i;
    T_ASSERT_TRUE(luagam.Execute());
    T_ASSERT_EQ(z[0], y[0] + 1);
    T_ASSERT_EQ(z[1], *x - 1);
    T_ASSERT_EQ(z[2], y[1] * 2);
  }
  duration = utils::utime() - duration;
  LOG("Total time %llu us\n", duration);
  LOG("Cycle time %f us\n", (double)(duration) / N);
  return ok;
}

bool LuaGAMTest::TestExecOutOfBound() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "int32", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "uint8", DB_TEST);
  const char *code = "function GAM()\n"
                     "  y = x + 1\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_TRUE(luagam.Setup());
  MARTe::int32 *x = (MARTe::int32 *)luagam.input_pointer(0);
  MARTe::uint8 *y = (MARTe::uint8 *)luagam.output_pointer(0);
  const int N_TEST = 6;
  int vals[N_TEST] = {0, -1, -100, 128, 255, 1024};
  for (int i = 0; i < N_TEST; i++) {
    *x = vals[i];
    int exp = vals[i] + 1;
    if (exp >= 0 && exp < 256) {
      T_ASSERT_TRUE(luagam.Execute());
      T_ASSERT_EQ(*y, (MARTe::uint8)exp);
    } else {
      T_ASSERT_FALSE(luagam.Execute());
    }
  }
  return ok;
}

bool LuaGAMTest::TestExecWriteInput() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "int32", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "int32", DB_TEST);
  const char *code_ = "function GAM()\n"
                      "  print(y)\n"
                      "  y = x + 1\n"
                      "  print(x)\n"
                      "  x = 0\n"
                      "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code_);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_FALSE(luagam.Setup());
  return ok;
}

bool LuaGAMTest::TestExecReadOutput() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "int32", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "int32", DB_TEST);
  const char *code = "function GAM()\n"
                     "  print(y)\n"
                     "  if (y > 0) then\n"
                     "    y = x\n"
                     "  else\n"
                     "    y = -x\n"
                     "  end\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_TRUE(luagam.Setup());
  MARTe::int32 *x = (MARTe::int32 *)luagam.input_pointer(0);
  MARTe::int32 *y = (MARTe::int32 *)luagam.output_pointer(0);
  int val = 10;
  *x = val;
  // LOG("y before gam %d\n", *y);
  T_ASSERT_TRUE(luagam.Execute());
  // LOG("x set into gam %d\n", *x);
  // LOG("y after gam %d\n", *y);
  T_ASSERT_EQ(*y, -val);
  *x = -2;
  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(*y, -*x);
  *x = 10;
  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(*y, *x);
  return ok;
}

bool LuaGAMTest::TestExecMath() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "float32", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "float32", DB_TEST);
  const char *code = "function GAM()\n"
                     "  y = math.cos(x*math.pi*2.0);\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_TRUE(luagam.Setup());
  MARTe::float32 *x = (MARTe::float32 *)luagam.input_pointer(0);
  MARTe::float32 *y = (MARTe::float32 *)luagam.output_pointer(0);
  for (int i = 0; i < 100; i++) {
    *x = i / 5.0;
    T_ASSERT_TRUE(luagam.Execute());
    T_ASSERT_NEAR(*y, FastMath::Cos(*x * 2 * FastMath::PI), 1e-4);
  }
  return ok;
}

template <typename T> bool testIntegerType(const char *name, T t_min, T t_max) {
  MARTe::int64 integer_min = (MARTe::int64)t_min;
  MARTe::int64 integer_max = (MARTe::int64)t_max;
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "int64", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", name, DB_TEST);
  const char *code = "function GAM()\n"
                     "  y = x;"
                     "end\n";

  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_TRUE(luagam.Setup());
  MARTe::int64 *x = (MARTe::int64 *)luagam.input_pointer(0);
  T *y = (T *)luagam.output_pointer(0);
  *x = 0;
  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(*y, 0);

  *x = (integer_max + integer_min) / 2 + 1;
  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(*y, (T)((integer_min + integer_max) / 2 + 1));

  *x = integer_max;
  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(*y, (T)integer_max);

  *x = integer_min;
  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(*y, (T)integer_min);

  if (integer_min > LUA_MAX_INTEGER) {
    *x = integer_min - 1;
    T_ASSERT_FALSE(luagam.Execute());
  }
  if (integer_max < -LUA_MAX_INTEGER) {
    *x = integer_max + 1;
    T_ASSERT_FALSE(luagam.Execute());
  }
  return ok;
}

template <typename T> bool testFloatType(const char *name) {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "float64", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", name, DB_TEST);
  const char *code = "function GAM()\n"
                     "  y = x;"
                     "end\n";

  MARTe::GAMDB::set_parameter(db, "Code", code);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_TRUE(luagam.Setup());
  double *x = (double *)luagam.input_pointer(0);
  T *y = (T *)luagam.output_pointer(0);
  const int N = 8;
  const double vals[N] = {
      0.0, 2, 10, 10000, 1.41512e-9, FastMath::PI, 9.3412141e9, FastMath::E,
  };
  for (int i = 0; i < N; i++) {
    *x = vals[i];
    T_ASSERT_TRUE(luagam.Execute());
    if (fabs((*y - vals[i]) / vals[i]) > 1e-3) {
      LOG("Failed %d: %f != %f\n", i, *y, vals[i]);
      return false;
    }
  }
  return ok;
}

#define testInt(type, min, max) testIntegerType<type>(#type, min, max)
#define testUInt(type, max) testIntegerType<type>(#type, 0, max)
#define testFloat(type) testFloatType<type>(#type)

bool LuaGAMTest::TestExecTypes() {
  using namespace MARTe;
  bool ok = true;
  ok = testInt(int8, INT8_MIN, INT8_MAX) & ok;
  ok = testInt(int16, INT16_MIN, INT16_MAX) & ok;
  ok = testInt(int32, INT32_MIN, INT32_MAX) & ok;
  ok = testInt(int64, -LUA_MAX_INTEGER, LUA_MAX_INTEGER) & ok;
  ok = testUInt(uint8, UINT8_MAX) & ok;
  ok = testUInt(uint16, UINT16_MAX) & ok;
  ok = testUInt(uint32, UINT32_MAX) & ok;
  ok = testUInt(uint64, LUA_MAX_INTEGER) & ok;
  ok = testFloat(float32) & ok;
  ok = testFloat(float64) & ok;
  return ok;
}

bool addAuxiliaryFunction(MARTe::ConfigurationDatabase &db, const char *name,
                          const char *function) {
  bool ok = true;
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(db.MoveAbsolute("AuxiliaryFunctions"));
  T_ASSERT_TRUE(db.Write(name, function));
  db.MoveToRoot();
  return ok;
}

bool LuaGAMTest::TestExecWithAuxiliaries() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "float64", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "float64", DB_TEST);
  const char *code = "function GAM()\n"
                     "  y = helper(x)\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  const char *auxiliary_code = "function helper(input)\n"
                               "  return math.cos(input)\n"
                               "end\n";
  T_ASSERT_TRUE(db.CreateRelative("AuxiliaryFunctions"));
  db.MoveRelative("AuxiliaryFunctions");
  T_ASSERT_TRUE(addAuxiliaryFunction(db, "helper", auxiliary_code));
  db.MoveToAncestor(1u);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_TRUE(luagam.Setup());
  MARTe::float64 *x = (MARTe::float64 *)luagam.input_pointer(0);
  MARTe::float64 *y = (MARTe::float64 *)luagam.output_pointer(0);
  *x = 0;
  LOG("x:%f\n", *x)
  LOG("y:%f\n", *y)
  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(*y, 1);
  LOG("x:%f\n", *x)
  LOG("y:%f\n", *y)
  return ok;
}

bool LuaGAMTest::TestExecWrongAuxiliaries() {
  bool ok = true;
  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  MARTe::GAMDB::add_input(db, "x", "float64", DB_TEST);
  MARTe::GAMDB::add_output(db, "y", "float64", DB_TEST);
  const char *code = "function GAM()\n"
                     "  y = help(x)\n"
                     "end\n";
  MARTe::GAMDB::set_parameter(db, "Code", code);
  const char *auxiliary_code = "function help(input)\n"
                               "  return test(input)\n";
  T_ASSERT_TRUE(db.CreateAbsolute("AuxiliaryFunctions"));
  T_ASSERT_TRUE(addAuxiliaryFunction(db, "help", auxiliary_code));
  db.MoveToRoot();
  LUA::parse(auxiliary_code, ok);
  T_ASSERT_FALSE(ok);
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_FALSE(luagam.Initialise(db));
  return ok;
}

bool LuaGAMTest::TestSimulatorGAM() {
  bool ok = true;

  LuaFriend luagam;
  MARTe::ConfigurationDatabase db = MARTe::GAMDB::create();
  char filepath[] = "file://./Test/Resources/LuaGAM/testGAM.lua";
  MARTe::GAMDB::set_parameter(db, "Code", filepath);
  MARTe::GAMDB::add_input(db, "Ramp_cmd", "uint8", DB_TEST);
  MARTe::GAMDB::add_input(db, "Hold_cmd", "uint8", DB_TEST);
  MARTe::GAMDB::add_input(db, "Current_sp", "float32", DB_TEST);
  MARTe::GAMDB::add_input(db, "Sweep_rate", "float32", DB_TEST);
  MARTe::GAMDB::add_output(db, "State", "uint8", DB_TEST);
  MARTe::GAMDB::add_output(db, "Ramp_done", "uint8", DB_TEST);
  MARTe::GAMDB::add_output(db, "Current", "float32", DB_TEST);
  T_ASSERT_TRUE(db.CreateAbsolute("InternalStates"));
  addInternalState(db, "last_time", "0");
  MARTe::ConfigurationDatabase cdb = MARTe::GAMDB::make_cdb(db, ok);
  T_ASSERT_TRUE(ok);
  T_ASSERT_TRUE(luagam.Initialise(db));
  T_ASSERT_TRUE(luagam.SetConfiguredDatabase(cdb));
  T_ASSERT_TRUE(luagam.AllocateInputSignalsMemory());
  T_ASSERT_TRUE(luagam.AllocateOutputSignalsMemory());
  T_ASSERT_TRUE(luagam.Setup());

  uint8 &ramp_cmd = *(uint8 *)luagam.input_pointer(0);
  uint8 &hold_cmd = *(uint8 *)luagam.input_pointer(1);
  float32 &current_sp = *(float32 *)luagam.input_pointer(2);
  float32 &sweep_rate = *(float32 *)luagam.input_pointer(3);
  uint8 &state = *(uint8 *)luagam.output_pointer(0);
  uint8 &ramp_done = *(uint8 *)luagam.output_pointer(1);
  current_sp = 0.1;
  sweep_rate = 50000.0;
  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(state, 0u);
  ramp_cmd = 1u;
  T_ASSERT_TRUE(luagam.Execute());
  ramp_cmd = 0u;
  T_ASSERT_EQ(state, 1u);
  usleep(1000);

  T_ASSERT_TRUE(luagam.Execute());
  T_ASSERT_EQ(state, 3u);
  T_ASSERT_EQ(ramp_done, 1u);
  // MARTe::float32 *x = (MARTe::float32 *)luagam.input_pointer(0);
  // MARTe::float32 *y = (MARTe::float32 *)luagam.output_pointer(0);
  return ok;
}
