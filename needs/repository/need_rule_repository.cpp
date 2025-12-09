#include <QFile>
#include <QTextStream>
#include <QUuid>

#include "needs/repository/need_rule_repository.h"
#include "common/logger/event_logger.h"
#include "common/utils/filename_helper.h"

// 1. Convert: CSV sor → NeedRuleRow
std::optional<CsvImporter::AuditedRow<NeedRuleRepository::NeedRuleRow>>
NeedRuleRepository::convertRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx) {
    if (parts.size() < 2) {
        ctx.addError(ctx.currentLineNumber(), "⚠️ Kevés mező NeedRule sorban (>=2)");
        return std::nullopt;
    }

    NeedRuleRow row {
        .productIdStr  = parts[0].trimmed(),
        .materialIdStr = parts[1].trimmed()
    };

    return CsvImporter::AuditedRow<NeedRuleRow>{ ctx.currentLineNumber(), row };
}

// 2. Build: NeedRuleRow → NeedRule
std::optional<NeedRule>
NeedRuleRepository::buildRule(const NeedRuleRow& row, CsvImporter::FileContext& ctx) {
    auto errors = validateRow(row, ctx.currentLineNumber());
    ctx.addErrors(errors);

    if (!errors.isEmpty()) return std::nullopt;

    QUuid pid(row.productIdStr);
    QUuid mid(row.materialIdStr);

    NeedRule rule;
    rule.leftId = pid;    // productId
    rule.rightId = mid;   // materialId

    return rule;
}

// 2.5 Validate
QVector<CsvImporter::RowError>
NeedRuleRepository::validateRow(const NeedRuleRow& row, int lineNumber) {
    QVector<CsvImporter::RowError> errors;

    if (row.productIdStr.isEmpty())
        errors.append({lineNumber, "⚠️ Hiányzó productId"});
    else {
        QUuid pid(row.productIdStr);
        if (pid.isNull())
            errors.append({lineNumber, "⚠️ Érvénytelen productId UUID"});
    }

    if (row.materialIdStr.isEmpty())
        errors.append({lineNumber, "⚠️ Hiányzó materialId"});
    else {
        QUuid mid(row.materialIdStr);
        if (mid.isNull())
            errors.append({lineNumber, "⚠️ Érvénytelen materialId UUID"});
    }

    return errors;
}

// 3. Load & Assemble
QVector<CsvImporter::AuditedRow<NeedRuleRepository::NeedRuleRow>>
NeedRuleRepository::loadRows(CsvImporter::FileContext& ctx) {
    return CsvImporter::readAndConvert<CsvImporter::AuditedRow<NeedRuleRow>>(ctx, convertRow);
}

// Entry: load
bool NeedRuleRepository::loadFromCSV(NeedRuleRegistry& registry) {
    const auto& helper = FileNameHelper::instance();
    if (!helper.isInitialized()) {
        zEventWARN("❌ FileNameHelper nincs inicializálva");
        return false;
    }

    const QString path = helper.getNeedRuleCsvFile(); // pl. needs_rules.csv
    CsvImporter::FileContext ctx("NeedRule import", path);

    const auto rows = loadRows(ctx);

    const QVector<NeedRule> defs =
        CsvImporter::buildAll<NeedRuleRow, NeedRule>(rows, buildRule, ctx);

    if (ctx.hasErrors()) {
        zEventWARN(QString("⚠️ Hibák a NeedRule import során (%1)").arg(ctx.errorsSize()));
    }

    registry.setData(defs);
    zEventINFO(QString("📊 NeedRuleRepository: %1 kapcsolat betöltve").arg(defs.size()));
    return !defs.isEmpty();
}

// Entry: save
bool NeedRuleRepository::saveToCSV(const QVector<NeedRule>& data, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        zEventWARN(QString("❌ Nem sikerült megnyitni a fájlt írásra: %1").arg(path));
        return false;
    }

    QTextStream out(&file);
    out << "productId,materialId\n";
    for (const auto& r : data) {
        out << r.leftId.toString(QUuid::WithBraces) << ",";
        out << r.rightId.toString(QUuid::WithBraces) << "\n";
    }

    zEventINFO(QString("💾 NeedRuleRepository: %1 kapcsolat mentve → %2")
                   .arg(data.size()).arg(path));
    return true;
}
