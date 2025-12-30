#pragma once

#include "barcode_base.h"
#include "barcode_identifiable_entity.h"
#include "hierarchical_entity.h"
//#include "barcode_identifiable_entity.h"

struct BarcodeHierarchicalEntity: public BarcodeIdentifiableEntity, public HierarchicalBase
{
// NINCS új mező
    // NINCS duplikált logika
// NINCS redundancia //

//A displayName() a BarcodeIdentifiableEntity-ből jön
// A hierarchyPath() a HierarchicalEntity-ből jön
// 🧭 Hierarchikus útvonal szövegesen (pl. "Roletta → Rugós → Tetőtéri")
    QString hierarchyPath() const override {
        return isRoot() ? name : parentId.toString(QUuid::WithoutBraces) + " → " + name;
    }
};
