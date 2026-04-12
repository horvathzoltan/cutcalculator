#include "dsl_syntax_highlighter.h"
#include <QRegularExpression>

DslSyntaxHighlighter::DslSyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    fmtKeyword.setForeground(QColor("#0077cc"));
    fmtVariable.setForeground(QColor("#009977"));
    fmtNumber.setForeground(QColor("#cc7700"));
    fmtOperator.setForeground(QColor("#555555"));
    fmtError.setForeground(Qt::red);
}

void DslSyntaxHighlighter::highlightBlock(const QString& text)
{
    QRegularExpression reKeyword("\\b(len|qty|mat)\\b");
    QRegularExpression reVar("\\b(w|h|qty|handler|owner|color|externalId|product)\\b");
    QRegularExpression reNum("\\b\\d+\\b");
    QRegularExpression reOp("[:\\+\\-\\?]");

    auto apply = [&](const QRegularExpression& re, const QTextCharFormat& fmt) {
        auto it = re.globalMatch(text);
        while (it.hasNext()) {
            auto m = it.next();
            setFormat(m.capturedStart(), m.capturedLength(), fmt);
        }
    };

    apply(reKeyword, fmtKeyword);
    apply(reVar, fmtVariable);
    apply(reNum, fmtNumber);
    apply(reOp, fmtOperator);

    _errorTokens.clear();
    for (const QString& err : _errors) {
        QRegularExpression re("'([^']+)'");
        auto m = re.match(err);
        if (m.hasMatch())
            _errorTokens << m.captured(1);
    }

    for (const QString& tok : _errorTokens) {
        QRegularExpression reTok("\\b" + QRegularExpression::escape(tok) + "\\b");
        auto it = reTok.globalMatch(text);
        while (it.hasNext()) {
            auto m = it.next();
            QTextCharFormat f = fmtError;
            f.setUnderlineStyle(QTextCharFormat::WaveUnderline);
            setFormat(m.capturedStart(), m.capturedLength(), f);
        }
    }


}
