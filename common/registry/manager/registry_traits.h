#pragma once

// registry_traits.h
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"

template<typename Entity> struct RegistryFor;

template<> struct RegistryFor<MaterialMaster> {
    using type = MaterialRegistry;
};

template<> struct RegistryFor<ProductMaster> {
    using type = ProductRegistry;
};
