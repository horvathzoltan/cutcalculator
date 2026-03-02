#include "formula_analysis.h"

#include <expression/tokenizer.h>
#include <expression/parser.h>
#include <expression/ast.h>
#include <expression/function_registry.h>
#include <expression/node_pool.h>
#include <expression/ast_builder.h>

// AST bejáró – readVars, writtenVars, calledFunctions gyűjtése
class AstWalker {
public:
    QSet<QString> readVars;
    QSet<QString> writtenVars;
    QSet<QString> calledFunctions;

    void walk(AstNode* n) {
        if (!n) return;

        switch (n->type) {

        case AstNode::Type::Variable:
            readVars.insert(n->value);
            break;

        case AstNode::Type::Assignment:
            // lhs = children[0], rhs = children[1]
            writtenVars.insert(n->value);   // lhs neve
            if (n->children.size() > 0) walk(n->children[0]);
            if (n->children.size() > 1) walk(n->children[1]);
            break;

        case AstNode::Type::Function:
            calledFunctions.insert(n->value);
            for (AstNode* arg : n->children)
                walk(arg);
            break;

        default:
            for (AstNode* c : n->children)
                walk(c);
            break;
        }
    }
};

FormulaAnalysis analyzeFormula(const QString& formula,
                               const FormulaContract& contract)
{
    FormulaAnalysis out;

    //
    // 1) Parser teszt (tokenizer is benne van)
    //
    auto pr = Parser::parse(formula);

    if (!pr.ok) {
        out.errors << "Szintaktikai hiba: " + pr.error;
        return out;
    }


    //
    // 2) AST build teszt
    //
    NodePool pool;

    auto ast = AstBuilder::fromRpn(pr.value.rpn, pool);
    if (!ast.ok) {
        out.errors << "AST build hiba: " + ast.error;
        return out;
    }

    AstNode* root = ast.value;

    if (!root) {
        out.errors << "AST build sikertelen";
        return out;
    }

    //
    // 3) AST bejárás
    //
    AstWalker w;
    w.walk(root);

    out.readVars = w.readVars;
    out.writtenVars = w.writtenVars;
    out.calledFunctions = w.calledFunctions;

    //
    // 4) Szerződés ellenőrzése
    //

    // 4.1 Ismeretlen input változó
    for (const QString& v : out.readVars) {
        if (!contract.allowedInputs.contains(v) &&
            !out.writtenVars.contains(v))
        {
            out.errors << QString("Ismeretlen input változó: '%1'").arg(v);
        }
    }

    // 4.2 Kötelező output hiánya
    for (const QString& req : contract.requiredOutputs) {
        if (!out.writtenVars.contains(req)) {
            out.errors << QString("Kötelező output hiányzik: '%1'").arg(req);
        }
    }

    // 4.3 Felesleges output (warning)
    for (const QString& wv : out.writtenVars) {
        if (!contract.requiredOutputs.contains(wv) &&
            !contract.optionalOutputs.contains(wv))
        {
            out.warnings << QString("Ismeretlen output változó: '%1'").arg(wv);
        }
    }

    // 4.4 Ismeretlen függvény
    for (const QString& fn : out.calledFunctions) {
        if (!FunctionRegistry::instance().hasFn(fn)) {
            out.errors << QString("Ismeretlen függvény: '%1'").arg(fn);
        }
    }

    //
    // 5) Eredmény
    //
    out.ok = out.errors.isEmpty();
    return out;
}

