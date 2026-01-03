#pragma once

//void initializeAllRegistries();

// registry_catalog.h
#include <vector>
#include <functional>

#define REGISTER_REGISTRY_TYPE(T) \
static bool _auto_##T = [](){ \
        RegistryCatalog::add<T>(); \
        return true; \
}();

struct RegistryCatalog {
    static inline std::vector<std::function<void()>> factories;

    template<typename T>
    static void add() {
        factories.push_back([](){
            T::instance().initialize();
        });
    }

    static void initializeAll() {
        for (auto& f : factories)
            f();
    }
};
