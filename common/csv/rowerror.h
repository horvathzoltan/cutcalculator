#pragma once

#include <QString>

namespace CsvImporter{

struct RowError{
private:
    int _lineIndex;
    QString _errorMessage;

    QString _barcode; // opcionális
    QString _name;    // opcionális

  //  QString _fileName; // új mező

public:
    RowError(int lineIndex, const QString& errorMessage = QString(),
             const QString& barcode = QString(), const QString& name = QString())
        : _lineIndex(lineIndex), _errorMessage(errorMessage)
        , _barcode(barcode), _name(name)
     //   , _fileName(fileName)
    {}

    QString toString() const {
        if (_errorMessage.isEmpty())
            return QString("⚠️ Ismeretlen hiba (Sor: %1)").arg(_lineIndex);
        return QString("%1 (Sor: %2)").arg(_errorMessage, QString::number(_lineIndex));
    }

    QString errorMessage() const {
        return _errorMessage;
    }

    QString barcode() const {
        return _barcode;
    }

    QString name() const {
        return _name;
    }

    // QString fileName() const {
    //     return _fileName;
    // }

    int lineIndex() const {return _lineIndex;}
};
} // endof namespace CsvImporter
