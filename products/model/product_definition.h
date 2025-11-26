// products/model/product_definition.h
#pragma once
#include "common/model/hierarchical_entity.h"

/**
 * ProductDefinition
 *
 * Hierarchikus terméktípus definíció (fa struktúra).
 * - Örökli az IdentifiableEntity és HierarchicalEntity mezőit (id, name, barcode, parentId).
 * - Később bővíthető CalculationRule listával (mérési módok, levonások, fix darabok).
 */
struct ProductDefinition : public HierarchicalEntity {
    // Később: QVector<CalculationRule> rules;
    // Most a fa felépítéséhez elég az ős által biztosított mezők.
};
