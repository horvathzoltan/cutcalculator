#pragma once

#include "ui/adapters/log_view_adapter.h"
#include <QStringList>
#include <QColor>
#include <QDebug>

// Teszt futtató függvény
// --test eventlogger
inline bool runEventLoggerTests() {
    bool ok = true;

    // Dummy QPlainTextEdit nélkül tesztelünk → nullptr
    LogViewAdapter adapter(nullptr);

    // 1. parseLine teszt
    ParsedLine parsed = adapter.parseLine("[2025-11-16T20:23:38.145] ERROR: adatbázis hiba");
    if (parsed.timestamp.isEmpty() || parsed.prefix != "ERROR:" || parsed.message != "adatbázis hiba") {
        qWarning() << "❌ parseLine FAILED:" << parsed.timestamp << parsed.prefix << parsed.message;
        ok = false;
    } else {
        qInfo() << "✅ parseLine PASSED";
    }

    // 2. colorForPrefix teszt
    QColor cError = adapter.colorForPrefix("ERROR:");
    QColor cWarn  = adapter.colorForPrefix("WARN:");
    QColor cInfo  = adapter.colorForPrefix("INFO:");
    if (cError != Qt::red || cWarn != QColor("#d98e00") || cInfo != Qt::darkGreen) {
        qWarning() << "❌ colorForPrefix FAILED";
        ok = false;
    } else {
        qInfo() << "✅ colorForPrefix PASSED";
    }

    return ok;
}
