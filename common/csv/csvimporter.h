#pragma once
#include <QVector>
#include <QString>
#include <QList>
#include <functional>
#include <optional>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include "common/utils/filehelper.h"
#include "common/logger/logger.h"
#include "filecontext_collector.h"

namespace CsvImporter{

struct RowError{
private:
    int _lineIndex;
    QString _errorMessage;
public:
    RowError(int lineIndex, const QString& errorMessage = QString())
        : _lineIndex(lineIndex), _errorMessage(errorMessage) {}

    QString toString() const {
        if (_errorMessage.isEmpty())
            return QString("⚠️ Ismeretlen hiba (Sor: %1)").arg(_lineIndex);
        return QString("%1 (Sor: %2)").arg(_errorMessage, QString::number(_lineIndex));
    }

};

struct FileContext {
private:
    QString _filepath;
    QString _operationName;
    QVector<RowError> _errors;
    QString _fileError;
    int _currentLineNumber = 0;
    int _totalLines;
    int _readlines = 0;
public:

    FileContext(const QString& operationName, const QString& filepath)
        : _filepath(filepath), _operationName(operationName) {}

    ~FileContext() {
        // Observer értesítés: automatikus gyűjtés
        FileContextCollector::instance().onContextDestroyed(*this);
    }

    QString fileError() const {
        return _fileError;
    }

    void setFileError(const QString& err) {
        _fileError = err;
    }
    // // Explicit copy ctor
    // FileContext(const FileContext& other)
    //     : _filepath(other._filepath),
    //     _errors(other._errors),
    //     _currentLineNumber(other._currentLineNumber),
    //     _totalLines(other._totalLines),
    //     _readlines(other._readlines) {}

    // // Explicit assignment
    // FileContext& operator=(const FileContext& other) {
    //     if (this != &other) {
    //         _filepath = other._filepath;
    //         _errors = other._errors;
    //         _currentLineNumber = other._currentLineNumber;
    //         _readlines = other._readlines;
    //         _totalLines = other._totalLines;
    //     }
    //     return *this;
    // }

    QString filepath() const { return _filepath; }

    void setCurrentLineNumber(int lineNumber) {
         _currentLineNumber = lineNumber;
    }

    void setTotalLines(int totalLines) {
        _totalLines = totalLines;
    }

    void setReadlines(int readlines) {
        _readlines = readlines;
    }

    int currentLineNumber() const {
         return _currentLineNumber;
    }

    QString operationName() const {
        return _operationName;
    }

    int totalLines() const {
        return _totalLines;
    }

    int readlines() const {
        return _readlines;
    }

    QVector<RowError> errors() const {
        return _errors;
    }

    void addError(int l, const QString& msg) {
        _errors.append({l, msg});
    }

    bool hasErrors() const {
        return !_errors.isEmpty() || !_fileError.isEmpty();;
    }

    int errorsSize() const {
        return _errors.size();
    }

    // QString toString() const {
    //     QString result = QString("📄 Fájl: %1\n").arg(_filepath);

    //     if (_errors.isEmpty()) {
    //         result += "✅ Nincs hiba.\n";
    //     } else {
    //         result += "Hibalista:\n";
    //         int idx = 1;
    //         for (const auto& err : _errors) {
    //             result += QString("  %1. %2").arg(idx++).arg(err.toString());
    //         }
    //     }

    //     return result;
    // }

};

inline QList<QVector<QString>> read(const QString& filepath, QChar separator = QChar()) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString msg = L("❌ Nem sikerült megnyitni a csv fájlt: %1").arg(filepath);
        zWarning(msg);
        return {};
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // 🔍 Automatikus szeparátor detektálás, ha nincs megadva
    if (separator.isNull()) {
     //   zInfo("🔍 Automatikus szeparátor keresés...");
        separator = FileHelper::detectSeparatorSmart(&in);
        if (separator.isNull()) {
            zWarning(L("❌ Nem sikerült szeparátort detektálni a fájlban: %1").arg(filepath));
            return {};
        }
        file.seek(0); // 🔁 Vissza az elejére, újraolvasáshoz
        in.seek(0);
    }

    const auto rows = FileHelper::parseCSV(&in, separator);
    return rows;
}

template<typename T>
static QVector<T> readAndConvert(CsvImporter::FileContext& ctx,
                                 std::function<std::optional<T>(const QVector<QString>&, FileContext&)> converter,
                                 bool skipHeader = true)
{
    const auto rows = read(ctx.filepath());
    QVector<T> result;

    int readlines =0;
    ctx.setTotalLines(skipHeader?rows.size()-1:rows.size());
    for (int i = 0; i < rows.size(); ++i) {
        if (skipHeader && i == 0) continue;

        const auto& row = rows[i];

        ctx.setCurrentLineNumber(i + 1);

        auto maybeObj = converter(row, ctx);
        if (maybeObj.has_value()){
            result.append(std::move(maybeObj.value()));
            readlines++;
            ctx.setReadlines(readlines);
        }
    }

    return result;
}
}
