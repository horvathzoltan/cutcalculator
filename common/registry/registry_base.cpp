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


QString RegistryBase::logEntityAction(const QString& action,
                        const IRegistryEntity& e,
                        const QString& extra) const
{
    QStringList lines;

    lines << QString("[%1] %2 → %3")
                 .arg(m_name, action, e.displayName());
    lines << QString("  id: %1").arg(e.shortId());
    lines << QString("  type: %1").arg(m_typeName);

    if (!extra.isEmpty())
        lines << QString("  %1").arg(extra);

    for (const auto& l : lines)
        zInfo(l);

    return lines.join("\n");
}
