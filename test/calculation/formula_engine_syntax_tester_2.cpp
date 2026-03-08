#include "formula_engine_syntax_tester_2.h"
#include "expression/parser.h"
#include "expression/ast_builder.h"
#include "expression/node_pool.h"
#include "common/logger/logger.h"

bool FormulaEngineSyntaxTester_2::run()
{
    zInfo("=== FormulaEngine SYNTAX TESTS 2 START ===");

    // Optional + ternary combos
    testOptionalInsideTernary();
    testTernaryInsideOptional();
    testOptionalChain();
    testOptionalAndTernaryMixed();

    // Function nesting
    testNestedFunctionsSimple();
    testNestedFunctionsWithOptional();
    testNestedFunctionsWithTernary();
    testDeepFunctionNesting();

    // Combined stress tests
    testFunctionOptionalTernaryCombo();
    testFunctionOptionalTernaryDeep();

    // Invalid syntax
    testInvalidOptionalMissingExpr();
    testInvalidOptionalMissingFlag();
    testInvalidFunctionOptionalMix();

    zInfo("=== FormulaEngine SYNTAX TESTS 2 END ===");
    return true;
}

// ---------------------------------------------------------
// OPTIONAL + TERNARY COMBOS
// ---------------------------------------------------------

void FormulaEngineSyntaxTester_2::testOptionalInsideTernary()
{
    auto r = Parser::parse("(w>10) ? (paint ?? 40) : 0");
    Q_ASSERT(r.ok);

    bool foundOpt = false, foundTer = false;
    for (auto& t : r.value.rpn) {
        if (t.type == TokenType::OptionalOp) foundOpt = true;
        if (t.type == TokenType::TernaryOp)  foundTer = true;
    }
    Q_ASSERT(foundOpt && foundTer);
}

void FormulaEngineSyntaxTester_2::testTernaryInsideOptional()
{
    auto r = Parser::parse("flag ?? (w>10 ? A : B)");
    Q_ASSERT(r.ok);

    bool foundOpt = false, foundTer = false;
    for (auto& t : r.value.rpn) {
        if (t.type == TokenType::OptionalOp) foundOpt = true;
        if (t.type == TokenType::TernaryOp)  foundTer = true;
    }
    Q_ASSERT(foundOpt && foundTer);
}

void FormulaEngineSyntaxTester_2::testOptionalChain()
{
    auto r = Parser::parse("a ?? b ?? c");
    Q_ASSERT(r.ok);

    int optCount = 0;
    for (auto& t : r.value.rpn)
        if (t.type == TokenType::OptionalOp)
            optCount++;

    Q_ASSERT(optCount == 2);
}

void FormulaEngineSyntaxTester_2::testOptionalAndTernaryMixed()
{
    auto r = Parser::parse("a ?? b ? c : d");
    Q_ASSERT(r.ok);

    bool foundOpt = false, foundTer = false;
    for (auto& t : r.value.rpn) {
        if (t.type == TokenType::OptionalOp) foundOpt = true;
        if (t.type == TokenType::TernaryOp)  foundTer = true;
    }
    Q_ASSERT(foundOpt && foundTer);
}

// ---------------------------------------------------------
// FUNCTION NESTING
// ---------------------------------------------------------

void FormulaEngineSyntaxTester_2::testNestedFunctionsSimple()
{
    auto r = Parser::parse("sum(10, mul(2, 3))");
    Q_ASSERT(r.ok);
}

void FormulaEngineSyntaxTester_2::testNestedFunctionsWithOptional()
{
    auto r = Parser::parse("sum(10, mul(2, 3)) ?? 40");
    Q_ASSERT(r.ok);

    bool foundOpt = false;
    for (auto& t : r.value.rpn)
        if (t.type == TokenType::OptionalOp)
            foundOpt = true;

    Q_ASSERT(foundOpt);
}

void FormulaEngineSyntaxTester_2::testNestedFunctionsWithTernary()
{
    auto r = Parser::parse("sum(10, mul(2, 3)) ? A : B");
    Q_ASSERT(r.ok);

    bool foundTer = false;
    for (auto& t : r.value.rpn)
        if (t.type == TokenType::TernaryOp)
            foundTer = true;

    Q_ASSERT(foundTer);
}

void FormulaEngineSyntaxTester_2::testDeepFunctionNesting()
{
    auto r = Parser::parse("sum(avg(1,2), mul(3, add(4,5)))");
    Q_ASSERT(r.ok);
}

// ---------------------------------------------------------
// COMBINED STRESS TESTS
// ---------------------------------------------------------

void FormulaEngineSyntaxTester_2::testFunctionOptionalTernaryCombo()
{
    auto r = Parser::parse("(w>10) ? sum(1,2) ?? 40 : mul(3,4)");
    Q_ASSERT(r.ok);

    bool foundOpt = false, foundTer = false;
    for (auto& t : r.value.rpn) {
        if (t.type == TokenType::OptionalOp) foundOpt = true;
        if (t.type == TokenType::TernaryOp)  foundTer = true;
    }
    Q_ASSERT(foundOpt && foundTer);
}

void FormulaEngineSyntaxTester_2::testFunctionOptionalTernaryDeep()
{
    auto r = Parser::parse("flag ?? (w>10 ? sum(1,2) : mul(3,4))");
    Q_ASSERT(r.ok);

    bool foundOpt = false, foundTer = false;
    for (auto& t : r.value.rpn) {
        if (t.type == TokenType::OptionalOp) foundOpt = true;
        if (t.type == TokenType::TernaryOp)  foundTer = true;
    }
    Q_ASSERT(foundOpt && foundTer);
}

// ---------------------------------------------------------
// INVALID SYNTAX
// ---------------------------------------------------------

void FormulaEngineSyntaxTester_2::testInvalidOptionalMissingExpr()
{
    auto r = Parser::parse("flag ??");
    Q_ASSERT(!r.ok);
}

void FormulaEngineSyntaxTester_2::testInvalidOptionalMissingFlag()
{
    auto r = Parser::parse("?? 40");
    Q_ASSERT(!r.ok);
}

void FormulaEngineSyntaxTester_2::testInvalidFunctionOptionalMix()
{
    auto r = Parser::parse("sum(1,2 ?? 3)");
    Q_ASSERT(!r.ok);
}
