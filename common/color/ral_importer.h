#pragma once

#include "common/color/ralsystem.h"
#include "common/csv/csvimporter.h"
#include "namedcolor.h"
#include <QString>
#include <QList>
//#include "namedcolor.h"

// Forrás definíció: melyik RAL rendszerhez melyik fájl tartozik
struct RalSource {
    RalSystem system;
    QString filePath;
};

// Importáló osztály
class RalImporter {
public:
    // Feltölti a NamedColor::ralColors_ mapet a megadott forrásokból
    static bool loadRalColors(const QList<RalSource>& sources);

private:
    // Egy CSV sor konvertálása NamedColor objektummá
    static std::optional<std::pair<QString, NamedColor>>
    convertRowToNamedColor(const QVector<QString>& row,
                           CsvImporter::FileContext& ctx,
                           RalSystem system);
};
