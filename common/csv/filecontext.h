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
    int _totalLines;
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

    bool hasErrors() const {
        return !_errors.isEmpty() || !_fileError.isEmpty();;
    }

    int errorsSize() const {
        return _errors.size();
    }
};
} //end namespace CsvImporter
