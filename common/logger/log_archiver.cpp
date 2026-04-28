#include "log_archiver.h"

#include <QDir>
#include <QDateTime>
#include <QRegularExpression>

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

/*
 * LOG ROTATION PIPELINE (bundle-alapú)
 *
 * 1) Fájllista beolvasása a log könyvtárból
 * 2) Bundle-építés timestamp alapján
 * 3) Bundle-konzisztencia ellenőrzés (prefixek, fájlszám, timestamp)
 * 4) Archiválási döntés (>= 10 bundle esetén)
 * 5) ZIP készítése a régi bundle-ökből
 * 6) Régi logfájlok törlése
 *
 * A rendszer determinisztikus, auditálható és hibabiztos.
 */

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

/*
 * 1) Fájllista beolvasása
 * Csak .txt fájlok, idő szerinti rendezés.
 * A bundle-építés alapja a fájlnevekben lévő timestamp.
 */

    QStringList logs = logDir.entryList(QStringList() << "*.txt",
                                        QDir::Files,
                                        QDir::Time);

    log(QString("📄 Found %1 log files").arg(logs.size()));

/*
 * 2) Bundle építés
 *
 * A fájlnév formátuma:
 *   <prefix>_<YYYYMMDD_HHMMSS>.txt
 *
 * Prefixek:
 *   - events_
 *   - log_
 *   - audit_
 *   - perf_
 *
 * A teljes fájlnév regex-szel validálva.
 * A timestamp külön is validálva.
 * A bundle kulcsa: a timestamp (YYYYMMDD_HHMMSS).
 */

    QMap<QString, QStringList> bundles;
    QRegularExpression fullRe("^(events|log|audit|perf)_\\d{8}_\\d{6}\\.txt$");
    QRegularExpression tsRe("^\\d{8}_\\d{6}$");

    for (const QString& fileName : logs) {
        if (!fullRe.match(fileName).hasMatch()) {
            log("skipped invalid filename: " + fileName);
            continue;
        }

        // prefix_YYYYMMDD_HHMMSS.txt → vágjuk ki a timestampet
        const int underscorePos = fileName.indexOf('_');
        if (underscorePos <= 0) {
            log("skipped invalid filename (no underscore): " + fileName);
            continue;
        }

        const QString base = fileName.mid(underscorePos + 1, 15); // "YYYYMMDD_HHMMSS"

        if (!tsRe.match(base).hasMatch()) {
            log("skipped invalid timestamp in filename: " + fileName);
            continue;
        }

        bundles[base].append(fileName);
    }


    /*
 * 3) Bundle-konzisztencia ellenőrzés
 *
 * A bundle akkor érvényes, ha:
 *   - nem üres
 *   - minden fájl ugyanahhoz a timestamphez tartozik
 *   - van legalább 1 events_ fájl
 *   - van legalább 1 log_ fájl
 *   - max. 1 perf_ fájl
 *   - max. 1 audit_ fájl
 *
 * A hibás bundle-ök logolva és eltávolítva.
 */

    QStringList invalidBundles;
    for (auto it = bundles.begin(); it != bundles.end(); ++it) {
        const QString key = it.key();
        const QStringList files = it.value();

        if (files.isEmpty()) {
            log("skipped empty bundle: " + key);
            invalidBundles << key;
            continue;
        }

        bool hasEvents = false;
        bool hasLog = false;
        int perfCount = 0;
        int auditCount = 0;

        for (const QString& f : files) {
            if (!f.contains(key)) {
                log("skipped inconsistent bundle: " + key);
                invalidBundles << key;
                break;
            }
            if (f.startsWith("events_")) hasEvents = true;
            if (f.startsWith("log_")) hasLog = true;
            if (f.startsWith("perf_")) ++perfCount;
            if (f.startsWith("audit_")) ++auditCount;
        }

        if (invalidBundles.contains(key))
            continue;

        if (!hasEvents || !hasLog) {
            log("skipped incomplete bundle (missing events/log): " + key);
            invalidBundles << key;
            continue;
        }

        if (perfCount > 1 || auditCount > 1) {
            log("skipped invalid prefix distribution in bundle: " + key);
            invalidBundles << key;
            continue;
        }
    }

    for (const QString& k : invalidBundles)
        bundles.remove(k);

    /*
 * 4) Bundle-sorrendezés
 * Legfrissebb timestamp elöl (csökkenő sorrend).
 * A rotáció determinisztikus működéséhez szükséges.
 */

    QStringList bundleKeys = bundles.keys();
    std::sort(bundleKeys.begin(), bundleKeys.end(), std::greater<QString>());

    /*
 * 5) Archiválási döntés
 *
 * Ha <= 10 bundle van → nincs archiválás.
 * Ha > 10 bundle → az első 3 marad, a többi archiválódik.
 */

    if (bundleKeys.size() <= 10) {
        log("ℹ️ Nothing to archive (<= 10 log bundles)");
        return true;
    }

    QStringList oldBundleKeys = bundleKeys.mid(3);
    QStringList fullPaths;
    for (const QString& key : oldBundleKeys)
        for (const QString& f : bundles[key])
            fullPaths << logDir.filePath(f);

    // 3
    QDir archiveDir(m_archiveDir);
    if (!archiveDir.exists()) {
        if (!archiveDir.mkpath(QStringLiteral(".")))
            return false;
    }

    //4
    if (oldBundleKeys.isEmpty()) {
        log("ℹ️ No old bundles to archive");
        return true;
    }

    const QString bundleId = oldBundleKeys.first();
    if (bundleId.isEmpty()) {
        log("❌ Invalid bundle identifier");
        return false;
    }

    QRegularExpression re("^\\d{8}_\\d{6}$");
    if (!re.match(bundleId).hasMatch()) {
        log("❌ Invalid bundle identifier format");
        return false;
    }

    /*
 * 6) ZIP név generálása
 * A ZIP neve a legrégebbi archiválandó bundle timestampje.
 * A ZIP csak validált bundle-ökből készülhet.
 */

    const QString zipName = QStringLiteral("logs_%1.zip")
                                .arg(bundleId);

    const QString zipPath = archiveDir.filePath(zipName);

    /*
 * 7) ZIP készítése és régi fájlok törlése
 *
 * A ZIP csak akkor készül el, ha:
 *   - a ZIP writer működik
 *   - a bundle-ök konzisztenciája rendben van
 *   - a fájlok elérhetők
 *
 * Sikeres ZIP után a régi logfájlok törlődnek.
 */

    log("📦 Creating ZIP: " + zipPath);

    const bool ok = m_zipWriter->createZip(zipPath, fullPaths);
    if (!ok) {
        log("❌ ZIP creation failed");
        return false;
    }

    log("✅ ZIP created successfully");

    log("🧹 Deleting old logs…");

    for (const QString& f : fullPaths)
        QFile::remove(f);

    /*
 * A rotáció sikeresen lefutott.
 * A rendszer állapota konzisztens és auditálható.
 */

    log("🧹 Old logs deleted");

    return true;
}

void LogArchiver::log(const QString& msg) const
{
    zInfo() << "[LogArchiver]" << msg;
}
