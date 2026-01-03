#pragma once
#include "common/model/iregistry_entity.h"
#include <QDebug>
/**
 * @brief RegistryBase – minden registry bázisosztálya.
 *
 * A registry NEM regisztrálja magát automatikusan.
 * A regisztráció explicit az initialize() hívással történik.
 */

class RegistryBase {
public:
    RegistryBase(const QString& registryName,
                 const QString& entityTypeName);
    virtual ~RegistryBase() = default;

    QString name() const { return _name; }
    QString typeName() const { return _typeName; }
    virtual int size() const = 0;

    // log wrapper
    QString logEntityAction(const QString& action,
                            const IRegistryEntity& e,
                            const QString& extra = QString()) const;

    // életciklus
    void initialize();
    bool isInitialized() const { return _isInitialized; }
    bool isRegistered() const { return _isRegistered; }

   // instance() guard – leszármazottak hívják az instance() getterben
    void guardInstanceUsage() const;

protected:
    QString _name;      // Registry neve (pl. "MaterialRegistry")
    QString _typeName;  // Entitás típusa (pl. "MaterialMaster")
    bool _isInitialized = false;
    bool _isRegistered= false;
};




