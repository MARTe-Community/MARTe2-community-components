#ifndef LUA_GAM_TEST_H__
#define LUA_GAM_TEST_H__

class LuaGAMTest {
public:
  bool TestEmptyInitialisation();
  bool TestInitialisation();
  bool TestInitialisationWithCode();
  bool TestWrongInitialisationWithCode();
  bool TestSameSignalName();
  bool TestInitMissingGAM();
  bool TestInitReassignVar();
  bool TestInitUnusedSignal();
  bool TestInitOutputUsedBeforeAssignment();
  bool TestInitMissingSignal();
  bool TestInitCodeWithExtraCode();
  bool TestInternals();
  bool TestWrongInternals();
  bool TestExecution();
  bool TestExecOutOfBound();
  bool TestExecWriteInput();
  bool TestExecReadOutput();
  bool TestExecMath();
  bool TestExecTypes();
  bool TestExecWithAuxiliaries();
  bool TestExecWrongAuxiliaries();
  bool TestSimulatorGAM();
};

class LuaParserTest {
public:
  bool TestParserInitialization();
  bool TestParserTokenization();
  bool TestParserTokenizeNumber();
  bool TestParserTokenizeString();
  bool TestParserTokenizeComment();
  bool TestParserBuildBlock();
  bool TestParserDoBlock();
  bool TestParserWhileBlock();
  bool TestParserRepeatBlock();
  bool TestParserFunctionBlock();
  bool TestParserIfBlock();
  bool TestParserForBlock();
  bool TestParserLocalBlock();
  bool TestParserVarlistBlock();
  bool TestParserFunctioncallBlock();
  bool TestParserStringAssignment();
  bool TestParserTable();
  bool TestParserTreeBuilding();
  bool TestParserExpressions();
  bool TestParserStatements();
  bool TestParserDeclarations();
  bool TestParserComments();
  bool TestUnopBinop();
};

#endif
