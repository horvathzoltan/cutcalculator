#include "ral_importer.h"
#include "namedcolor.h"
#include "common/csv/csvimporter.h"   // CsvReader namespace

std::optional<RalRow>
RalImporter::convertRowToRalRow(const QVector<QString>& row,
                                CsvImporter::FileContext& ctx) {
    int line = ctx.currentLineNumber();

    if (row.size() != 3) {
        ctx.addError(line, QString("❌ Hibás sor a RAL CSV-ben: %1").arg(row.join(";")));
        return std::nullopt;
    }

    RalRow rr;
    rr.code = "RAL " + row[0].trimmed();
    rr.name = row[1].trimmed();
    rr.hex  = row[2].trimmed();
    rr.lineNumber = line;
    return rr;
}

std::optional<NamedColor>
RalImporter::buildNamedColorFromRow(const RalRow& rr,
                                    CsvImporter::FileContext& ctx,
                                    RalSystem system) {
    // Validációs hibák külön ctx-ben
    auto validationCtx = validateRalRow(rr, system);
    ctx.merge(validationCtx);

    QColor color(rr.hex);
    if (!color.isValid()) {
        ctx.addError(rr.lineNumber, QString("❌ Érvénytelen HEX kód: %1").arg(rr.hex));
        return std::nullopt;
    }

    NamedColor nc(color, rr.name, rr.code.toUpper(), system);
    return nc;
}

CsvImporter::FileContext
RalImporter::validateRalRow(const RalRow& rr, RalSystem system) {
    CsvImporter::FileContext ctx("RAL validation", rr.code);

    if (rr.name.isEmpty()) {
        ctx.addError(rr.lineNumber, QString("⚠️ Hiányzó név a RAL kódhoz: %1").arg(rr.code));
    }

    if (system == RalSystem::Unknown) {
        ctx.addError(rr.lineNumber, QString("⚠️ Ismeretlen RAL rendszer: %1").arg(rr.code));
    }

    return ctx;
}

bool RalImporter::loadRalColors(const QList<RalSource>& sources) {
    NamedColor::clearRalColors();
    bool ok = true;

    for (const RalSource& src : sources) {
        QString ral_systemName = RalSystemUtils::toString(src.system);
        CsvImporter::FileContext ctx("RAL "+ral_systemName+" import", src.filePath);

        // Stage 1: Convert
        const auto rows = CsvImporter::readAndConvert<RalRow>(ctx, convertRowToRalRow);

        // Stage 2: Build (+ Validate)
        const auto colors = CsvImporter::buildAll<RalRow, NamedColor>(
            rows,
            [&](const RalRow& rr, CsvImporter::FileContext& ctx) {
                return buildNamedColorFromRow(rr, ctx, src.system);
            },
            ctx
            );

        if (ctx.hasErrors()) {
            ok = false;
        }

        // Stage 3: Assemble
        for (const auto& nc : colors) {
            NamedColor::insertRalColor(src.system, nc.code(), nc);
        }
    }

    return ok;
}
