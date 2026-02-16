#include <QList>

#include "formula_engine.h"
#include "formula_normalizer.h"

EvaluatedFormula FormulaEngine::eval(const QString& f, int w, int h, int qty)
{
    EvaluatedFormula out;
    out.stringValue.clear();

    QString t = FormulaNormalizer::normalizeWhitespace(f);
    t = FormulaNormalizer::stripOuterParens(t);


    // --- ÚJ: szám literál támogatás ---
    bool okNum = false;
    int num = t.toInt(&okNum);
    if (okNum) {
        out.length_mm = num;
        out.pieces = qty;
        return out;
    }

    // --- ÚJ: szám literál (double) támogatás ---
    if (FormulaNormalizer::isNumberLiteral(t)) {
        double dval = t.toDouble();
        out.length_mm = static_cast<int>(dval);
        out.pieces = qty;
        return out;
    }


    // --- ÚJ DSL: choose: ---
    if (t.startsWith("choose:")) {
        return evalChoose(t, w, h, qty);
    }

    // --- ÚJ DSL: opt: ---
    if (t.contains("opt:")) {
        return evalWithOpt(t, w, h, qty);
    }

    // --- ÚJ DSL: len:w-10 ---
    if (t.startsWith("len:w-")) {
        bool ok = false;
        int sub = t.mid(6).toInt(&ok);
        if (ok) {
            out.length_mm = w - sub;
            out.pieces = qty;
        }
        return out;
    }

    // --- ÚJ DSL: len:h-10 ---
    if (t.startsWith("len:h-")) {
        bool ok = false;
        int sub = t.mid(6).toInt(&ok);
        if (ok) {
            out.length_mm = h - sub;
            out.pieces = qty;
        }
        return out;
    }

    // --- ÚJ DSL: qty:fixed:3 ---
    if (t.startsWith("qty:fixed:")) {
        bool ok = false;
        int val = t.mid(QStringLiteral("qty:fixed:").size()).toInt(&ok);
        if (ok) {
            out.pieces = val * qty;
        }
        return out;
    }

    // --- ÚJ DSL: qty:perOrder:2 ---
    if (t.startsWith("qty:perOrder:")) {
        bool ok = false;
        int val = t.mid(QStringLiteral("qty:perOrder:").size()).toInt(&ok);
        if (ok) {
            out.pieces = val;
        }
        return out;
    }

    // --- ÚJ DSL: qty:perArea:1000 ---
    if (t.startsWith("qty:perArea:")) {
        bool ok = false;
        int area = t.mid(QStringLiteral("qty:perArea:").size()).toInt(&ok);
        if (ok && area > 0) {
            int productArea = w * h;
            out.pieces = productArea / area;
        }
        return out;
    }

    // --- VISSZAFELÉ kompatibilitás ---
    if (t.startsWith("w-")) {
        bool ok = false;
        int sub = t.mid(2).toInt(&ok);
        if (ok) {
            out.length_mm = w - sub;
            out.pieces = qty;
        }
        return out;
    }

    if (t.startsWith("h-")) {
        bool ok = false;
        int sub = t.mid(2).toInt(&ok);
        if (ok) {
            out.length_mm = h - sub;
            out.pieces = qty;
        }
        return out;
    }

    if (t.startsWith("fixed:")) {
        bool ok = false;
        int val = t.mid(QStringLiteral("fixed:").size()).toInt(&ok);
        if (ok) {
            out.pieces = val * qty;
        }
        return out;
    }

    if (t == "w*h") {
        out.length_mm = w * h;
        out.pieces = qty;
        return out;
    }

    return out;
}

EvaluatedFormula FormulaEngine::evalChoose(const QString& t, int w, int h, int qty)
{
    EvaluatedFormula out;

    // 1) choose: prefix levágása + normalizálás
    QString expr = t.mid(QStringLiteral("choose:").size()).trimmed();
    expr = FormulaNormalizer::normalizeWhitespace(expr);
    expr = FormulaNormalizer::stripOuterParens(expr);

    // 2) ? és : pozíciók
    int q = expr.indexOf('?');
    int c = expr.indexOf(':', q + 1);
    if (q < 0 || c < 0)
        return out;

    // 3) feltétel normalizálása
    QString cond = expr.left(q);
    cond = FormulaNormalizer::normalizeCondition(cond);

    // 4) ágak
    QString trueBranch = expr.mid(q + 1, c - q - 1).trimmed();
    QString falseBranch = expr.mid(c + 1).trimmed();

    // 5) operátor felismerése
    QString op;
    if (cond.contains(">=")) op = ">=";
    else if (cond.contains("<=")) op = "<=";
    else if (cond.contains("==")) op = "==";
    else if (cond.contains(">"))  op = ">";
    else if (cond.contains("<"))  op = "<";
    else return out;

    // 6) bal és jobb oldal szétválasztása
    auto parts = cond.split(op);
    if (parts.size() != 2)
        return out;

    QString leftExpr  = FormulaNormalizer::stripOuterParens(parts[0]);
    QString rightExpr = FormulaNormalizer::stripOuterParens(parts[1]);

    leftExpr  = FormulaNormalizer::normalizeWhitespace(leftExpr);
    rightExpr = FormulaNormalizer::normalizeWhitespace(rightExpr);

    // 7) kiértékelés
    int leftVal  = eval(leftExpr,  w, h, qty).length_mm;
    int rightVal = eval(rightExpr, w, h, qty).length_mm;

    // 8) összehasonlítás
    bool result = false;
    if (op == ">")  result = (leftVal >  rightVal);
    if (op == "<")  result = (leftVal <  rightVal);
    if (op == ">=") result = (leftVal >= rightVal);
    if (op == "<=") result = (leftVal <= rightVal);
    if (op == "==") result = (leftVal == rightVal);

    // 9) eredmény
    out.stringValue = result ? trueBranch : falseBranch;
    return out;
}


EvaluatedFormula FormulaEngine::evalWithOpt(const QString& t, int w, int h, int qty)
{
    EvaluatedFormula out;
    out.stringValue.clear();

    // 1) normalizálás
    QString expr = FormulaNormalizer::normalizeWhitespace(t);
    expr = FormulaNormalizer::stripOuterParens(expr);

    // 2) tokenizálás
    QStringList tokens = expr.split('+', Qt::SkipEmptyParts);

    int total = 0;

    for (QString tok : tokens) {
        tok = FormulaNormalizer::normalizeWhitespace(tok);

        // 3) opt: token
        if (tok.startsWith("opt:")) {

            // opt:flag:+value
            int first  = tok.indexOf(':');
            int second = tok.indexOf(':', first + 1);
            if (second < 0)
                continue;

            QString flag   = tok.mid(first + 1, second - first - 1).trimmed();
            QString valStr = tok.mid(second + 1).trimmed();

            // érték parse (double is mehet)
            bool ok = false;
            double dval = valStr.toDouble(&ok);
            if (!ok)
                continue;

            // FLAG KEZELÉS – később
            continue;
        }

        // 4) normál rész → eval()
        auto ev = eval(tok, w, h, qty);
        total += ev.length_mm;
    }

    out.length_mm = total;
    out.pieces = qty;
    return out;
}

