#pragma once

#include "common/color/ralsystem.h"
#include "common/csv/csvimporter.h"
#include "namedcolor.h"
#include <QString>
#include <QList>
//#include "namedcolor.h"

// Forrás definíció: melyik RAL rendszerhez melyik fájl tartozik
#pragma once

#include "common/color/ralsystem.h"
#include "common/csv/csvimporter.h"
#include "namedcolor.h"
#include <QString>
#include <QList>

// Forrás definíció: melyik RAL rendszerhez melyik fájl tartozik
struct RalSource {
    RalSystem system;
    QString filePath;
};

// Nyers sor modell (Convert stage)
struct RalRow {
    QString code;
    QString name;
    QString hex;
    int lineNumber;
};

// Importáló osztály
class RalImporter {
public:
    static bool loadRalColors(const QList<RalSource>& sources);

private:
    // Stage 1: Convert
    static std::optional<RalRow>
    convertRowToRalRow(const QVector<QString>& row,
                       CsvImporter::FileContext& ctx);

    // Stage 2: Build
    static std::optional<NamedColor>
    buildNamedColorFromRow(const RalRow& rr,
                           CsvImporter::FileContext& ctx,
                           RalSystem system);

    // Stage 2.5: Validate (segédfüggvény)
    static CsvImporter::FileContext
    validateRalRow(const RalRow& rr, RalSystem system);
};
