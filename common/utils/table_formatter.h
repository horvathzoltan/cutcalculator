#pragma once

#include <QString>
#include <QVector>

/**
 * @class fTableFormatter
 * @brief Hunglish: auditbarát táblázat formázó – fejléc + sorokból szép log táblázatot csinál.
 *
 * 📚 Koncepció:
 * - Input: fejléc (QVector<QString>) + sorok (QVector<QVector<QString>>).
 * - Output: QString sorok, ahol az oszlopok szélessége a legszélesebb adat alapján számolódik.
 * - Elválasztó: " | " és "-----" sorok, mint a CSV import összefoglalóban.
 *
 * 🛠 Elhelyezés:
 * - common/utils/f_table_formatter.h/.cpp
 * - Utility jellegű, több modul is használhatja (VerboseManager, CsvImporter, Registry audit).
 *
 * 🎯 Integráció:
 * - VerboseManager::dumpRegistry() → fTableFormatter::format() hívás.
 * - CsvImporter összefoglaló → ugyanígy fTableFormatter-rel.
 */
class TableFormatter {
public:
    /// Hunglish: statikus helper – visszaadja a táblázat sorait QString listában
    static QVector<QString> format(const QVector<QString>& header,
                                   const QVector<QVector<QString>>& rows);
};
