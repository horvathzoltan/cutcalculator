#pragma once

#include <QFile>
#include <QTextStream>
#include <QUuid>
#include <QStringList>
#include <QVector>

#include "common/utils/filehelper.h"
#include "common/csv/filecontext.h"
#include "common/logger/logger.h"
#include "common/registry/manager/registry_traits.h"
#include "common/registry/manager/registry_manager.h"

/**
 * 🧩 ConnectionRow – CSV sor modell (barcode-okkal)
 *
 * A tényleges kapcsolattípust (ConnectionType) már GUID-okkal tároljuk,
 * de a CSV réteg barcode-okkal dolgozik, mert az emberi input is ilyen.
 */
struct ConnectionRow {
    QString leftBarcode;
    QString rightBarcode;
};

/**
 * 📂 ConnectionRepository<ConnectionType, Traits> – generikus CSV import/export
 *
 * A Traits felel a konfigurációért:
 *  - static QString filePath();
 *  - static QStringList headers();          // pl. {"productId","materialId"}
 *  - using LeftEntity; using RightEntity;   // entitástípusok
 *  - using RegistryType;                    // kapcsolattábla registry típusa (NeedRuleRegistry, stb.)
 *
 * A repository:
 *  - csak IO-t és parse-olást végez
 *  - a registry-ket a RegistryManager-en keresztül éri el
 *  - CsvImporter::FileContext segítségével auditálható hibakezelést ad
 */
template<typename ConnectionType, typename Traits>
class ConnectionRepository {
public:
    //template<typename RegistryEngine>
    // static bool load(RegistryEngine& registry) {
    //     const QString path = Traits::filePath();

    //     using RegistryType = typename Traits::RegistryType;
    //     auto& regInstance = RegistryType::instance();

    //     QString opName = QString("%1 import").arg(regInstance.typeName());
    //     CsvImporter::FileContext ctx(opName, path);

    //     QFile file(path);
    //     QIODevice::OpenMode mode = QIODevice::ReadOnly | QIODevice::Text;
    //     if (!file.open(mode)) {
    //         ctx.setFileError(QStringLiteral("Nem sikerült megnyitni beolvasásra"));
    //         FileHelper::logFileError(file, "CSV CONNECTION_READ", mode);
    //         return false;
    //     }

    //     QTextStream in(&file);
    //     QVector<ConnectionType> defs;

    //     QString header = in.readLine();
    //     int lineNumber = 1; // header counted

    //     while (!in.atEnd()) {
    //         const QString line = in.readLine();
    //         ++lineNumber;
    //         ctx.setCurrentLineNumber(lineNumber);
    //         ctx.setReadlines(lineNumber);

    //         if (line.trimmed().isEmpty())
    //             continue;

    //         const QStringList parts = line.split(',', Qt::KeepEmptyParts);

    //         if (parts.size() < 2) {
    //             ctx.addError(lineNumber, QStringLiteral("⚠️ Kevés mező kapcsolatsorban"));
    //             continue;
    //         }

    //         const QString leftBarcode = parts[0].trimmed();
    //         const QString rightBarcode = parts[1].trimmed();

    //         if (leftBarcode.isEmpty() || rightBarcode.isEmpty()) {
    //             ctx.addError(lineNumber, QStringLiteral("⚠️ Hiányzó barcode a kapcsolatsorban"),
    //                          leftBarcode, rightBarcode);
    //             continue;
    //         }

    //         // 🔍 Registry lookup – bal oldal
    //         using LeftEntity = typename Traits::LeftEntity;
    //         using LeftRegistry = typename RegistryFor<LeftEntity>::type;
    //         auto* leftReg = RegistryManager::instance().lookupRegistry<LeftRegistry>();
    //         auto* leftEntity = leftReg ? leftReg->findByBarcode(leftBarcode) : nullptr;

    //         // 🔍 Registry lookup – jobb oldal
    //         using RightEntity = typename Traits::RightEntity;
    //         using RightRegistry = typename RegistryFor<RightEntity>::type;
    //         auto* rightReg = RegistryManager::instance().lookupRegistry<RightRegistry>();
    //         auto* rightEntity = rightReg ? rightReg->findByBarcode(rightBarcode) : nullptr;

    //         if (!leftEntity || !rightEntity) {
    //             ctx.addError(lineNumber,
    //                          QStringLiteral("⚠️ Nem található entitás a barcode alapján"),
    //                          leftBarcode,
    //                          rightBarcode);
    //             continue;
    //         }

    //         // GUID-ok beállítása a feloldott entitásokból
    //         ConnectionType c;
    //         c.leftId = leftEntity->id;
    //         c.rightId = rightEntity->id;
    //         defs.append(c);
    //     }

    //     ctx.setTotalLines(lineNumber);

    //     // A registry engine teljes adatkészletét cseréljük
    //     registry.setAll(defs);

    //     zInfo(QStringLiteral("📊 ConnectionRepository: %1 kapcsolat betöltve → %2")
    //               .arg(defs.size()).arg(path));

    //     // amikor a ctx kimegy a scope-ból, a FileContextCollector automatikusan gyűjti
    //     return !ctx.hasErrors();
    // }
    //template<typename RegistryEngine>
    static bool load(QVector<ConnectionType>& out) {
        const QString path = Traits::filePath();

        using RegistryType = typename Traits::RegistryType;
        auto& regInstance = RegistryType::instance();

        QString opName = QString("%1 import").arg(regInstance.typeName());
        CsvImporter::FileContext ctx(opName, path);

        QFile file(path);
        QIODevice::OpenMode mode = QIODevice::ReadOnly | QIODevice::Text;
        if (!file.open(mode)) {
            ctx.setFileError(QStringLiteral("Nem sikerült megnyitni beolvasásra"));
            FileHelper::logFileError(file, "CSV CONNECTION_READ", mode);
            return false;
        }

        QTextStream in(&file);
        QVector<ConnectionType> defs;

        QString header = in.readLine();
        int lineNumber = 1;

        while (!in.atEnd()) {
            const QString line = in.readLine();
            ++lineNumber;
            ctx.setCurrentLineNumber(lineNumber);
            ctx.setReadlines(lineNumber);

            if (line.trimmed().isEmpty())
                continue;

            const QStringList parts = line.split(',', Qt::KeepEmptyParts);
            if (parts.size() < 2) {
                ctx.addError(lineNumber, QStringLiteral("⚠️ Kevés mező kapcsolatsorban"));
                continue;
            }

            const QString leftBarcode = parts[0].trimmed();
            const QString rightBarcode = parts[1].trimmed();

            if (leftBarcode.isEmpty() || rightBarcode.isEmpty()) {
                ctx.addError(lineNumber, QStringLiteral("⚠️ Hiányzó barcode a kapcsolatsorban"),
                             leftBarcode, rightBarcode);
                continue;
            }

            using LeftEntity = typename Traits::LeftEntity;
            using LeftRegistry = typename RegistryFor<LeftEntity>::type;
            auto* leftReg = RegistryManager::instance().lookupRegistry<LeftRegistry>();
            auto* leftEntity = leftReg ? leftReg->findByBarcode(leftBarcode) : nullptr;

            using RightEntity = typename Traits::RightEntity;
            using RightRegistry = typename RegistryFor<RightEntity>::type;
            auto* rightReg = RegistryManager::instance().lookupRegistry<RightRegistry>();
            auto* rightEntity = rightReg ? rightReg->findByBarcode(rightBarcode) : nullptr;

            if (!leftEntity || !rightEntity) {
                ctx.addError(lineNumber,
                             QStringLiteral("⚠️ Nem található entitás a barcode alapján"),
                             leftBarcode, rightBarcode);
                continue;
            }

            ConnectionType c;
            c.leftId = leftEntity->id;
            c.rightId = rightEntity->id;
            defs.append(c);
        }

        ctx.setTotalLines(lineNumber);

        out = defs;

        zInfo(QStringLiteral("📊 ConnectionRepository: %1 kapcsolat betöltve → %2")
                  .arg(defs.size()).arg(path));

        return !ctx.hasErrors();
    }



    template<typename RegistryEngine>
    static bool save(const RegistryEngine& registry) {
        QFile file(Traits::filePath());
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
        if (!file.open(mode)) {
            FileHelper::logFileError(file, "CSV CONNECTION_SAVE", mode);
            return false;
        }

        QTextStream out(&file);
        const auto hdr = Traits::headers(); // pl. {"productId","materialId"}
        out << hdr.join(",") << "\n";

        const auto data = registry.readAll();

        for (const auto& c : data) {
            // QString leftBarcode;
            // QString rightBarcode;

            // 🔍 Bal oldali entitás barcode-ja
            // if (auto opt = c.left()) {
            //     leftBarcode = opt->barcode;
            // } else {
            //     leftBarcode.clear();
            // }

            // // 🔍 Jobb oldali entitás barcode-ja
            // if (auto opt = c.right()) {
            //     rightBarcode = opt->barcode;
            // } else {
            //     rightBarcode.clear();
            // }

            // out << leftBarcode << "," << rightBarcode << "\n";

            out << toCsvLine(c) << "\n";
        }

        zInfo(QStringLiteral("💾 ConnectionRepository: %1 kapcsolat mentve → %2")
                  .arg(data.size()).arg(Traits::filePath()));
        return true;
    }

    static QString toCsvLine(const ConnectionType& c)
    {
        QString leftBarcode;
        QString rightBarcode;

        if (auto opt = c.left())
            leftBarcode = opt->barcode;

        if (auto opt = c.right())
            rightBarcode = opt->barcode;

        return leftBarcode + "," + rightBarcode;
    }

};
