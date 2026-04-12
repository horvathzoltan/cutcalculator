#pragma once
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTimer>

class DslSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit DslSyntaxHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QTextCharFormat fmtKeyword;
    QTextCharFormat fmtVariable;
    QTextCharFormat fmtNumber;
    QTextCharFormat fmtOperator;
    QTextCharFormat fmtError;

    QStringList _errors;
    QStringList _errorTokens;
    QHash<QString, QString> _errorMap;
public:
    const QHash<QString, QString>& errorMap() const { return _errorMap; }

public:
    void setErrors(const QStringList& errs) {
        _errors = errs;
        _errorMap.clear();

        QRegularExpression re("'([^']+)'");

        for (const QString& err : errs) {
            auto m = re.match(err);
            if (m.hasMatch()) {
                QString tok = m.captured(1);
                _errorMap[tok] = err;   // token → teljes hibaüzenet
            }
        }

        // CRASH FIX:
        QTimer::singleShot(0, this, [this]() { rehighlight(); });

    }



};
