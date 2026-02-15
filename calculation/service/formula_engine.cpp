#include "formula_engine.h"

EvaluatedFormula FormulaEngine::eval(const QString& f, int w, int h, int qty)
{
    EvaluatedFormula out;
    const QString t = f.trimmed();

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
