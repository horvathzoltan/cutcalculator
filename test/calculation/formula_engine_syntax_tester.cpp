
#include "expression/tokenizer.h"
#include "expression/parser.h"
#include "expression/ast_builder.h"
#include "expression/node_pool.h"
#include "expression/token.h"
#include "common/logger/logger.h"
#include "formula_engine_syntax_tester.h"

bool FormulaEngineSyntaxTester::run()
{
    zInfo("=== FormulaEngine SYNTAX TESTS START ===");

    // Tokenizer
    testTokenizeSimple();
    testTokenizeStringLiteral();
    testTokenizeMultiLine();
    testTokenizeInvalidChar();

    // Parser
    testParseSimpleExpression();
    testParseAssignment();
    testParseTernary();
    testParseNestedTernary();
    testParseOptional();
    testParseInvalidMissingColon();
    testParseInvalidParen();

    // RPN
    testRpnOperatorPrecedence();
    testRpnTernaryStructure();
    testRpnOptionalStructure();

    // AST
    testAstSimple();
    testAstAssignment();
    testAstTernary();
    testAstOptional();

    zInfo("=== FormulaEngine SYNTAX TESTS END ===");
    return true;
}

// ---------------------------------------------------------
// TOKENIZER TESTS
// ---------------------------------------------------------

void FormulaEngineSyntaxTester::testTokenizeSimple()
{
    zInfo("→ testTokenizeSimple");

    auto r = Tokenizer::tokenize("w - 10");
    Q_ASSERT(r.ok);
    Q_ASSERT(r.value.size() >= 3);
}

void FormulaEngineSyntaxTester::testTokenizeStringLiteral()
{
    zInfo("→ testTokenizeStringLiteral");

    auto r = Tokenizer::tokenize("\"HELLO\"");
    Q_ASSERT(r.ok);
    Q_ASSERT(r.value[0].type == TokenType::StringLiteral);
}

void FormulaEngineSyntaxTester::testTokenizeMultiLine()
{
    zInfo("→ testTokenizeMultiLine");

    auto r = Tokenizer::tokenize("a=1\nb=2");
    Q_ASSERT(r.ok);

    int newlineCount = 0;
    for (const Token& t : r.value)
        if (t.type == TokenType::Newline)
            newlineCount++;

    Q_ASSERT(newlineCount == 1);
}


void FormulaEngineSyntaxTester::testTokenizeInvalidChar()
{
    zInfo("→ testTokenizeInvalidChar");

    auto r = Tokenizer::tokenize("a = 1 @ 2");
    Q_ASSERT(!r.ok);
}

// ---------------------------------------------------------
// PARSER TESTS
// ---------------------------------------------------------

void FormulaEngineSyntaxTester::testParseSimpleExpression()
{
    zInfo("→ testParseSimpleExpression");

    auto r = Parser::parse("w - 10");
    Q_ASSERT(r.ok);
}

void FormulaEngineSyntaxTester::testParseAssignment()
{
    zInfo("→ testParseAssignment");

    auto r = Parser::parse("x = w - 10");
    Q_ASSERT(r.ok);
}

void FormulaEngineSyntaxTester::testParseTernary()
{
    zInfo("→ testParseTernary");

    auto r = Parser::parse("(w>10) ? A : B");
    Q_ASSERT(r.ok);
}

void FormulaEngineSyntaxTester::testParseNestedTernary()
{
    zInfo("→ testParseNestedTernary");

    QString cmd = "(w>10) ? ((h>20) ? A : B) : C";
    Result<Parser::ParseResult> r = Parser::parse(cmd);
    Parser::debugDump(r.value, cmd, r.error);

    Q_ASSERT(r.ok);
}

void FormulaEngineSyntaxTester::testParseOptional()
{
    zInfo("→ testParseOptional");

    auto r = Parser::parse("paint ?? 40");
    Q_ASSERT(r.ok);
}

void FormulaEngineSyntaxTester::testParseInvalidMissingColon()
{
    zInfo("→ testParseInvalidMissingColon");

    auto r = Parser::parse("(w>10) ? A");
    Q_ASSERT(!r.ok);
}

void FormulaEngineSyntaxTester::testParseInvalidParen()
{
    zInfo("→ testParseInvalidParen");

    auto r = Parser::parse("(w - 10");
    Q_ASSERT(!r.ok);
}

// ---------------------------------------------------------
// RPN TESTS
// ---------------------------------------------------------

void FormulaEngineSyntaxTester::testRpnOperatorPrecedence()
{
    zInfo("→ testRpnOperatorPrecedence");

    QString cmd = "w - 10 * 2";
    auto r = Parser::parse(cmd);
    Parser::debugDump(r.value, cmd, r.error);

    Q_ASSERT(r.ok);

    // RPN should be: w 10 2 * -
    auto rpn = r.value.rpn;
    Q_ASSERT(rpn.size() == 5);
    // RPN: w(0)  10(1)  2(2)  *(3)  -(4)
    Q_ASSERT(rpn[0].text == "w");
    Q_ASSERT(rpn[1].text == "10");
    Q_ASSERT(rpn[2].text == "2");
    Q_ASSERT(rpn[3].text == "*");
    Q_ASSERT(rpn[4].text == "-");
}

void FormulaEngineSyntaxTester::testRpnTernaryStructure()
{
    zInfo("→ testRpnTernaryStructure");

    auto r = Parser::parse("(w>10) ? A : B");
    Q_ASSERT(r.ok);

    bool found = false;
    for (auto& t : r.value.rpn)
        if (t.type == TokenType::TernaryOp)
            found = true;

    Q_ASSERT(found);
}

void FormulaEngineSyntaxTester::testRpnOptionalStructure()
{
    zInfo("→ testRpnOptionalStructure");

    auto r = Parser::parse("paint ?? 40");
    Q_ASSERT(r.ok);

    bool found = false;
    for (auto& t : r.value.rpn)
        if (t.type == TokenType::OptionalOp)
            found = true;

    Q_ASSERT(found);
}

// ---------------------------------------------------------
// AST TESTS
// ---------------------------------------------------------

void FormulaEngineSyntaxTester::testAstSimple()
{
    zInfo("→ testAstSimple");

    NodePool pool;
    auto pr = Parser::parse("w - 10");
    auto ast = AstBuilder::fromRpn(pr.value.rpn, pool);

    Q_ASSERT(ast.ok);
    Q_ASSERT(ast.value->type == AstNode::Type::Operator);
}

void FormulaEngineSyntaxTester::testAstAssignment()
{
    zInfo("→ testAstAssignment");

    NodePool pool;
    auto pr = Parser::parse("x = w - 10");
    auto ast = AstBuilder::fromRpn(pr.value.rpn, pool);

    Q_ASSERT(ast.ok);
    Q_ASSERT(ast.value->type == AstNode::Type::Assignment);
}

void FormulaEngineSyntaxTester::testAstTernary()
{
    zInfo("→ testAstTernary");

    NodePool pool;
    auto pr = Parser::parse("(w>10) ? A : B");
    auto ast = AstBuilder::fromRpn(pr.value.rpn, pool);

    Q_ASSERT(ast.ok);
    Q_ASSERT(ast.value->type == AstNode::Type::Choose);
}

void FormulaEngineSyntaxTester::testAstOptional()
{
    zInfo("→ testAstOptional");

    NodePool pool;
    auto pr = Parser::parse("paint ?? 40");
    auto ast = AstBuilder::fromRpn(pr.value.rpn, pool);

    Q_ASSERT(ast.ok);
    Q_ASSERT(ast.value->type == AstNode::Type::Opt);
}
