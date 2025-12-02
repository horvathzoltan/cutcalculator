// registry_base.h
#pragma once
#include <QString>

/**
 * @brief RegistryBase – minden registry bázisosztálya.
 *
 * A konstruktor automatikusan regisztrálja magát a RegistryManager singletonba.
 */
class RegistryBase {
public:
    RegistryBase(const QString& name);
    virtual ~RegistryBase() = default;

    virtual QString name() const { return m_name; }
    virtual QString typeName() const = 0;
    virtual int size() const = 0;

protected:
    QString m_name;
};
