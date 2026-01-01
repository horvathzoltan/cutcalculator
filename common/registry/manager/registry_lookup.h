// registry_lookup.h
#pragma once
//#include "connections/connection_registry.h"
#include "registry_manager.h"

template<typename RegistryType>
RegistryType* lookupRegistry() {
    for (auto* repo : RegistryManager::instance().allRepos()) {
        if (repo->typeName() == RegistryType::instance().typeName()) {
            return dynamic_cast<RegistryType*>(repo);
        }
    }
    return nullptr;
}
