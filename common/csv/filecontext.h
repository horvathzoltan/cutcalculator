#pragma once

#include "rowerror.h"

#include <QString>
#include <QVector>

namespace CsvImporter{
struct FileContext {
private:
    QString _filepath;
    QString _operationName;
    QVector<CsvImporter::RowError> _errors;
    QString _fileError;
    int _currentLineNumber = 0;
    int _totalLines = 0;
    int _readlines = 0;
public:

    FileContext(const QString& operationName, const QString& filepath);

    ~FileContext();

    QString fileError() const {
        return _fileError;
    }

    void setFileError(const QString& err) {
        _fileError = err;
    }

    QString filepath() const { return _filepath; }

    void setFilePath(const QString& path) {
        _filepath = path;
    }

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

    QVector<CsvImporter::RowError> errors() const {
        return _errors;
    }

    void addError(int l, const QString& msg) {
        _errors.append({l, msg});
    }

    void addErrors(const QVector<RowError>& errs) {
        _errors += errs;
    }

    bool hasErrors() const {
        return !_errors.isEmpty() || !_fileError.isEmpty();;
    }

    int errorsSize() const {
        return _errors.size();
    }

    void merge(const FileContext& other) {
        // Sorhibák átvétele
        // for (const auto& err : other.errors()) {
        //     _errors.append(err);
        // }
        _errors += other.errors();

        // File szintű hiba, ha van
        if (!other.fileError().isEmpty()) {
            if (_fileError.isEmpty()) {
                _fileError = other.fileError();
            } else {
                _fileError += "\n" + other.fileError();
            }
        }

        // Metaadatok összevonása
        if (other.totalLines() > 0) {
            _totalLines += other.totalLines();
        }
        _readlines += other.readlines();
    }
};
} //end namespace CsvImporter
