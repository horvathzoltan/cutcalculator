#include "common/registry/feature/registry_base.h"
#include "common/system/verbose_manager.h"
#include "common/registry/manager/registry_manager.h"

// #include <execinfo.h>   // backtrace(), backtrace_symbols()
// #include <cstdlib>

// static void printStack() {
//     void* array[32];
//     int size = backtrace(array, 32);
//     char** symbols = backtrace_symbols(array, size);

//     zWarning("🔍 Stack trace:");
//     for (int i = 0; i < size; ++i) {
//         zWarning(symbols[i]);
//     }

//     free(symbols);
// }


RegistryBase::RegistryBase(const QString& registryName,
                           const QString& entityTypeName)
    : _registryName(registryName),
    _typeName(entityTypeName)
{
// fontos: itt NEM regisztrálunk!
}

void RegistryBase::initialize() {
    if (_isInitialized)
        return;

    //onInitialize(); // leszármazottak saját logikája (most még üres)

    RegistryManager::instance().registerRepo(this);

    _isInitialized = true;
    _isRegistered = true;
}

void RegistryBase::guardInstanceUsage() const {
    if (!_isInitialized && !_warned) {
        _warned = true;

        zWarning().noquote()
            << QString("❌ Registry '%1' used before initialize()")
                   .arg(_registryName);

        // if(_name == "NeedCalculationRegistry"){
        //     zWarning("krokodil");
        // }
        //printStack();
    }
}

QString RegistryBase::logEntityAction(const QString& action,
                        const IRegistryEntity& e,
                        const QString& extra) const
{
    QStringList lines;

    lines << QString("[%1] %2 → %3")
                 .arg(_registryName, action, e.displayName());
    lines << QString("  id: %1").arg(e.shortId());
    lines << QString("  type: %1").arg(_typeName);

    if (!extra.isEmpty())
        lines << QString("  %1").arg(extra);

    for (const auto& l : lines)
        zInfo(l);

    return lines.join("\n");
}


