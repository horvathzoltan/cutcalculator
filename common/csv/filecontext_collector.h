#pragma once
/**
 * @file filecontext_collector.h
 * @brief Globális gyűjtő és összegző a CSV beolvasási FileContext-ek számára.
 *
 * Cél:
 * - Minden CsvImporter::FileContext példány destruktora automatikusan értesíti ezt a gyűjtőt.
 * - A gyűjtő egy másolatot tárol (nem ownership), így az eredeti példányok szabadon megsemmisülhetnek.
 * - A végén audit-barát összefoglaló készül a teljes feldolgozásról (fájlok, hibaszámok, részletek).
 *
 * Használat:
 * - A CsvImporter::FileContext destruktora hívja: FileContextCollector::instance().onContextDestroyed(ctx).
 * - A startup végén hívd: FileContextCollector::instance().logSummary(); majd clear().
 *
 * Szálbiztonság:
 * - Mutex védi a belső konténert; párhuzamos beolvasásnál is konzisztens állapot.
 */

//#include "common/csv/csvimporter.h"
#include <QVector>
#include <QMutex>
#include <QString>

namespace CsvImporter { struct FileContext; struct RowError; }

// struct RowErr2{
//     int _lineIndex;
//     QString _errorMessage;
// };

struct FileContextSnapshot {
    QString _filepath;
    QString _operationName;
    QString _fileError;
    /*QVector<CsvImporter::RowError>*/
    QStringList _errors;
    int _totalLines;
    int _readlines = 0;

    explicit FileContextSnapshot(const CsvImporter::FileContext& c);

    bool hasError() const{
        if(!_fileError.isEmpty()) return true;
        if(!_errors.isEmpty()) return true;
        if(_totalLines!=_readlines) return true;
        return false;
    }
};

class FileContextCollector {
public:
    // Singleton elérés
    static FileContextCollector& instance();

    // Engedélyezés/tiltás (pl. teszt módban vagy performance okokból)
    void setEnabled(bool enabled);
    bool isEnabled() const;

    // Observer callback: FileContext destruktora hívja
    void onContextDestroyed(const CsvImporter::FileContext& ctx);

    // Audit összefoglaló logolása; nem töröl
    void logSummary() const;

    // Összefoglaló + törlés (praktikus a startup végén)
    void flushSummary();

    // Manuális törlés (pl. új run előtt)
    void clear();

    // Opcionális: pillanatkép a begyűjtött kontextusokról
    //QVector<CsvImporter::FileContext> snapshot() const;

private:
    FileContextCollector() = default;
    ~FileContextCollector() = default;
    FileContextCollector(const FileContextCollector&) = delete;
    FileContextCollector& operator=(const FileContextCollector&) = delete;

private:
    mutable QMutex _mutex;
    QVector<FileContextSnapshot> _contexts;
    bool _enabled = true;
};

