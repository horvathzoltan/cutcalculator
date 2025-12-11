#pragma once
#include <QFile>
#include <QTextStream>
#include <QUuid>
#include <QStringList>
#include "common/csv/filecontext.h"
#include "common/logger/logger.h"
#include "common/registry/registry_traits.h"
#include "connection_registry.h"
#include "common/registry/registry_lookup.h"


struct ConnectionRow {
    QString leftBarcode;
    QString rightBarcode;
};

/**
 * 📂 ConnectionRepository<T, Traits> – generikus CSV import/export
 *
 * Hunglish: a traits mondja meg a pathot és a headert. A repo csak IO-t végez és parse-ol.
 * Elvárás a Traits-től:
 *  - static QString filePath();
 *  - static QStringList headers(); // pl. {"productId","materialId"}
 */
template<typename ConnectionType, typename Traits>
class ConnectionRepository {
public:
    static bool load(class ConnectionRegistry<ConnectionType>& registry) {
        const QString path = Traits::filePath();

        using RegistryType = typename Traits::RegistryType;
        auto* reg = lookupRegistry<RegistryType>();
        QString opName = reg ? QString("%1 import").arg(reg->typeName()) : "Connection import";

        //QString opName = QString("%1 import").arg(reg->typeName());
        CsvImporter::FileContext ctx(opName, path);

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            ctx.setFileError("Nem sikerült megnyitni beolvasásra");
            zWarning(QString("❌ Nem sikerült megnyitni beolvasásra: %1").arg(path));
            return false;
        }

        QTextStream in(&file);
        QVector<ConnectionType> defs;

        QString header = in.readLine();
        int lineNumber = 1; // header counted

        // while (!in.atEnd()) {
        //     const QString line = in.readLine();
        //     ++lineNumber;
        //     ctx.setCurrentLineNumber(lineNumber);
        //     ctx.setReadlines(lineNumber);

        //     if (line.trimmed().isEmpty()) continue;
        //     const QStringList parts = line.split(',', Qt::KeepEmptyParts);

        //     if (parts.size() < 2) {
        //         ctx.addError(lineNumber, "⚠️ Kevés mező kapcsolatsorban");
        //         continue;
        //     }

        //     const QUuid left(parts[0].trimmed());
        //     const QUuid right(parts[1].trimmed());

        //     if (left.isNull() || right.isNull()) {
        //         ctx.addError(lineNumber, "⚠️ Érvénytelen UUID a kapcsolatsorban");
        //         continue;
        //     }

        //     ConnectionType c;
        //     c.leftId = left;
        //     c.rightId = right;
        //     defs.append(c);
        // }

        // connection_repository.h
        // ConnectionRepository::load

        while (!in.atEnd()) {
            const QString line = in.readLine();
            ++lineNumber;
            ctx.setCurrentLineNumber(lineNumber);
            ctx.setReadlines(lineNumber);

            if (line.trimmed().isEmpty()) continue;
            const QStringList parts = line.split(',', Qt::KeepEmptyParts);

            if (parts.size() < 2) {
                ctx.addError(lineNumber, "⚠️ Kevés mező kapcsolatsorban");
                continue;
            }

            // Új: barcode stringek beolvasása
            const QString leftBarcode = parts[0].trimmed();
            const QString rightBarcode = parts[1].trimmed();

            if (leftBarcode.isEmpty() || rightBarcode.isEmpty()) {
                ctx.addError(lineNumber, "⚠️ Hiányzó barcode a kapcsolatsorban");
                continue;
            }

            // Registry lookup – bal oldal
            using LeftRegistry = typename RegistryFor<typename Traits::LeftEntity>::type;
            auto* leftReg = lookupRegistry<LeftRegistry>();
            auto* leftEntity = leftReg ? leftReg->findByBarcode(leftBarcode) : nullptr;

            // Registry lookup – jobb oldal
            using RightRegistry = typename RegistryFor<typename Traits::RightEntity>::type;
            auto* rightReg = lookupRegistry<RightRegistry>();
            auto* rightEntity = rightReg ? rightReg->findByBarcode(rightBarcode) : nullptr;

            if (!leftEntity || !rightEntity) {
                ctx.addError(lineNumber,
                             "⚠️ Nem található entitás a barcode alapján",
                             leftBarcode,
                             rightBarcode);
                continue;
            }

            // GUID beállítása a feloldott entitásokból
            ConnectionType c;
            c.leftId = leftEntity->id;
            c.rightId = rightEntity->id;
            defs.append(c);
        }

        ctx.setTotalLines(lineNumber);
        registry.setData(defs);

        zInfo(QString("📊 ConnectionRepository: %1 kapcsolat betöltve → %2")
                  .arg(defs.size()).arg(path));

        // amikor a ctx kimegy a scope-ból, a destruktor átadja magát a FileContextCollector-nak
        return true;
    }


    static bool save(const QVector<ConnectionType>& data) {
        QFile file(Traits::filePath());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            zWarning(QString("❌ Nem sikerült megnyitni írásra: %1").arg(Traits::filePath()));
            return false;
        }

        QTextStream out(&file);
        const auto hdr = Traits::headers(); // pl. {"productBarcode","materialBarcode"}
        out << hdr.join(",") << "\n";

        for (const auto& c : data) {
            QString leftBarcode, rightBarcode;

            // 🔍 Bal oldali entitás barcode-ja
            if (auto opt = c.left()) {
                leftBarcode = opt->barcode;
            } else {
                leftBarcode = ""; // vagy "<missing>"
            }

            // 🔍 Jobb oldali entitás barcode-ja
            if (auto opt = c.right()) {
                rightBarcode = opt->barcode;
            } else {
                rightBarcode = ""; // vagy "<missing>"
            }

            out << leftBarcode << "," << rightBarcode << "\n";
        }

        zWarning(QString("💾 ConnectionRepository: %1 kapcsolat mentve → %2")
                     .arg(data.size()).arg(Traits::filePath()));
        return true;
    }


    // static bool save(const QVector<ConnectionType>& data) {
    //     QFile file(Traits::filePath());
    //     if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    //         zWarning(QString("❌ Nem sikerült megnyitni írásra: %1").arg(Traits::filePath()));
    //         return false;
    //     }

    //     QTextStream out(&file);
    //     const auto hdr = Traits::headers();
    //     out << hdr.join(",") << "\n";

    //     for (const auto& c : data) {
    //         out << c.leftId.toString(QUuid::WithBraces) << ","
    //             << c.rightId.toString(QUuid::WithBraces) << "\n";
    //     }

    //     zWarning(QString("💾 ConnectionRepository: %1 kapcsolat mentve → %2")
    //                    .arg(data.size()).arg(Traits::filePath()));
    //     return true;
    // }
};
