#pragma once
#include "common/logger/logger.h"
//#include "common/model/barcode_identifiable_entity.h"
#include <QStringList>
#include "common/model/barcode_identifiable_entity.h"
#include "common/model/iregistry_entity.h"

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

    virtual const BarcodeIdentifiableEntity* findEntityById(const QUuid&) const {
        return nullptr;
    }


    //bool isInitialized(){return _isInitialized;}

protected:
    QString _name;      // Registry neve (pl. "MaterialRegistry")
    QString _typeName;  // Entitás típusa (pl. "MaterialMaster")
    bool _isInitialized = false;
    bool _isRegistered= false;

    // leszármazottak saját inicializációs hookja
//     virtual void onInitialize() {}

// // CRUD hookok a RegistryBase‑ben
//     virtual void onBeforeAdd(const IRegistryEntity&) {}
//     virtual void onAfterAdd(const IRegistryEntity&) {}

//     virtual void onBeforeUpdate(const IRegistryEntity&) {}
//     virtual void onAfterUpdate(const IRegistryEntity&) {}

//     virtual void onBeforeRemove(const IRegistryEntity&) {}
//     virtual void onAfterRemove(const IRegistryEntity&) {}

};


