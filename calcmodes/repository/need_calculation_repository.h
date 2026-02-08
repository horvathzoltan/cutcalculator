#pragma once
#include <QVector>
#include <optional>

#include "common/csv/csvimporter.h"
#include "common/csv/filecontext.h"
#include "calcmodes/model/need_calculation.h"
#include "products/registry/product_registry.h"

class NeedCalculationRepository {
    friend class test_NeedRuleRepository;
public:
    static bool load(QVector<NeedCalculation>& out);
    static bool save(const QVector<NeedCalculation>& data);
    static QString toCsvLine(const NeedCalculation& e);

private:
    struct Row {
        QString productBarcode;
        QString modeName;
    };

    // Stage 1: Convert
    static std::optional<CsvImporter::AuditedRow<Row>>
    convertRowToRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx);

    // Stage 2: Build
    static std::optional<NeedCalculation>
    buildFromRow(const Row& row, CsvImporter::FileContext& ctx);

    // Stage 2.5: Validate
    static QVector<CsvImporter::RowError>
    validateRow(const Row& row, int lineNumber);

    // Stage 3: Load & Assemble
    static QVector<CsvImporter::AuditedRow<Row>>
    loadRows(CsvImporter::FileContext& ctx);

    //static QString filePath();
};
