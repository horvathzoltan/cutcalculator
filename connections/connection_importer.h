#pragma once

#include <QVector>
#include <QString>
#include <optional>

#include "common/csv/csvimporter.h"
#include "common/csv/filecontext.h"

/**
 * 🧩 ConnectionImporter<Row, Domain>
 *
 * Háromfázisú import segéd:
 *  - Convert: CSV fields → Row
 *  - Validate: Row → RowError lista
 *  - Build: Row → Domain (pl. NeedRule)
 *
 * A tényleges logika a Traits-ben lesz, így az importer maga újrafelhasználható.
 */
template<typename Row, typename Domain, typename Traits>
class ConnectionImporter {
public:
    static QVector<Domain> import_all(CsvImporter::FileContext& ctx) {
        // 1) CSV sorok → Row
        auto rows = load_rows(ctx);

        // 2) Row → Domain (Domain-specifikus logikával)
        return CsvImporter::buildAll<Row, Domain>(
            rows,
            [](const Row& r, CsvImporter::FileContext& localCtx) -> std::optional<Domain> {
                return Traits::build_domain(r, localCtx);
            },
            ctx
            );
    }

private:
    static QVector<CsvImporter::AuditedRow<Row>> load_rows(CsvImporter::FileContext& ctx) {
        QVector<CsvImporter::AuditedRow<Row>> out;

        const auto converter = [](const QVector<QString>& fields,
                                  CsvImporter::FileContext& localCtx) -> std::optional<Row> {
            return Traits::convert_row(fields, localCtx);
        };

        auto items = CsvImporter::readAndConvert<Row>(ctx, converter, true);
        out.reserve(items.size());

        // readAndConvert már adja a sorindexet → be kell csomagolni AuditedRow-ba
        // Itt feltételezzük, hogy Row nem tartalmazza külön a sorindexet.
        int rawLine = 2; // header utáni első sor
        for (const auto& r : items) {
            out.push_back({ rawLine, r });
            ++rawLine;
        }

        return out;
    }
};
