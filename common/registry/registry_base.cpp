#include "registry_base.h"
#include "registry_manager.h"

RegistryBase::RegistryBase(const QString& registryName,
                           const QString& entityTypeName)
    : m_name(registryName),
    m_typeName(entityTypeName) {
    // minden registry automatikusan regisztrálja magát
    RegistryManager::instance().registerRepo(this);
}

IdentifiableRegistryBase::IdentifiableRegistryBase(const QString& registryName,
                                                   const QString& entityTypeName)
    : RegistryBase(registryName, entityTypeName) {
    RegistryManager::instance().registerRepo(this); // identifiable-be
}
