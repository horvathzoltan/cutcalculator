#pragma once
#include "registry_catalog.h"
//#include <cstring>
//#include "common/logger/logger.h"

// Compile‑Time Enforced CRTP Self‑Registering Registry Pattern

template<typename T>
concept HasRegistrationMacro =
    requires {
        { T::has_registration_macro } -> std::convertible_to<bool>;
    };

template<typename TDerived>
class RegisterMe {
public:
    using Derived = TDerived;
protected:
    RegisterMe() {
        ensure_macro(); // csak ellenőriz, nem regisztrál
    }
    virtual ~RegisterMe() = default;

private:
    static void ensure_macro() {
        static bool checked = []{
            static_assert(
                HasRegistrationMacro<TDerived>,
                "Hiányzik a AUTO_REGISTER_REGISTRY() makró ebből a registry-ből."
                );
            return true;
        }();
        (void)checked;
    }

public:
    inline static bool autoRegister() {

        RegistryCatalog::add<TDerived>();
        return true;
    }
};

/*
#define AUTO_REGISTER_REGISTRY(T) \
public: \
    static constexpr bool has_registration_macro = true;\
private: \
    inline static bool _isAutoRegistered = T::autoRegister();
*/

#define AUTO_REGISTER_REGISTRY \
public: \
    static constexpr bool has_registration_macro = true; \
private: \
    inline static bool _isAutoRegistered = RegisterMe<Derived>::autoRegister();

#define REGISTRY_CTOR(RegistryType, EntityType) \
private: \
    RegistryType() \
    : RegistryEngineBase(#RegistryType, #EntityType) \
{ \
        static_assert(std::is_class_v<RegistryType>, \
                      "REGISTRY_CTOR: first argument must be a class type (registry)"); \
        static_assert(std::is_class_v<EntityType>, \
                      "REGISTRY_CTOR: second argument must be an entity class/struct"); \
        static_assert(std::is_same_v<typename RegistryType::Derived, RegistryType>, \
                      "REGISTRY_CTOR: RegistryType must match the CRTP Derived type"); \
}
