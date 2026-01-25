#include "event_logger_tester.h"

#include <QColor>
#include <QStringList>
#include "common/logger/logger.h"
#include "ui/adapters/log_view_adapter.h"

void EventLoggerTester::testParseLine()
{
    zInfo("→ testParseLine");

    LogViewAdapter adapter(nullptr);

    ParsedLine parsed =
        adapter.parseLine("[2025-11-16T20:23:38.145] ERROR: adatbázis hiba");

    Q_ASSERT(!parsed.timestamp.isEmpty());
    Q_ASSERT(parsed.prefix == "ERROR:");
    Q_ASSERT(parsed.message == "adatbázis hiba");

    zInfo("✓ testParseLine OK");
}

void EventLoggerTester::testColorForPrefix()
{
    zInfo("→ testColorForPrefix");

    LogViewAdapter adapter(nullptr);

    QColor cError = adapter.colorForPrefix("ERROR:");
    QColor cWarn  = adapter.colorForPrefix("WARN:");
    QColor cInfo  = adapter.colorForPrefix("INFO:");

    Q_ASSERT(cError == Qt::red);
    Q_ASSERT(cWarn  == QColor("#d98e00"));
    Q_ASSERT(cInfo  == Qt::darkGreen);

    zInfo("✓ testColorForPrefix OK");
}

bool EventLoggerTester::run()
{
    zInfo(QString("=== %1 TESTS START ===").arg(name()));

    testParseLine();
    testColorForPrefix();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
