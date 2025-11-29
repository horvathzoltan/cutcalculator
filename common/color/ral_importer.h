#pragma once

#include "common/color/ralsystem.h"
#include "common/csv/csvimporter.h"
#include "namedcolor.h"
#include <QString>
#include <QList>
#include <optional>
#include <QVector>

/**
 * @brief A forrás definíciója: melyik RAL rendszerhez melyik fájl tartozik.
 */
struct RalSource {
    RalSystem system;
    QString filePath;
};

/**
 * @brief Nyers sor modell a Convert fázisban.
 *
 * A CSV sorból típusos, minimálisan érvényesített struktúra
 * a Build fázis mélyebb validációjához.
 */
struct RalRow {
    QString code;       ///< RAL kód (pl. "RAL 1001")
    QString name;       ///< Színnév
    QString hex;        ///< HEX színkód (pl. "#AABBCC")
    int lineNumber;     ///< Eredeti CSV sor sorszáma
};

/**
 * @brief Háromfázisú RAL importáló.
 *
 * Fázisok:
 * 1) Convert: CSV sor → RalRow (formai ellenőrzések)
 * 2) Build (+ Validate): RalRow → NamedColor (tartalmi és runtime ellenőrzések)
 * 3) Assemble: NamedColor → registry (konzisztencia, duplikátumok)
 */
class RalImporter {
public:
    /**
     * @brief Betölti a megadott forrásokból a RAL színeket.
     * @param sources RAL rendszer + fájl párok listája
     * @return true, ha nem volt hiba; false, ha volt bármely forrásnál hiba
     */
    static bool loadRalColors(const QList<RalSource>& sources);

private:
    // =========================
    // Stage 1: Convert
    // =========================

    /**
     * @brief Konvertál egy CSV sort RalRow struktúrává (formai ellenőrzés).
     * @param row CSV sor mezők
     * @param ctx Audit context (hibák ide kerülnek)
     * @return RalRow, ha sikerült; std::nullopt, ha formai hiba volt
     */
    static std::optional<RalRow>
    convertRowToRalRow(const QVector<QString>& row,
                       CsvImporter::FileContext& ctx);

    // =========================
    // Stage 2: Build (+ Validate)
    // =========================

    /**
     * @brief RalRow → NamedColor építése, tartalmi és runtime validációval.
     * @param rr Konvertált sor
     * @param ctx Audit context (hibák ide kerülnek)
     * @param system RAL rendszer
     * @return NamedColor, ha minden valid; std::nullopt hiba esetén
     */
    static std::optional<NamedColor>
    buildNamedColorFromRow(const RalRow& rr,
                           CsvImporter::FileContext& ctx,
                           RalSystem system);

    /**
     * @brief Tartalmi validáció RalRow szinten (Stage 2.5).
     * @param rr Konvertált sor
     * @param system RAL rendszer
     * @return Külön FileContext csak a validációs hibákkal
     */
    static QVector<CsvImporter::RowError>
    validateRalRow(const RalRow& rr, RalSystem system);

    // =========================
    // Stage 2.5: Validation helpers
    // =========================

    /**
     * @brief Ellenőrzi, hogy a HEX formátum megfelel-e a szabványnak.
     * @param hex HEX sztring
     * @return true, ha a formátum: ^#[0-9A-Fa-f]{6}$ ; különben false
     */
    static bool isValidHexFormat(const QString& hex);

    /**
     * @brief Ellenőrzi, hogy a RAL kód formátuma Classic: ^RAL [0-9]{4}$.
     * @param code RAL kód (pl. "RAL 1001")
     * @return true, ha egyezik; false különben
     */
    static bool isValidRalCode(const QString& code);
};
