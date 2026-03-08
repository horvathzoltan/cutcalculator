#pragma once
#include "test/common/test_module_base.h"

class FormulaEngineSyntaxTester : public TestModuleBase
{
public:
    FormulaEngineSyntaxTester()
        : TestModuleBase("FormulaEngineSyntaxTester")
    {}
    bool run() override;

private:
    // Tokenizer tests
    void testTokenizeSimple();
    void testTokenizeStringLiteral();
    void testTokenizeMultiLine();
    void testTokenizeInvalidChar();

    // Parser tests
    void testParseSimpleExpression();
    void testParseAssignment();
    void testParseTernary();
    void testParseNestedTernary();
    void testParseOptional();
    void testParseInvalidMissingColon();
    void testParseInvalidParen();

    // RPN tests
    void testRpnOperatorPrecedence();
    void testRpnTernaryStructure();
    void testRpnOptionalStructure();

    // AST tests
    void testAstSimple();
    void testAstAssignment();
    void testAstTernary();
    void testAstOptional();
};
