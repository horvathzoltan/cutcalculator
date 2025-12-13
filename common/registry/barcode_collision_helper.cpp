#include "common/registry/barcode_collision_helper.h"
#include "common/logger/event_logger.h"
#include "common/registry/registry_manager.h"
//#include "common/model/identifiable_entity.h"

// Hunglish: kis helper, mert ez a formátum gyakran kell.
QString BarcodeCollisionHelper::formatRowDisplay(const RowInfo& row) {
    if (row.name.isEmpty() && row.barcode.isEmpty()) return QString("<no name, no barcode>");
    if (row.name.isEmpty()) return QString("[%1]").arg(row.barcode);
    if (row.barcode.isEmpty()) return row.name;
    return QString("%1 [%2]").arg(row.name, row.barcode);
}

QString BarcodeCollisionHelper::formatStatus(BarcodeTable::Status st) {
    return (st == BarcodeTable::Status::Active) ? "Active" : "Retired";
}

std::optional<CsvImporter::RowError>
BarcodeCollisionHelper::makeBarcodeCollisionError(const QString& myTypeName,
                                                  const RowInfo& row,
                                                  int lineNumber)
{
    const QString code = row.barcode.trimmed();

    // Hunglish: üres kódra itt nem dobunk hibát – azt a hívó dönti el (korai validáció).
    if (code.isEmpty()) {
        return std::nullopt;
    }

    // Globális uniqueness check – ha nem unique, nézzük, mivel ütközik
    if (!BarcodeTable::instance().checkUnique(code, myTypeName, QUuid{})) {

        // Feljegyezzük auditban (EventLogger) – ez lehet INFO/WARN, itt WARNING
        zEventWARN(QString("Barcode collision detected: %1 (%2)").arg(code, myTypeName));

        if (auto rec = BarcodeTable::instance().find(code)) {
            const auto* otherEntity =
                RegistryManager::instance().findEntity(rec->entityType, rec->entityId);

            const QString status = formatStatus(rec->status);

            QString msg;
            if (otherEntity) {
                // Hunglish: emberbarát, kétoldalú kijelzés: saját + ütköző
                msg = QString("Barcode collision: %1[%2] ütközik %3[%4](%5)")
                          .arg(myTypeName)
                          .arg(formatRowDisplay(row))
                          .arg(rec->entityType)
                          .arg(otherEntity->toDisplay())
                          .arg(status);
            } else {
                // Fallback, ha a registry nem találja az ütköző entitást
                msg = QString("Barcode collision: %1[%2] ütközik %3{%4}(%5)")
                          .arg(myTypeName)
                          .arg(formatRowDisplay(row))
                          .arg(rec->entityType)
                          .arg(rec->entityId.toString(QUuid::WithoutBraces))
                          .arg(status);

            }

            return CsvImporter::RowError{ lineNumber, msg, row.barcode, row.name };
        } else {
            // Ritka helyzet: uniq false, de find nem találja → auditáljuk külön
            const QString msg = QString("Barcode collision: %1 (record not found after check)").arg(code);
            zEventERROR(QString("Inconsistent BarcodeTable state on collision: %1").arg(code));
            return CsvImporter::RowError{ lineNumber, msg, row.barcode, row.name };
        }
    }

    // Nincs ütközés → nincs hiba
    return std::nullopt;
}
