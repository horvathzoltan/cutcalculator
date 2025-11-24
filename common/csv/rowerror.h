#pragma once

#include <QString>

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

    QString errorMessage() const {
        return _errorMessage;
    }

    int lineIndex() const {return _lineIndex;}
};
} // endof namespace CsvImporter
