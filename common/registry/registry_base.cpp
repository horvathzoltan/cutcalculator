// registry_base.cpp
#include "registry_base.h"
#include "registry_manager.h"

RegistryBase::RegistryBase(const QString& name)
    : m_name(name) {
    RegistryManager::instance().registerRepo(this);
}
