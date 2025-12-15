#pragma once

#include <QString>
#include <QUuid>
#include <QDateTime>
#include <optional>

/**
 * BarcodeRecord – auditbarát életút modell egy barcode-hoz.
 *
 * - "introducedAt" a kiadás ideje (első regisztráció).
 * - "retiredAt" opcionális nyugdíjazási időpont.
 * - "entityType" emberbarát – pl. "Material", "Product".
 * - "entityId" belső trace, de nem része az export/import szerződésnek.
 * - "status" getterből számolódik → nincs redundáns mező.
 */
struct BarcodeRecord {
    QString code;
    QString entityType;
    std::optional<QUuid> entityId;                     // csak runtime trace
    QDateTime introducedAt;
    std::optional<QDateTime> retiredAt; // üres → Active

    enum class Status { Active, Retired };

    Status status() const {
        return retiredAt.has_value() ? Status::Retired : Status::Active;
    }

    bool isActive() const { return status() == Status::Active; }
};
