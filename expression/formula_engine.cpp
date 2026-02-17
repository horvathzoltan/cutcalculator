#include <QList>

#include "formula_engine.h"
#include "formula_normalizer.h"
#include "variable‎.h"

#include "tokenizer.h"
#include "parser.h"
#include "ast_builder.h"
#include "ast_to_expression.h"
#include "expression.h"

// Általános kifejezés: w-10, (w*h)/1000, avg(10,w*h), stb.
static QVariant evalExpressionGeneric(const QString& expr, int w, int h, int qty)
{
    auto& vars = VariableRepository::instance();
    vars.set("w",   w);
    vars.set("h",   h);
    vars.set("qty", qty);

    auto tokens = Tokenizer::tokenize(expr);
    auto rpn    = Parser::toRpn(tokens);
    AstNode* root = AstBuilder::fromRpn(rpn);
    if (!root)
        return {};

    QString exprStr = AstToExpression::generate(root);
    // pl.: div(mul($w,$h),1000) vagy mul(sub($w,10),$h)

    Expression e = Expression::Parse(exprStr);
    return e.Calculate();
}

EvaluatedFormula FormulaEngine::eval(const QString& f, int w, int h, int qty)
{
    EvaluatedFormula out;
    out.stringValue.clear();

    QString t = FormulaNormalizer::normalizeWhitespace(f);
    t = FormulaNormalizer::stripOuterParens(t);

    // --- szám literál (int) ---
    {
        bool okNum = false;
        int num = t.toInt(&okNum);
        if (okNum) {
            out.length_mm = num;
            out.pieces    = qty;
            return out;
        }
    }

    // --- szám literál (double) ---
    if (FormulaNormalizer::isNumberLiteral(t)) {
        bool ok = false;
        double dval = t.toDouble(&ok);
        if (ok) {
            out.length_mm = static_cast<int>(dval);
            out.pieces    = qty;
            return out;
        }
    }

    // --- qty: DSL-ek ---

    // qty:fixed:N  → pieces = N * qty
    if (t.startsWith("qty:fixed:")) {
        bool ok = false;
        int val = t.mid(QStringLiteral("qty:fixed:").size()).toInt(&ok);
        if (ok) {
            out.pieces = val * qty;
        }
        return out;
    }

    // qty:perOrder:N → pieces = N
    if (t.startsWith("qty:perOrder:")) {
        bool ok = false;
        int val = t.mid(QStringLiteral("qty:perOrder:").size()).toInt(&ok);
        if (ok) {
            out.pieces = val;
        }
        return out;
    }

    // qty:perArea:A → pieces = (w*h)/A
    if (t.startsWith("qty:perArea:")) {
        bool ok = false;
        int area = t.mid(QStringLiteral("qty:perArea:").size()).toInt(&ok);
        if (ok && area > 0) {
            int productArea = w * h;
            out.pieces = productArea / area;
        }
        return out;
    }


    // --- choose: DSL (egyelőre marad a régi implementáció) ---
    if (t.startsWith("choose:")) {
        return evalChoose(t, w, h, qty);
    }

    // --- opt: DSL (egyelőre marad a régi implementáció) ---
    if (t.contains("opt:")) {
        return evalWithOpt(t, w, h, qty);
    }

    // --- MINDEN MÁS: általános kifejezés az új pipeline-on ---
    QVariant v = evalExpressionGeneric(t, w, h, qty);
    bool ok = false;
    int len = v.toInt(&ok);
    if (ok) {
        out.length_mm = len;
        out.pieces    = qty;
    }

    return out;
}

EvaluatedFormula FormulaEngine::evalChoose(const QString& t, int w, int h, int qty)
{
    EvaluatedFormula out;

    // 1) "choose:" levágása
    QString expr = t.mid(QStringLiteral("choose:").size()).trimmed();
    expr = FormulaNormalizer::normalizeWhitespace(expr);
    expr = FormulaNormalizer::stripOuterParens(expr);

    // 2) ? és : pozíciók
    int q = expr.indexOf('?');
    int c = expr.indexOf(':', q + 1);
    if (q < 0 || c < 0)
        return out;

    QString condStr  = expr.left(q).trimmed();
    QString trueStr  = expr.mid(q + 1, c - q - 1).trimmed();
    QString falseStr = expr.mid(c + 1).trimmed();

    // 3) Feltétel tokenizálása
    auto condTokens = Tokenizer::tokenize(condStr);

    // 4) RPN
    auto condRpn = Parser::toRpn(condTokens);

    // 5) AST
    AstNode* condAst = AstBuilder::fromRpn(condRpn);
    if (!condAst)
        return out;

    // 6) Expression string generálás
    QString condExpr = AstToExpression::generate(condAst);
    // pl.: "mul($w,$h)>=1000"

    // 7) Expression kiértékelése → bool
    Expression e = Expression::Parse(condExpr);
    QVariant v = e.Calculate();

    bool cond = false;
    if (v.typeId() == QMetaType::Bool)
        cond = v.toBool();
    else
        cond = (v.toInt() != 0);

    // 8) A megfelelő ág kiértékelése
    QString branch = cond ? trueStr : falseStr;

    return eval(branch, w, h, qty);
}

EvaluatedFormula FormulaEngine::evalWithOpt(const QString& t, int w, int h, int qty)
{
    EvaluatedFormula out;
    out.stringValue.clear();

    QString expr = FormulaNormalizer::normalizeWhitespace(t);
    expr = FormulaNormalizer::stripOuterParens(expr);

    // 1) Tokenizálás
    auto tokens = Tokenizer::tokenize(expr);

    // 2) RPN
    auto rpn = Parser::toRpn(tokens);

    // 3) AST
    AstNode* root = AstBuilder::fromRpn(rpn);
    if (!root)
        return out;

    // 4) AST → Expression string
    QString exprStr = AstToExpression::generate(root);

    // 5) Expression::Parse
    Expression e = Expression::Parse(exprStr);

    // 6) Expression::Calculate
    QVariant v = e.Calculate();

    // 7) Eredmény
    bool ok = false;
    int len = v.toInt(&ok);
    if (ok) {
        out.length_mm = len;
        out.pieces    = qty;
    }

    return out;
}

