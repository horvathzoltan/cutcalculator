#include "colors/repository/color_repository.h"
#include "colors/model/named_color.h"
#include "common/csv/csvimporter.h"
#include <QColor>
#include <QRegularExpression>
#include "colors/registry/color_registry.h"

/* =========================================================
 * Stage 1: Convert
 * ========================================================= */

std::optional<CsvImporter::AuditedRow<RalRow>>
ColorRepository::convertRowToRalRow(const QVector<QString>& row,
                                CsvImporter::FileContext& ctx) {
    const int line = ctx.currentLineNumber();

    // Formai ellenőrzés: pontosan 3 mező legyen
    if (row.size() != 3) {
        ctx.addError(line, QString("❌ Hibás sor a RAL CSV-ben (mezők száma != 3): %1").arg(row.join(";")));
        return std::nullopt;
    }

    RalRow rr;
    rr.code = row[0].trimmed(); //"RAL " + row[0].trimmed();  // Classic formátum preferenciája
    rr.name = row[1].trimmed();
    rr.hex  = row[2].trimmed();
    //rr.lineNumber = line;

    // Alap üresség check (konverter szinten még minimál)
    if (rr.code.isEmpty() || rr.name.isEmpty() || rr.hex.isEmpty()) {
        ctx.addError(line, QString("❌ Üres mező a sorban: %1").arg(row.join(";")));
        return std::nullopt;
    }

    return CsvImporter::AuditedRow<RalRow>{ line, rr };
}

/* =========================================================
 * Stage 2.5: Validation helpers
 * ========================================================= */

bool ColorRepository::isValidHexFormat(const QString& hex) {
    static const QRegularExpression re("^#[0-9A-Fa-f]{6}$");
    return re.match(hex.trimmed()).hasMatch();
}

// bool ColorRepository::isValidRalCode(const QString& code) {
//     static const QRegularExpression re(
//         // Opcionális RAL + opcionális P1/P2, majd vagy classic (4 számjegy),
//         // vagy 3/4 + 2 + 2/3 számjegyes blokkok, szóköz/kötőjel elválasztással.
//         R"(^(?:RAL[-\s]*)?(?:(P[12])\s*)?(?:(\d{4})|(\d{3})[\s-]*(\d{2})[\s-]*(\d{2,3}))$)",
//         QRegularExpression::UseUnicodePropertiesOption
//         );
//     return re.match(code.trimmed().toUpper()).hasMatch();
// }
bool ColorRepository::isValidRalCode(const QString& code) {
    auto n = NamedColor::normalizeRalExtended(code);

    return n.isValid();
}

/* =========================================================
 * Stage 2: Build (+ Validate)
 * ========================================================= */

std::optional<NamedColor>
ColorRepository::buildNamedColorFromRow(const RalRow& rr,
                                    CsvImporter::FileContext& ctx,
                                    RalSystem system) {
    // Tartalmi validáció (külön context, majd merge)

    auto rowErrors = validateRalRow(rr, system, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);

    // Runtime validáció: QColor elfogadja-e a HEX-et
    const QColor color(rr.hex);
    if (isValidHexFormat(rr.hex) && !color.isValid()) {
        ctx.addError(ctx.currentLineNumber(), QString("❌ Érvénytelen HEX kód (QColor): %1").arg(rr.hex));
        return std::nullopt;
    }

    auto n = NamedColor::normalizeRalExtended(rr.code);
    if(n.system != system){
        ctx.addError(ctx.currentLineNumber(),
                     QString("❌ RAL kód és rendszer nem egyezik: kód=%1, rendszer=%2")
                         .arg(rr.code, RalSystemUtils::toString(system)));
        return std::nullopt;
    };
    // Építés: domain objektum
    NamedColor nc(color, rr.name, n.key, system);
    return nc;
}

/* =========================================================
 * Stage 2.5: Validate (segédfüggvény)
 * ========================================================= */

QVector<CsvImporter::RowError>
ColorRepository::validateRalRow(const RalRow& rr, RalSystem system, int lineNumber) {
    QVector<CsvImporter::RowError> errors;

    if (rr.name.isEmpty())
        errors.append({lineNumber, "⚠️ Hiányzó név"});
    if (system == RalSystem::Unknown)
        errors.append({lineNumber, "⚠️ Ismeretlen RAL rendszer"});
    if (!isValidRalCode(rr.code))
        errors.append({lineNumber, "❌ Hibás RAL kód formátum"});
    if (!isValidHexFormat(rr.hex))
        errors.append({lineNumber, "❌ Hibás HEX formátum"});

    return errors;
}

/* =========================================================
 * Stage 3: Assemble
 * ========================================================= */

// bool ColorRepository::loadRalColors(const QList<RalSource>& sources, QVector<NamedColor>& out) {
//     //NamedColor::clearRalColors();
//     ColorRegistry::instance().clear();

//     bool ok = true;

//     for (const RalSource& src : sources) {
//         const QString systemName = RalSystemUtils::toString(src.system);
//         CsvImporter::FileContext ctx("RAL " + systemName + " import", src.filePath);

//         // 1) Convert
//         const auto rows = CsvImporter::readAndConvert<CsvImporter::AuditedRow<RalRow>>(ctx, convertRowToRalRow);

//         // 2) Build (+ Validate)
//         const QVector<NamedColor> namedColors =
//             CsvImporter::buildAll<RalRow, NamedColor>(
//             rows,
//             [&](const RalRow &rr, CsvImporter::FileContext &buildCtx) {
//                 return buildNamedColorFromRow(rr, buildCtx, src.system);
//             },
//             ctx);

//         // 3) Assemble (duplikátum ellenőrzés + beszúrás)
//         QHash<QString, QVector<int>> occurrenceMap;

//         for (int i = 0; i < namedColors.size(); ++i) {
//             const auto& nc = namedColors[i];
//             if (!nc.isValid()) continue; // hibás sor, már validációban jelzett

//             const QString key = nc.code();//.trimmed().toUpper();

//             //if (NamedColor::containsRalColor(key)) {
//             if (ColorRegistry::instance().findByCode(key) != nullptr) {
//                 occurrenceMap[key].append(rows[i].rawLineNumber);

//                 QString allLines;
//                 for (int ln : occurrenceMap[key]) {
//                     allLines += QString::number(ln) + " ";
//                 }

//                 ctx.addError(rows[i].rawLineNumber,
//                              QString("❌ Többszörös RAL kód: %1 (%2), előfordulások sorai: %3")
//                                  .arg(key, systemName, allLines.trimmed()));
//                 ok = false;
//                 continue;
//             }

//             //NamedColor::insertRalColor(nc);
//             ColorRegistry::instance().insert(nc);
//             occurrenceMap[key] = { rows[i].rawLineNumber };
//         }


//         if (ctx.hasErrors()) {
//             ok = false;
//             // opcionálisan: itt lehet összefoglalót kiírni a loggerbe
//             // zInfo(ctx.summary());
//         }
//     }

//     return ok;
// }
bool ColorRepository::loadRalColors(const QList<RalSource>& sources,
                                    QVector<NamedColor>& out)
{
    out.clear();
    bool ok = true;

    // Lokális duplikáció-ellenőrzés
    QSet<QString> globalSeen;

    for (const RalSource& src : sources) {
        const QString systemName = RalSystemUtils::toString(src.system);
        CsvImporter::FileContext ctx("RAL " + systemName + " import", src.filePath);

        // 1) Convert
        const auto rows =
            CsvImporter::readAndConvert<CsvImporter::AuditedRow<RalRow>>(
                ctx, convertRowToRalRow);

        // 2) Build (+ Validate)
        const QVector<NamedColor> namedColors =
            CsvImporter::buildAll<RalRow, NamedColor>(
                rows,
                [&](const RalRow& rr, CsvImporter::FileContext& buildCtx) {
                    return buildNamedColorFromRow(rr, buildCtx, src.system);
                },
                ctx);

        // 3) Assemble (duplikáció-ellenőrzés lokálisan)
        for (int i = 0; i < namedColors.size(); ++i) {
            const auto& nc = namedColors[i];
            if (!nc.isValid())
                continue;

            QString key = nc.code().trimmed().toUpper();

            if (globalSeen.contains(key)) {
                ctx.addError(rows[i].rawLineNumber,
                             QString("❌ Többszörös RAL kód: %1 (%2)")
                                 .arg(key, systemName));
                ok = false;
                continue;
            }

            globalSeen.insert(key);
            out.append(nc);
        }

        if (ctx.hasErrors())
            ok = false;
    }

    return ok;
}
