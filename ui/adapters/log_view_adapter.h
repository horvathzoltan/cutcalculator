#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QStringList>
#include <QTextCursor>
#include <QColor>

struct ParsedLine {
    QString timestamp;
    QString prefix;
    QString message;
};

class LogViewAdapter {
    friend bool runEventLoggerTests();  // <-- tesztfüggvény barátként

public:
    explicit LogViewAdapter(QPlainTextEdit* target);

    void appendLine(const QString& line);
    void appendLines(const QStringList& lines);
    void appendColoredLineWithTimestamp(const QString& line);

private:
    QPlainTextEdit* _target;

    QString ensureNewline(const QString& s) const;
    ParsedLine parseLine(const QString& line) const;
    QColor colorForPrefix(const QString& prefix) const;
    void insertColored(QTextCursor& cursor, const QString& text, const QColor& color) const;
};
