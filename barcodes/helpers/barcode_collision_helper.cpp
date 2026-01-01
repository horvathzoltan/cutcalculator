#include "barcodes/helpers/barcode_collision_helper.h"
#include "common/logger/event_logger.h"
#include "common/registry/manager/registry_manager.h"
#include "common/utils/optional_utils.h"

QString BarcodeCollisionHelper::formatRowDisplay(const RowInfo& row) {
    if (row.name.isEmpty() && row.barcode.isEmpty()) return QString("<no name, no barcode>");
    if (row.name.isEmpty()) return QString("[%1]").arg(row.barcode);
    if (row.barcode.isEmpty()) return row.name;
    return QString("%1 [%2]").arg(row.name, row.barcode);
}

QString BarcodeCollisionHelper::formatStatus(const BarcodeRecord& rec) {
    return rec.isActive() ? "Active" : "Retired";
}

std::optional<CsvImporter::RowError>
BarcodeCollisionHelper::makeBarcodeCollisionError(const QString& myTypeName,
                                                  const RowInfo& row,
                                                  int lineNumber)
{
    const QString code = row.barcode.trimmed();
    if (code.isEmpty()) {
        return std::nullopt; // üres kódra nem dob hibát
    }

    auto& br = BarcodeRegistry::instance();

    // Globális uniqueness check – ha nem unique, nézzük, mivel ütközik
    if (!br.isBarcodeUnique(code)) {
        if (auto rec = br.findByCode(code)) {
            // 🔑 ÚJ: ha entityId üres → hazatérés, nem collision
            if (!rec->entityId.has_value()) {
                return std::nullopt;
            }

            zEventWARN(QString("Barcode collision detected: %1 (%2)").arg(code, myTypeName));

            const auto* otherEntity =
                rec->entityId.has_value()
                    ? RegistryManager::instance().findEntity(rec->entityType, *rec->entityId)
                    : nullptr;

            // const auto* thisEntity =
            //     RegistryManager::instance().findEntityByBarcode(myTypeName, row.barcode);


            const QString status = formatStatus(*rec);

            QString msg;
            if (otherEntity) {
                msg = QString("Barcode collision: %1[%2] ütközik %3[%4](%5)")
                          .arg(myTypeName)
                          .arg(formatRowDisplay(row))
                          .arg(rec->entityType)
                          .arg(otherEntity->displayName())
                          .arg(status);
            } else {
                msg = QString("Barcode collision: %1[%2] ütközik %3{%4}(%5)")
                          .arg(myTypeName)
                          .arg(formatRowDisplay(row))
                          .arg(rec->entityType)
                          .arg(OptionalUtils::toString(rec->entityId))
                          .arg(status);
            }

            return CsvImporter::RowError{ lineNumber, msg, row.barcode, row.name };
        } else {
            const QString msg = QString("Barcode collision: %1 (record not found after check)").arg(code);
            zEventERROR(QString("Inconsistent BarcodeRegistry state on collision: %1").arg(code));
            return CsvImporter::RowError{ lineNumber, msg, row.barcode, row.name };
        }
    }

    return std::nullopt; // nincs ütközés
}
