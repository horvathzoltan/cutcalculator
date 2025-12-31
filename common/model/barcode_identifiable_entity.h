#pragma once

#include "barcode_base.h"
#include "identifiable_entity.h"

// 🔷 Alapstruktúra az összes azonosítható törzselemhez:
// - Minden olyan entitás, ami rendelkezik technikai ID-val, névvel, vonalkóddal
// - Alkalmas törzsregiszterekhez, UI megjelenítéshez, címkézéshez

struct BarcodeIdentifiableEntity : public IdentifiableEntity, public BarcodeBase {
//    QUuid id;           // 🆔 Technikai, rendszer által generált egyedi azonosító (általában GUID)
//    QString name;       // 📛 Emberbarát megnevezés, pl. UI-ban megjelenő cím
    //QString barcode;    // 🏷️ Nyomtatott, beolvasott vagy egyéb fizikai kód (általában "MAT-..." vagy egyedi string)

    // 🖼️ Vizuális, felhasználóbarát megjelenítési név (pl. listában)
    QString displayName() const override {
        return barcode.isEmpty()
                   ? name
                   : QString("%1 [%2]").arg(name, barcode);
    }

    // 🧾 Teljes technikai szöveges reprezentáció – log, export, debug célra
    QString toString() const override {
        return QString("{id=%1, name=%2, barcode=%3}")
        .arg(id.toString(), name, barcode);
    }

    // // 🏷️ Opcionális címkekód (pl. barcode, de később override-olható)
    // QString labelCode() const {
    //     return barcode;
    // }

    // // 🔢 Rövidített ID (pl. QR vagy logokban csak az első 8 karakter)
    // QString shortId() const {
    //     return id.toString(QUuid::WithoutBraces).left(8);
    // }
};
