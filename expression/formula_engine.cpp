#include <QList>

#include "formula_engine.h"
#include "formula_normalizer.h"

#include "tokenizer.h"
#include "parser.h"
#include "ast_builder.h"
#include "ast_to_expression.h"
#include "expression.h"
#include "variable.h"

// Általános kifejezés: w-10, (w*h)/1000, avg(10,w*h), stb.
static QVariant evalExpressionGeneric(const QString& expr)
{
    auto& vars = VariableRepository::instance();

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

EvaluatedFormula FormulaEngine::eval(const QString& f)
{
    EvaluatedFormula out;
    out.stringValue.clear();

    QString t = FormulaNormalizer::normalizeWhitespace(f);
    t = FormulaNormalizer::stripOuterParens(t);

    if (t.isEmpty()) {
        // üres formula → 0,0
        return out;
    }

    // Többsoros formula: minden sort lefuttatunk, az utolsó eredményét adjuk vissza
    const QStringList lines = t.split('\n', Qt::SkipEmptyParts);
    if (lines.size() > 1) {
        EvaluatedFormula last;
        for (int i = 0; i < lines.size(); ++i) {
            last = eval(lines[i]);
        }
        return last;
    }

    auto& vars = VariableRepository::instance();
    int qty = vars.get("qty").toInt();

    // Változó-hozzárendelés: name = expr
    {
        int eq = t.indexOf('=');
        if (eq > 0 && !t.contains("==") && !t.contains(">=") && !t.contains("<=")) {
            QString name = t.left(eq).trimmed();
            QString rhs  = t.mid(eq + 1).trimmed();

            auto rhsEv = eval(rhs);

            // szám jellegű assignment
            if (rhsEv.length_mm != 0 || rhsEv.pieces != 0) {
                vars.set(name, rhsEv.length_mm);
                out = rhsEv;
                return out;
            }

            // string jellegű assignment
            if (!rhsEv.stringValue.isEmpty()) {
                vars.set(name, rhsEv.stringValue);
                out = rhsEv;
                return out;
            }

            // fallback: Expression‑pipeline
            QVariant v = evalExpressionGeneric(rhs);
            vars.set(name, v);

            bool okNum = false;
            int num = v.toInt(&okNum);
            if (okNum) {
                out.length_mm = num;
                out.pieces    = qty;
            }
            return out;
        }
    }


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

    // --- string literal: "TEXT" ---
    if (t.size() >= 2 && t.front() == '"' && t.back() == '"') {
        out.stringValue = t.mid(1, t.size() - 2);
        out.length_mm = 0;
        out.pieces    = qty;
        return out;
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
            int w = vars.get("w").toInt();
            int h = vars.get("h").toInt();
            int productArea = w * h;
            out.pieces = productArea / area;
        }
        return out;
    }

    // --- choose: DSL ---
    if (t.startsWith("choose:")) {
        return evalChoose(t);
    }

    // --- opt: DSL ---
    if (t.contains("opt:")) {
        return evalWithOpt(t);
    }

    // --- MINDEN MÁS: általános kifejezés az új pipeline-on ---
    QVariant v = evalExpressionGeneric(t);
    bool ok = false;
    int len = v.toInt(&ok);
    if (ok) {
        out.length_mm = len;
        out.pieces    = qty;
    }

    return out;
}

EvaluatedFormula FormulaEngine::evalChoose(const QString& t)
{
    EvaluatedFormula out;

    // 1) choose: prefix levágása
    QString expr = t.mid(QStringLiteral("choose:").size()).trimmed();
    expr = FormulaNormalizer::normalizeWhitespace(expr);
    expr = FormulaNormalizer::stripOuterParens(expr);

    // 2) Tokenizálás
    auto tokens = Tokenizer::tokenize(expr);

    // 3) Megkeressük a top-level ? és : tokeneket
    int depth = 0;
    int qIndex = -1;
    int cIndex = -1;

    for (int i = 0; i < tokens.size(); ++i) {
        const Token& tok = tokens[i];

        if (tok.type == TokenType::LParen) depth++;
        else if (tok.type == TokenType::RParen) depth--;
        else if (tok.type == TokenType::Question && depth == 0) {
            qIndex = i;
        }
        else if (tok.type == TokenType::Colon && depth == 0 && qIndex != -1) {
            cIndex = i;
            break;
        }
    }

    if (qIndex < 0 || cIndex < 0)
        return out;

    // 4) Három token-szelet
    QVector<Token> condTokens  = tokens.mid(0, qIndex);
    QVector<Token> trueTokens  = tokens.mid(qIndex + 1, cIndex - qIndex - 1);
    QVector<Token> falseTokens = tokens.mid(cIndex + 1);

    // 5) Feltétel kiértékelése
    auto condRpn = Parser::toRpn(condTokens);
    AstNode* condAst = AstBuilder::fromRpn(condRpn);
    if (!condAst)
        return out;

    QString condExpr = AstToExpression::generate(condAst);
    Expression e = Expression::Parse(condExpr);
    QVariant v = e.Calculate();

    bool cond = false;
    if (v.typeId() == QMetaType::Bool)
        cond = v.toBool();
    else
        cond = (v.toInt() != 0);

    // 6) Ág kiválasztása
    QVector<Token> branchTokens = cond ? trueTokens : falseTokens;

    // 7) Tokenekből újra string
    QString branch;
    for (const Token& tok : branchTokens)
        branch += tok.text;

    branch = branch.trimmed();

    // 8) Ág kiértékelése → eval()
    return eval(branch);
}



EvaluatedFormula FormulaEngine::evalWithOpt(const QString& t)
{
    EvaluatedFormula out;
    out.stringValue.clear();

    QString expr = FormulaNormalizer::normalizeWhitespace(t);
    expr = FormulaNormalizer::stripOuterParens(expr);

    auto& vars = VariableRepository::instance();
    int qty = vars.get("qty").toInt();

    // 1) Tokenizálás
    auto tokens = Tokenizer::tokenize(expr);

    int total = 0;
    int i = 0;

    while (i < tokens.size()) {
        const Token& tok = tokens[i];

        // 2) opt: blokk felismerése
        if (tok.type == TokenType::Identifier && tok.text == "opt") {
            // opt : flag : value
            if (i + 3 >= tokens.size()) break;

            // opt
            i++; // :
            if (tokens[i].type != TokenType::Colon) break;

            // flag
            i++;
            if (tokens[i].type != TokenType::Identifier) break;
            QString flag = tokens[i].text;

            // :
            i++;
            if (tokens[i].type != TokenType::Colon) break;

            // value expression (lehet több token, zárójelezett, nested opt, stb.)
            i++;
            int start = i;
            int depth = 0;

            while (i < tokens.size()) {
                if (tokens[i].type == TokenType::LParen) depth++;
                else if (tokens[i].type == TokenType::RParen) depth--;
                else if (depth == 0 && tokens[i].type == TokenType::Plus)
                    break;
                i++;
            }

            QVector<Token> valueTokens = tokens.mid(start, i - start);

            bool enabled = vars.get(flag).toBool();
            if (enabled && !valueTokens.isEmpty()) {
                QString valueExpr;
                for (const Token& t : valueTokens)
                    valueExpr += t.text;

                valueExpr = valueExpr.trimmed();
                if (!valueExpr.isEmpty()) {
                    auto ev = eval(valueExpr);
                    total += ev.length_mm;
                }
            }


            // Ha + volt, lépjünk túl rajta
            if (i < tokens.size() && tokens[i].type == TokenType::Plus)
                i++;

            continue;
        }

        // 3) Normál Expression rész
        int start = i;
        int depth = 0;

        while (i < tokens.size()) {
            if (tokens[i].type == TokenType::LParen) depth++;
            else if (tokens[i].type == TokenType::RParen) depth--;
            else if (depth == 0 && tokens[i].type == TokenType::Plus)
                break;
            i++;
        }

        QVector<Token> exprTokens = tokens.mid(start, i - start);

        QString exprStr;
        for (const Token& t : exprTokens)
            exprStr += t.text;

        exprStr = exprStr.trimmed();

        if (!exprStr.isEmpty()) {
            auto ev = eval(exprStr);
            total += ev.length_mm;
        }

        if (i < tokens.size() && tokens[i].type == TokenType::Plus)
            i++;
    }

    out.length_mm = total;
    out.pieces = qty;
    return out;
}

