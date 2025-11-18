#pragma once

#include "common/color/ralsystem.h"
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
};
