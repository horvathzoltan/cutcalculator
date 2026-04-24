#include "log_archiver.h"

#include <QDir>
#include <QDateTime>

#include "common/logger/logger.h"
#include "common/utils/zip/izip_writer.h"

LogArchiver::LogArchiver(const QString& logsDir,
                         const QString& archiveDir,
                         IZipWriter* zipWriter)
    : m_logsDir(logsDir)
    , m_archiveDir(archiveDir)
    , m_zipWriter(zipWriter)
{
}

bool LogArchiver::run()
{
    if (!m_zipWriter) {
        log("❌ No ZIP writer provided");
        return false;
    }

    log("📁 Log directory: " + m_logsDir);
    log("📁 Archive directory: " + m_archiveDir);

    QDir logDir(m_logsDir);
    if (!logDir.exists())
        return false;

    QStringList logs = logDir.entryList(QStringList() << "*.txt",
                                        QDir::Files,
                                        QDir::Time);

    log(QString("📄 Found %1 log files").arg(logs.size()));


    if (logs.size() <= 10) {
        log("ℹ️ Nothing to archive (<= 10 logs)");
        return true;
    }

    QStringList oldLogs = logs.mid(10);
    log(QString("🗑 Selected %1 old logs for archiving").arg(oldLogs.size()));

    QStringList fullPaths;
    fullPaths.reserve(oldLogs.size());
    for (const QString& fileName : oldLogs)
        fullPaths << logDir.filePath(fileName);

    QDir archiveDir(m_archiveDir);
    if (!archiveDir.exists()) {
        if (!archiveDir.mkpath(QStringLiteral(".")))
            return false;
    }

    const QString zipName = QStringLiteral("logs_%1.zip")
                                .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmm")));
    const QString zipPath = archiveDir.filePath(zipName);

    log("📦 Creating ZIP: " + zipPath);

    const bool ok = m_zipWriter->createZip(zipPath, fullPaths);
    if (!ok) {
        log("❌ ZIP creation failed");
        return false;
    }

    log("✅ ZIP created successfully");

    log("🧹 Deleting old logs…");

    for (const QString& fileName : oldLogs)
        logDir.remove(fileName);

    log("🧹 Old logs deleted");


    return true;
}

void LogArchiver::log(const QString& msg) const
{
    zInfo() << "[LogArchiver]" << msg;
}
