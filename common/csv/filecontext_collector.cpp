#include "filecontext_collector.h"
#include "common/csv/csvimporter.h"
#include "common/logger/event_logger.h"
#include "common/logger/logger.h"
//#include "common/logger/error_bucketizer.h" // új segédosztály a hibák kategorizálására

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

    //const QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    if (_contexts.isEmpty()) {
        zInfo(L("📊 CSV import összefoglaló: nincs begyűjtött kontextus."));
        zEvent(L("📊 CSV import összefoglaló: nincs begyűjtött kontextus."));
        return;
    }

    int totalErrors = 0;
    int totalFilesWithErrors = 0;
    int totalLines = 0;
    int totalOkLines = 0;

    struct FileRow {
        QString file;
        int lines;
        int okLines;
        int errors;
        QString status;
        QString op;
    };
    QVector<FileRow> table;

    // Aggregált hibák: üzenet -> sorok
    QMap<QString, ErrorAggregate> errorBuckets;

    zInfo(L("📊 CSV import összefoglaló (%1 fájl)").arg(_contexts.size()));

    for (const auto& ctx : _contexts) {
        const QString path = ctx._filepath;
        const int lines = ctx._totalLines;
        const int read = ctx._readlines;

        totalLines += lines;

        if (ctx.hasError()) {
            const int errs = ctx._errors.size();
            totalErrors += errs;
            totalFilesWithErrors++;

            int okLines = qMax(0, lines - errs);
            totalOkLines += okLines;

            // Hibák aggregálása
            for (const auto& err : ctx._errors) {
                auto& agg = errorBuckets[err.errorMessage()];
                agg.count++;
                agg.lines.push_back(err.lineIndex());

                // opcionálisan: azonosítók gyűjtése
                agg.barcodes.push_back(err.barcode());
                agg.names.push_back(err.name());
            }


            table.push_back(FileRow{ path, lines, okLines, errs, L("⚠️ Hibás"), ctx._operationName });
        } else {
            totalOkLines += lines;
            table.push_back(FileRow{ path, lines, lines, 0, L("✅ OK"), ctx._operationName });
        }
    }

    // Táblázat
    //zInfo("— Összesítés táblázat —");
    zInfo(L("Sorok | Hibátlan | Hibás | Státusz | Fájl                                   | Művelet"));
    zInfo(L("------------------------------------------------------------------------------------------------"));
    for (const auto& r : table) {
        QString fileCol = r.file;
        if (fileCol.size() > 40) fileCol = "…" + fileCol.right(39);
        zInfo(L("%1 | %2 | %3 | %4 | %5 | %6")
                  .arg(QString::number(r.lines), -5)
                  .arg(QString::number(r.okLines), -8)
                  .arg(QString::number(r.errors), -5)
                  .arg(r.status, -7)
                  .arg(fileCol, -40)
                  .arg(r.op));
    }
    zInfo(L("------------------------------------------------------------------------------------------------"));

    // Hibakategóriák sorlistával
    if (!errorBuckets.isEmpty()) {
        zInfo("— Hibakategóriák —");
        //zInfo(L("------------------------------------------------------------------------------------------------"));
        for (auto it = errorBuckets.constBegin(); it != errorBuckets.constEnd(); ++it) {
            QStringList nums;
            for (int ln : it.value().lines) nums << QString::number(ln);

            QStringList ids;
            for (int i = 0; i < it.value().lines.size(); ++i) {
                const QString& bc = it.value().barcodes[i];
                const QString& nm = it.value().names[i];

                QString idPart;
                if (!bc.isEmpty() && !nm.isEmpty()) {
                    idPart = QString("[%1:%2]").arg(bc).arg(nm);
                } else if (!bc.isEmpty()) {
                    idPart = QString("[%1]").arg(bc);
                } else if (!nm.isEmpty()) {
                    idPart = QString("[%1]").arg(nm);
                }

                if (!idPart.isEmpty()) {
                    ids << QString("%1 %2").arg(it.value().lines[i]).arg(idPart);
                } else {
                    ids << QString::number(it.value().lines[i]);
                }
            }


            QString lineList = ids.isEmpty() ? "" : L(" (Sorok: %1)").arg(ids.join(", "));
            zInfo(L("• %1× %2%3")
                      .arg(it.value().count)
                      .arg(it.key())
                      .arg(lineList));
        }
    }

    // Globális mérleg
    if (totalErrors > 0) {
        zEvent(L("⚠️ CSV import összefoglaló: %1 fájlban %2 hiba történt. Összes sor: %3; hibátlan: %4; hibás: %5.")
                   .arg(totalFilesWithErrors)
                   .arg(totalErrors)
                   .arg(totalLines)
                   .arg(totalOkLines)
                   .arg(totalErrors));
    } else {
        zEvent(L("✅ CSV import összefoglaló: minden fájl hibátlanul beolvasható volt. Összes sor: %1; hibátlan: %2.")
                   .arg(totalLines)
                   .arg(totalOkLines));
    }
}



void FileContextCollector::logSummary_simple() const {
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
                zWarning(QString("    • %1").arg(err.toString()));
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


