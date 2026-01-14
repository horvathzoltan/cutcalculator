// common/startup/startup_status_manager.cpp
#include "startup_status_manager.h"
#include "common/logger/logger.h"

#include <QMessageBox>
#include <QtGlobal>

#include "common/csv/filecontext_collector.h"

// Alapértelmezett konfigurációk.
int StartupStatusManager::s_previewLimit = 2;
bool StartupStatusManager::s_showLogHint = true;
QString StartupStatusManager::s_humanLogPath = {};

bool StartupStatusManager::handle(const StartupStatus& status)
{
    FileContextCollector::instance().flushSummary();

    // Kritikus hiba: log + GUI + leállás.
    if (!status.isSuccess()) {
        const QString summary = makeErrorSummary(status.errorMessage());
        if (status.isCritical()) {
            zError(summary);
            return false;   // kritikus → leáll
        } else {
            zWarning(summary);
            return true;    // nem kritikus → indulhat tovább
        }
    }

    // Figyelmeztetések: soronként log + összefoglaló GUI.
    if (!status.warnings().isEmpty()) {
        for (const auto& w : status.warnings()) {
            zWarning(w);
        }

        const QString summary = makeWarningSummary(status.warnings());
        QMessageBox::warning(nullptr, QStringLiteral("Figyelmeztetés"), summary);
    }

    // Minden rendben → folytatható.
    return true;
}

void StartupStatusManager::setPreviewLimit(int count)
{
    s_previewLimit = qMax(0, count);
}

void StartupStatusManager::setShowLogHint(bool enabled)
{
    s_showLogHint = enabled;
}

void StartupStatusManager::setHumanLogPath(QString path)
{
    s_humanLogPath = std::move(path);
}

QString StartupStatusManager::makeErrorSummary(const QString& error)
{
    QString summary = QStringLiteral("Az alkalmazás indítása az alábbi hibába ütközött:\n\n")
                      + error;

    if (s_showLogHint) {
        summary += QStringLiteral("\n\nRészletek a log fájlban találhatók");
        if (!s_humanLogPath.isEmpty())
            summary += QStringLiteral(": ") + s_humanLogPath;
        else
            summary += QStringLiteral(".");
    }

    return summary;
}

QString StartupStatusManager::makeWarningSummary(const QStringList& warnings)
{
    QString summary = QStringLiteral("Az alkalmazás elindult, de %1 figyelmeztetés történt.\n\n")
                          .arg(warnings.size());

    // Előnézet (max s_previewLimit sor).
    if (s_previewLimit > 0) {
        const int previewCount = qMin(s_previewLimit, warnings.size());
        if (previewCount > 0) {
            summary += QStringLiteral("Példák:\n");
            for (int i = 0; i < previewCount; ++i) {
                summary += QStringLiteral("• ") + warnings.at(i) + QStringLiteral("\n");
            }
            const int remaining = warnings.size() - previewCount;
            if (remaining > 0) {
                summary += QStringLiteral("+ további %1 figyelmeztetés...\n").arg(remaining);
            }
        }
    }

    if (s_showLogHint) {
        summary += QStringLiteral("\nRészletek a log fájlban találhatók");
        if (!s_humanLogPath.isEmpty())
            summary += QStringLiteral(": ") + s_humanLogPath;
        else
            summary += QStringLiteral(".");
    }

    return summary;
}
