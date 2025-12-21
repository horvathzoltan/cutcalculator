#pragma once

#include <QMap>
#include <QString>
#include <string>                // std::string konverzióhoz
#include "common/system/nameof.hpp" // Neargye header-only lib

/**
 * @class VerboseManager
 * @brief Központi verbosity router – eldönti, hogy egy modul/osztály verbose logoljon-e.
 *
 * Koncepció:
 *  - Minden osztály regisztrálja magát névvel (nameof) + bool flaggel.
 *  - Az isVerbose() függvény auditbarát módon visszaadja, hogy kell-e részletes log.
 *  - Induláskor dumpRegistry() kiírja a teljes táblát → audit összefoglaló.
 *
 * Elhelyezés:
 *  - common/system/verbose_manager.h/.cpp
 *  - A többi system manager mellé (LifecycleManager, RegistryManager).
 *
 * Integráció:
 *  - Osztályok .cpp fájljában: REGISTER_VERBOSE(ClassName, defaultFlag)
 *  - Kód közben: IS_VERBOSE(ClassName) vagy isVerbose<T>() (static metódusokhoz)
 */

/*
Self-registration makró – minden osztály .cpp-ben használandó
#define REGISTER_VERBOSE(ClassName, flag) \
namespace { const bool _##ClassName##_verbose_registered = [](){ \
        VerboseManager::instance().registerClass( \
            QString::fromStdString(std::string(nameof::nameof_type<ClassName>())), \
            flag); \
        return true; }(); }
*/

// Rövidítés static metódusokhoz (nincs this)
#define IS_VERBOSE(ClassName) \
VerboseManager::instance().isVerbose<ClassName>()

// Rövidítés non-static metódusokhoz (van this)
#define IS_VERBOSE_THIS() \
    VerboseManager::instance().isVerbose<decltype(*this)>()

    class VerboseManager {
public:
    static VerboseManager& instance();

    void registerClass(const QString& className, bool verbose);
    bool isVerbose(const QString& className) const;

    template<typename T>
    bool isVerbose() const {
        return isVerbose(QString::fromStdString(std::string(nameof::nameof_type<T>())));
    }

    void dumpRegistry() const;

private:
    VerboseManager() = default;
    VerboseManager(const VerboseManager&) = delete;
    VerboseManager& operator=(const VerboseManager&) = delete;

    QMap<QString, bool> _map; ///< osztálynév → verbose flag
};
