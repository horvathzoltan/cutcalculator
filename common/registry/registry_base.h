#pragma once
#include "common/model/identifiable_entity.h"
#include <QString>

/**
 * @brief RegistryBase – minden registry bázisosztálya.
 *
 * A konstruktor automatikusan regisztrálja magát a RegistryManager singletonba.
 */
class RegistryBase {
public:
    RegistryBase(const QString& registryName,
                 const QString& entityTypeName);
    virtual ~RegistryBase() = default;

    QString name() const { return m_name; }
    QString typeName() const { return m_typeName; }
    virtual int size() const = 0;

protected:
    QString m_name;      // Registry neve (pl. "MaterialRegistry")
    QString m_typeName;  // Entitás típusa (pl. "MaterialMaster")
};

class IdentifiableRegistryBase : public RegistryBase {
public:
    IdentifiableRegistryBase(const QString& registryName,
                             const QString& entityTypeName);

    using RegistryBase::RegistryBase;
    virtual const IdentifiableEntity* findEntityById(const QUuid& id) const = 0;
};
