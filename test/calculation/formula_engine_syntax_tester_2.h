#pragma once

class FormulaEngineSyntaxTester_2 {
public:
    static bool run();

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
