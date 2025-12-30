#pragma once

//#include "identifiable_entity.h"
//#include <QList>
#include <QUuid>

// 🔷 Hierarchikus törzselem – pl. terméktípus, kategória, fa-szerkezetű entitás
// - Örököl az BarcodeIdentifiableEntity-ből (id, name, barcode)
// - Tartalmaz szülő-gyerek viszonyokat
// - Alkalmas QTreeView megjelenítéshez, auditálható struktúrához

struct HierarchicalBase {
    QUuid parentId;              // 🌲 Szülő elem technikai azonosítója (GUID)
    //QList<QUuid> childrenIds;    // 🌿 Gyerekek azonosítói (GUID lista)

    // 🔍 Jelzi, hogy gyökérelem-e
    bool isRoot() const {
        return parentId.isNull();
    }

    // 🧭 Hierarchikus útvonal szövegesen (pl. "Roletta → Rugós → Tetőtéri")
    virtual QString hierarchyPath() const =0 ;
    //{
    //     return isRoot() ? name : parentId.toString(QUuid::WithoutBraces) + " → " + name;
    // }

    // 📦 Gyerekek száma
    // int childCount() const {
    //     return childrenIds.size();
    // }
};
