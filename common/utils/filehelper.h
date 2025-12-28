#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QTextStream>
#include <QList>
#include <QVector>
#include <QString>
#include <QFile>


struct CsvRawLine {
    int rawLineNumber;          // fájlbeli sorindex (1-től)
    QVector<QString> fields;    // feldolgozott cellák
};

class FileHelper {
public:    
    // Fő CSV parser metódus: escape karakterekkel, többsoros cellákkal
    static QList<CsvRawLine> parseCSV(QTextStream *st, const QChar& separator = ';');

    static bool isCsvWithOnlyHeader(const QString &filePath);
    static QChar detectSeparatorSmart(QTextStream *st);
    static void logFileError(const QFile& file, const QString& operation, QIODevice::OpenMode mode);
    static QString getFileError(const QFile &file, const QString &operation, QIODevice::OpenMode mode);
private:
    // Egyetlen cella értelmezése: escape karakterek feldolgozása
    static QString parseCell(const QString& rawCell);
};

#endif // FILEHELPER_H
