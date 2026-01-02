#include "registry_base.h"
#include "common/system/verbose_manager.h"
#include "common/registry/manager/registry_manager.h"

RegistryBase::RegistryBase(const QString& registryName,
                           const QString& entityTypeName)
    : _name(registryName),
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
    if (!_isInitialized) {
        QString err = QString("❌ Registry '%1' instance() used before initialize()").arg(_name);
        if(IS_VERBOSE_THIS()){
            //zWarning(err);
        }else{
            zError(err);
        }

    }
}


QString RegistryBase::logEntityAction(const QString& action,
                        const IRegistryEntity& e,
                        const QString& extra) const
{
    QStringList lines;

    lines << QString("[%1] %2 → %3")
                 .arg(_name, action, e.displayName());
    lines << QString("  id: %1").arg(e.shortId());
    lines << QString("  type: %1").arg(_typeName);

    if (!extra.isEmpty())
        lines << QString("  %1").arg(extra);

    for (const auto& l : lines)
        zInfo(l);

    return lines.join("\n");
}
