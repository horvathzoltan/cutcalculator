#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QTextStream>
#include <QList>
#include <QVector>
#include <QString>


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
private:
    // Egyetlen cella értelmezése: escape karakterek feldolgozása
    static QString parseCell(const QString& rawCell);
};

#endif // FILEHELPER_H
