#include "filecontext_collector.h"
#include "common/csv/csvimporter.h"
#include "common/logger/event_logger.h"
#include "common/logger/logger.h"

using namespace CsvImporter;

FileContextCollector& FileContextCollector::instance() {
    static FileContextCollector inst;
    return inst;
}

void FileContextCollector::setEnabled(bool enabled) {
    QMutexLocker lock(&_mutex);
    _enabled = enabled;
}

bool FileContextCollector::isEnabled() const {
    QMutexLocker lock(&_mutex);
    return _enabled;
}

void FileContextCollector::onContextDestroyed(const FileContext& ctx) {
    QMutexLocker lock(&_mutex);
    if (!_enabled) return;
    // Másolat: a FileContext legyen trivially copyable vagy biztosíts copy-t

    _contexts.append(FileContextSnapshot(ctx));
}

void FileContextCollector::logSummary() const {
    QMutexLocker lock(&_mutex);
    if (_contexts.isEmpty()) {
        zInfo("📊 CSV import összefoglaló: nincs begyűjtött kontextus.");
        zEvent("📊 CSV import összefoglaló: nincs begyűjtött kontextus.");
        return;
    }

    zInfo(QString("📊 CSV import összefoglaló (%1 fájl)").arg(_contexts.size()));

    int totalErrors = 0;
    int totalFilesWithErrors = 0;

    for (const auto& ctx : _contexts) {
        const QString path = ctx._filepath;
        if (ctx.hasError()) {
            totalErrors += ctx._errors.size();
            totalFilesWithErrors++;

            QString msg2 = (ctx._readlines!=ctx._totalLines)?
                               L("%1/%2").arg(ctx._readlines).arg(ctx._totalLines):
                               L("%2 sor %1 hiba").arg(ctx._errors.size()).arg(ctx._totalLines);

            if(! ctx._fileError.isEmpty()) msg2+=": "+ctx._fileError;

            zInfo(L("⚠️ %3 - Fájl: %1: %2")
                      .arg(path)
                      .arg(msg2)
                      .arg(ctx._operationName));

            if(!ctx._fileError.isEmpty()){
                zInfo(ctx._fileError);
            }

            // Teljes hibaszöveg audit célra
            for (const auto& err : ctx._errors) {
                zWarning(QString("    • %1").arg(err));
            }
        } else {
            zInfo(L("✅ %3 - Fájl: %1: %2 sor: ok")
                      .arg(path)
                      .arg(ctx._readlines)
                      .arg(ctx._operationName));
        }
    }
    // Globális mérleg
    if (totalErrors > 0) {
        zEvent(QString("⚠️ CSV import összefoglaló: %1 fájlban %2 hiba történt. Részletek a logban.")
                   .arg(totalFilesWithErrors)
                   .arg(totalErrors));
    } else {
        zEvent("✅ CSV import összefoglaló: minden fájl hibátlanul beolvasható volt.");
    }

}

void FileContextCollector::flushSummary() {
    logSummary();
    clear();
}

void FileContextCollector::clear() {
    QMutexLocker lock(&_mutex);
    _contexts.clear();
}

// QVector<FileContext> FileContextCollector::snapshot() const {
//     QMutexLocker lock(&_mutex);
//     return _contexts;
// }

FileContextSnapshot::FileContextSnapshot(const CsvImporter::FileContext &c)
    : _filepath(c.filepath()),
    _operationName(c.operationName()),
_fileError(c.fileError()),
    _totalLines(c.totalLines()),
    _readlines(c.readlines())
{
    for(const auto&a:c.errors()){
        _errors.append(a.toString());
    }
}
