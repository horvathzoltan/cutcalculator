#pragma once
#include <QVector>
#include <optional>

#include "common/csv/csvimporter.h"
#include "common/csv/filecontext.h"
#include "calculation/model/need_calculation_detail.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"

class NeedCalculationDetailRepository {
public:
    static bool load(QVector<NeedCalculationDetail>& out);
    static bool save(const QVector<NeedCalculationDetail>& data);

private:
    struct Row {
        QString productBarcode;
        QString modeName;
        QString materialBarcode;
        QString formula;
    };

    // Stage 1: Convert
    static std::optional<CsvImporter::AuditedRow<Row>>
    convertRowToRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx);

    // Stage 2.5: Validate
    static QVector<CsvImporter::RowError>
    validateRow(const Row& row, int lineNumber);

    // Stage 2: Build (barcode → UUID)
    static std::optional<NeedCalculationDetail>
    buildFromRow(const Row& row, CsvImporter::FileContext& ctx);

    // Stage 3: Load & Assemble
    static QVector<CsvImporter::AuditedRow<Row>>
    loadRows(CsvImporter::FileContext& ctx);

    //static QString filePath();
};
