#include "ral_importer.h"
#include "namedcolor.h"
//#include "common/logger/logger.h"
#include "common/csv/csvimporter.h"   // CsvReader namespace

bool RalImporter::loadRalColors(const QList<RalSource>& sources) {
    NamedColor::clearRalColors();
    bool ok = true;

    for (const RalSource& src : sources) {
        QString ral_systemName = RalSystemUtils::toString(src.system);

        CsvImporter::FileContext ctx("RAL "+ral_systemName+" import", src.filePath);

        const auto items = CsvImporter::readAndConvert<std::pair<QString, NamedColor>>(
            ctx,
            [&](const QVector<QString>& row, CsvImporter::FileContext& ctx) {
                return convertRowToNamedColor(row, ctx, src.system);
            }
            );

        if (items.isEmpty()) {
            ctx.setFileError("❌ Sikertelen beolvasás: a fájl nem tartalmazott feldolgozható adatot.");
        }

        if (ctx.hasErrors()) {
            // QString msg = QString("⚠️ RAL import hiba [színrendszer: %1]").arg(ral_systemName);
            // zWarning(msg);
            // zWarning(ctx.toString());
            ok = false;
        }

        for (const auto& [key, value] : items) {
            NamedColor::insertRalColor(src.system, key, value);
        }
    }

    return ok;
}

std::optional<std::pair<QString, NamedColor>>
RalImporter::convertRowToNamedColor(const QVector<QString>& row,
                                    CsvImporter::FileContext& ctx,
                                    RalSystem system)
{
    int line = ctx.currentLineNumber();

    if (row.size() != 3) {
        ctx.addError(line, QString("❌ Hibás sor a RAL CSV-ben: %1").arg(row.join(";")));
        return std::nullopt;
    }

    const QString code = "RAL " + row[0].trimmed();
    const QString name = row[1].trimmed();
    const QString hex  = row[2].trimmed();

    QColor color(hex);
    if (!color.isValid()) {
        ctx.addError(line, QString("❌ Érvénytelen HEX kód: %1").arg(hex));
        return std::nullopt;
    }

    NamedColor nc(color, name, code.toUpper(), system);
    return std::pair<QString, NamedColor>(code.toUpper(), nc);
}
