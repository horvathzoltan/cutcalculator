#include "log_view_adapter.h"
#include "colors/model/colorconstants.h"
#include "common/utils/font_utils.h"

LogViewAdapter::LogViewAdapter(QPlainTextEdit* target)
    : _target(target)
{
    FontUtils::applySafeMonospaceFont(target);
}

QString LogViewAdapter::ensureNewline(const QString& s) const {
    return s.endsWith('\n') ? s : s + '\n';
}

ParsedLine LogViewAdapter::parseLine(const QString& line) const {
    ParsedLine result;
    int idx = line.indexOf("] ");
    if (idx != -1) {
        result.timestamp = line.left(idx + 1) + " ";
        QString payload = line.mid(idx + 2);

        int spaceIdx = payload.indexOf(": ");
        if (spaceIdx != -1) {
            result.prefix = payload.left(spaceIdx + 1);
            result.message = payload.mid(spaceIdx + 2);
        } else {
            result.message = payload;
        }
    } else {
        result.message = line;
    }
    return result;
}

QColor LogViewAdapter::colorForPrefix(const QString& prefix) const
{
    if (prefix.startsWith("ERROR:"))
        return ColorConstants::ColorError;

    if (prefix.startsWith("WARN:"))
        return ColorConstants::ColorWarn;

    if (prefix.startsWith("INFO:"))
        return ColorConstants::ColorInfo;

    return ColorConstants::TextBlack;
}

void LogViewAdapter::insertColored(QTextCursor& cursor, const QString& text, const QColor& color) const {
    QTextCharFormat fmt;
    fmt.setForeground(color);
    cursor.insertText(text, fmt);
}

void LogViewAdapter::appendLine(const QString& line) {
    if (!_target) return;
    _target->appendPlainText(ensureNewline(line));
    _target->moveCursor(QTextCursor::End);
    _target->ensureCursorVisible();
}

void LogViewAdapter::appendLines(const QStringList& lines) {
    if (!_target || lines.isEmpty()) return;
    appendLine(lines.join("\n"));
}

void LogViewAdapter::appendColoredLineWithTimestamp(const QString& line) {
    if (!_target) return;

    QTextCursor cursor(_target->document());
    cursor.movePosition(QTextCursor::End);

    ParsedLine parsed = parseLine(line);

    if (!parsed.timestamp.isEmpty()) {
        insertColored(cursor, parsed.timestamp, _target->palette().color(QPalette::Text));
    }

    if (!parsed.prefix.isEmpty()) {
        insertColored(cursor, parsed.prefix + " ", Qt::white);
    }

    insertColored(cursor, ensureNewline(parsed.message), colorForPrefix(parsed.prefix));

    _target->ensureCursorVisible();
}
