// error_bucketizer.h
#pragma once
#include <QString>
#include <QMap>

class ErrorBucketizer {
public:
    // Egyszerű normalizálás: prefixek és kulcsszavak alapján
    static QString categorize(const QString& errText) {
        QString key = errText.trimmed();

        // Távolítsuk el az emoji prefixet
        if (key.startsWith("⚠️")) key = key.mid(2).trimmed();

        // Kulcsszó alapú kategorizálás
        if (key.contains("átmérő", Qt::CaseInsensitive)) return "Érvénytelen átmérő";
        if (key.contains("szélesség", Qt::CaseInsensitive) ||
            key.contains("magasság", Qt::CaseInsensitive)) return "Érvénytelen szélesség/magasság";
        if (key.contains("szín", Qt::CaseInsensitive)) return "Ismeretlen színformátum";
        if (key.contains("alak", Qt::CaseInsensitive)) return "Hiányzó vagy ismeretlen alak";
        if (key.contains("anyagtípus", Qt::CaseInsensitive)) return "Hiányzó vagy ismeretlen anyagtípus";

        // Alapértelmezett: teljes szöveg
        return key;
    }
};
