#include "ral_importer.h"
#include "namedcolor.h"
#include "common/logger/logger.h"
#include "common/csv/csvimporter.h"   // CsvReader namespace

bool RalImporter::loadRalColors(const QList<RalSource>& sources) {
    NamedColor::clearRalColors();

    for (const RalSource& src : sources) {
        CsvReader::FileContext ctx(src.filePath);

        auto converter = [system = src.system](const QVector<QString>& row,
                                               CsvReader::FileContext& ctx)
            -> std::optional<std::pair<QString, NamedColor>> {
            // Sorindex auditáláshoz
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
        };

        const auto items = CsvReader::readAndConvert<std::pair<QString, NamedColor>>(ctx, converter);

        if (ctx.hasErrors()) {
            zWarning(QString("⚠️ Hibák az importálás során (%1 sor):").arg(ctx.errorsSize()));
            zWarning(ctx.toString());
        }

        for (const auto& [key, value] : items) {
            NamedColor::insertRalColor(src.system, key, value);
        }

    }

    return true;
}
