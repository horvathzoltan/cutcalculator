#pragma once

#include "test/common/test_module_base.h"

class FormulaEngineSyntaxTester_2 : public TestModuleBase {
public:
    FormulaEngineSyntaxTester_2()
        : TestModuleBase("FormulaEngineSyntaxTester_2")
    {}

    bool run() override;

private:
    // Optional + ternary combos
    static void testOptionalInsideTernary();
    static void testTernaryInsideOptional();
    static void testOptionalChain();
    static void testOptionalAndTernaryMixed();

    // Function nesting
    static void testNestedFunctionsSimple();
    static void testNestedFunctionsWithOptional();
    static void testNestedFunctionsWithTernary();
    static void testDeepFunctionNesting();

    // Combined stress tests
    static void testFunctionOptionalTernaryCombo();
    static void testFunctionOptionalTernaryDeep();

    // Invalid syntax tests
    static void testInvalidOptionalMissingExpr();
    static void testInvalidOptionalMissingFlag();
    static void testInvalidFunctionOptionalMix();
};
