// products/model/product_master.h
#pragma once
#include "common/model/barcode_hierarchical_entity.h"

/**
 * ProductMaster
 *
 * Hierarchikus terméktípus definíció (fa struktúra).
 * - Örökli az IdentifiableEntity és HierarchicalEntity mezőit (id, name, barcode, parentId).
 * - Később bővíthető CalculationRule listával (mérési módok, levonások, fix darabok).
 */
struct ProductMaster : public BarcodeHierarchicalEntity {
    // Később: QVector<CalculationRule> rules;
    // Most a fa felépítéséhez elég az ős által biztosított mezők.
};
