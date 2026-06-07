#pragma once

#include <QString>
#include <QSizeF>
#include "common/model/barcode_identifiable_entity.h"
#include "colors/model/named_color.h"
#include "materials/model/material_type.h"
#include "materials/model/crosssectionshape.h"
#include "materials/model/cutting_mode.h"
#include "materials/model/painting_mode.h"


// 📦 Anyagdefiníció: szálhossz, forma, méret, szín, típus, súly, gép
struct MaterialMaster : public BarcodeIdentifiableEntity {
    MaterialMaster(){}; // 🔧 Default konstruktor deklaráció

    double stockLength_mm = 0.0;       // 📏 Teljes szálhossz mm-ben (pl. 6000)

    CrossSectionShape shape;           // 🧩 Keresztmetszet formája
    double diameter_mm = 0.0;          // ⚪ Kör formánál: átmérő
    QSizeF size_mm;                    // ▭ Téglalapnál: szélesség és magasság mm-ben

    //QString ralColorCode;              // 🎨 Szín RAL-kóddal (pl. "RAL 9006")
    MaterialType type;                 // 🧪 Anyagtípus: Aluminium / Steel / stb.
    //ProfileCategory category = ProfileCategory::Unknown; // 🗂️ Gyártási kategória

    double weightPerStock_kg = 0.0;    // ⚖️ Teljes szál súlya kg-ban
    QString defaultMachineId;          // ⚙️ Ajánlott gép az anyaghoz

    //QString coating;       // pl. fehér, szürke, szinterezhető
    NamedColor color; // 🎨 Anyag színe (RAL vagy HEX kód)

    PaintingMode paintingMode = PaintingMode::Paintable; // jelzi, ha az anyag festhető
    CuttingMode cuttingMode = CuttingMode::Length; // 🔧 Alapértelmezés: szálhossz vágás

    int trim_mm = 0;
    int minLeftOver_mm = 0;
    int scrap_mm = 0;
    int goodLeftOver_Min_mm = 0;
    int goodLeftOver_Max_mm = 0;
    QString externalCode;
    QString description;
};
