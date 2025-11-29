#include "ral_importer.h"
#include "namedcolor.h"
#include "common/csv/csvimporter.h"
#include <QColor>
#include <QRegularExpression>

/* =========================================================
 * Stage 1: Convert
 * ========================================================= */

std::optional<RalRow>
RalImporter::convertRowToRalRow(const QVector<QString>& row,
                                CsvImporter::FileContext& ctx) {
    const int line = ctx.currentLineNumber();

    // Formai ellenőrzés: pontosan 3 mező legyen
    if (row.size() != 3) {
        ctx.addError(line, QString("❌ Hibás sor a RAL CSV-ben (mezők száma != 3): %1").arg(row.join(";")));
        return std::nullopt;
    }

    RalRow rr;
    rr.code = "RAL " + row[0].trimmed();  // Classic formátum preferenciája
    rr.name = row[1].trimmed();
    rr.hex  = row[2].trimmed();
    rr.lineNumber = line;

    // Alap üresség check (konverter szinten még minimál)
    if (rr.code.isEmpty() || rr.name.isEmpty() || rr.hex.isEmpty()) {
        ctx.addError(line, QString("❌ Üres mező a sorban: %1").arg(row.join(";")));
        return std::nullopt;
    }

    return rr;
}

/* =========================================================
 * Stage 2.5: Validation helpers
 * ========================================================= */

bool RalImporter::isValidHexFormat(const QString& hex) {
    static const QRegularExpression re("^#[0-9A-Fa-f]{6}$");
    return re.match(hex.trimmed()).hasMatch();
}

bool RalImporter::isValidRalCode(const QString& code) {
    static const QRegularExpression re(
        // Opcionális RAL + opcionális P1/P2, majd vagy classic (4 számjegy),
        // vagy 3/4 + 2 + 2/3 számjegyes blokkok, szóköz/kötőjel elválasztással.
        R"(^(?:RAL\s*)?(?:(P[12])\s*)?(?:(\d{4})|(\d{3,4})[\s-]+(\d{2})[\s-]+(\d{2,3}))$)",
        QRegularExpression::UseUnicodePropertiesOption
        );
    return re.match(code.trimmed().toUpper()).hasMatch();
}


/* =========================================================
 * Stage 2: Build (+ Validate)
 * ========================================================= */

std::optional<NamedColor>
RalImporter::buildNamedColorFromRow(const RalRow& rr,
                                    CsvImporter::FileContext& ctx,
                                    RalSystem system) {
    // Tartalmi validáció (külön context, majd merge)

    auto rowErrors = validateRalRow(rr, system);
    ctx.addErrors(rowErrors);

    // Runtime validáció: QColor elfogadja-e a HEX-et
    const QColor color(rr.hex);
    if (!color.isValid()) {
        ctx.addError(rr.lineNumber, QString("❌ Érvénytelen HEX kód (QColor): %1").arg(rr.hex));
        return std::nullopt;
    }

    // Építés: domain objektum
    NamedColor nc(color, rr.name, rr.code.toUpper(), system);
    return nc;
}

/* =========================================================
 * Stage 2.5: Validate (segédfüggvény)
 * ========================================================= */

QVector<CsvImporter::RowError>
RalImporter::validateRalRow(const RalRow& rr, RalSystem system) {
    QVector<CsvImporter::RowError> errors;

    if (rr.name.isEmpty())
        errors.append({rr.lineNumber, QString("⚠️ Hiányzó név: %1").arg(rr.code)});

    if (system == RalSystem::Unknown)
        errors.append({rr.lineNumber, QString("⚠️ Ismeretlen RAL rendszer: %1").arg(rr.code)});

    if (!isValidRalCode(rr.code))
        errors.append({rr.lineNumber, QString("⚠️ Hibás RAL kód formátum: %1").arg(rr.code)});

    if (!isValidHexFormat(rr.hex))
        errors.append({rr.lineNumber, QString("⚠️ Hibás HEX formátum: %1").arg(rr.hex)});

    return errors;
}

/* =========================================================
 * Stage 3: Assemble
 * ========================================================= */

bool RalImporter::loadRalColors(const QList<RalSource>& sources) {
    NamedColor::clearRalColors();
    bool ok = true;

    for (const RalSource& src : sources) {
        const QString systemName = RalSystemUtils::toString(src.system);
        CsvImporter::FileContext ctx("RAL " + systemName + " import", src.filePath);

        // 1) Convert
        const auto rows = CsvImporter::readAndConvert<RalRow>(ctx, convertRowToRalRow);

        // 2) Build (+ Validate)
        const auto colors = CsvImporter::buildAll<RalRow, NamedColor>(
            rows,
            [&](const RalRow& rr, CsvImporter::FileContext& buildCtx) {
                return buildNamedColorFromRow(rr, buildCtx, src.system);
            },
            ctx
            );

        // 3) Assemble (duplikátum ellenőrzés + beszúrás)
        for (const auto& nc : colors) {
            const QString key = nc.code().trimmed().toUpper();

            if (NamedColor::containsRalColor(src.system, key)) {
                ctx.addError(/*line*/ 0, QString("❌ Duplikált RAL kód: %1 (%2)").arg(key, systemName));
                ok = false;
                continue;
            }

            NamedColor::insertRalColor(src.system, key, nc);
        }

        if (ctx.hasErrors()) {
            ok = false;
            // opcionálisan: itt lehet összefoglalót kiírni a loggerbe
            // zInfo(ctx.summary());
        }
    }

    return ok;
}
