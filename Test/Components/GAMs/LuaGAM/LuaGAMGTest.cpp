#include "LuaGAMTest.h"
#include "gtest/gtest.h"

TEST(LuaParser, TestParserInitialization) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserInitialization());
}

TEST(LuaParser, TestParserTokenization) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserTokenization());
}

TEST(LuaParser, TestParserTokenizeNumber) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserTokenizeNumber());
}

TEST(LuaParser, TestParserTokenizeString) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserTokenizeString());
}

TEST(LuaParser, TestParserTokenizeComment) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserTokenizeComment());
}

TEST(LuaParser, TestParserBuildBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserBuildBlock());
}

TEST(LuaParser, TestParserDoBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserDoBlock());
}

TEST(LuaParser, TestParserWhileBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserWhileBlock());
}

TEST(LuaParser, TestParserRepeatBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserRepeatBlock());
}

TEST(LuaParser, TestParserIfBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserIfBlock());
}

TEST(LuaParser, TestParserForBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserForBlock());
}

TEST(LuaParser, TestParserFunctionBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserFunctionBlock());
}

TEST(LuaParser, TestParserLocalBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserLocalBlock());
}

TEST(LuaParser, TestParserVarlistBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserVarlistBlock());
}

TEST(LuaParser, TestParserFunctioncallBlock) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserFunctioncallBlock());
}

TEST(LuaParser, TestParserTreeBuilding) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserTreeBuilding());
}

TEST(LuaParser, TestParserStringAssignment) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserStringAssignment());
}

TEST(LuaParser, TestParserTable) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserTable());
}

TEST(LuaParser, TestParserExpressions) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserExpressions());
}

TEST(LuaParser, TestParserStatements) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserStatements());
}

TEST(LuaParser, TestParserDeclarations) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserDeclarations());
}

TEST(LuaParser, TestParserComments) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestParserComments());
}

TEST(LuaParser, TestUnopBinop) {
  LuaParserTest tester;
  ASSERT_TRUE(tester.TestUnopBinop());
}

TEST(LuaGAM, TestEmptyInitialisation) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestEmptyInitialisation());
}

TEST(LuaGAM, TestInitialisationWithCode) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInitialisationWithCode());
}

TEST(LuaGAM, TestWrongInitialisationWithCode) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestWrongInitialisationWithCode());
}

TEST(LuaGAM, TestSameSignalName) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestSameSignalName());
}

TEST(LuaGAM, TestInitialisation) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInitialisation());
}

TEST(LuaGAM, TestInitMissingGAM) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInitMissingGAM());
}

TEST(LuaGAM, TestInitReassignVar) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInitReassignVar());
}

TEST(LuaGAM, TestInitUnusedSignal) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInitUnusedSignal());
}

TEST(LuaGAM, TestInitOutputUsedBeforeAssignment) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInitOutputUsedBeforeAssignment());
}

TEST(LuaGAM, TestInitMissingSignal) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInitMissingSignal());
}

TEST(LuaGAM, TestInitCodeWithExtraCode) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInitCodeWithExtraCode());
}

TEST(LuaGAM, TestInternals) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestInternals());
}

TEST(LuaGAM, TestWrongInternals) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestWrongInternals());
}

TEST(LuaGAM, TestExecution) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestExecution());
}

TEST(LuaGAM, TestExecOutOfBound) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestExecOutOfBound());
}

TEST(LuaGAM, TestExecWriteInput) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestExecWriteInput());
}

TEST(LuaGAM, TestExecReadOutput) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestExecReadOutput());
}

TEST(LuaGAM, TestExecMath) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestExecMath());
}

TEST(LuaGAM, TestTypes) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestExecTypes());
}

TEST(LuaGAM, TestExecWithAuxiliaries) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestExecWithAuxiliaries());
}

TEST(LuaGAM, TestExecWrongAuxiliaries) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestExecWrongAuxiliaries());
}

TEST(LuaGAM, TestSimulatorGAM) {
  LuaGAMTest tester;
  ASSERT_TRUE(tester.TestSimulatorGAM());
}
