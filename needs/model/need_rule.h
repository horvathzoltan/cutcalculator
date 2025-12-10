#pragma once
#include "connections/connection_entity.h"

// Hunglish: ezek a típusok a te projektedben már léteznek.
// Elvárások (minimal):
//  - Product::registry().findById(QUuid) → std::optional<Product>
//  - Material::registry().findById(QUuid) → std::optional<Material>
#include "products/model/product_master.h"
#include "materials/model/material_master.h"

/**
 * 🧩 NeedRule – Product ↔ Material kapcsolat
 *
 * Hunglish: a "left" Product, a "right" Material. A convenience bal/jobb lookupok
 * automatikusan a megfelelő registryt hívják.
 */
using NeedRule = ConnectionEntity<ProductMaster, MaterialMaster>;
