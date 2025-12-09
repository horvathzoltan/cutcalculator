#pragma once
#include <optional>
#include <QVector>
#include <QString>
#include "common/csv/csvimporter.h"
#include "common/csv/filecontext.h"
#include "needs/model/need_rule.h"
#include "needs/registry/need_rule_registry.h"

/**
 * NeedRuleRepository
 *
 * Hunglish:
 * - Three Phase Import minta (Convert → Build → Assemble).
 * - Export CSV-be ugyanúgy, mint a ProductRepository esetén.
 *
 * CSV séma:
 * productId,materialId
 * {uuid},{uuid}
 */
class NeedRuleRepository {
public:
    static bool loadFromCSV(NeedRuleRegistry& registry);
    static bool saveToCSV(const QVector<NeedRule>& data, const QString& path);

private:
    struct NeedRuleRow {
        QString productIdStr;
        QString materialIdStr;
    };

    // 1. Convert
    static std::optional<CsvImporter::AuditedRow<NeedRuleRow>>
    convertRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx);

    // 2. Build
    static std::optional<NeedRule>
    buildRule(const NeedRuleRow& row, CsvImporter::FileContext& ctx);

    // 2.5 Validate
    static QVector<CsvImporter::RowError>
    validateRow(const NeedRuleRow& row, int lineNumber);

    // 3. Load & Assemble
    static QVector<CsvImporter::AuditedRow<NeedRuleRow>>
    loadRows(CsvImporter::FileContext& ctx);
};
