#pragma once
#include "common/logger/logger.h"
#include <QStringList>
#include "common/model/iregistry_entity.h"
#include "common/system/nameof.hpp"
#include "common/system/registry_catalog.h"
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

// Compile‑Time Enforced CRTP Self‑Registering Registry Pattern

template<typename T>
concept HasAutoRegister = requires {
    { T::_isAutoRegistered } -> std::convertible_to<bool>;
};

template<typename TDerived>
class RegisterMe {
public:
    inline static bool autoRegister() {
        RegistryCatalog::add<TDerived>();
        return true;
    }

    virtual bool __auto_register_marker() const = 0;
};

#define AUTO_REGISTER_REGISTRY(T) \
public: \
    bool __auto_register_marker() const override { return _isAutoRegistered; } \
private:\
    inline static bool _isAutoRegistered = T::autoRegister(); \
    static_assert(HasAutoRegister<T>); \

