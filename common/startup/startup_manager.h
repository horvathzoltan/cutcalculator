#pragma once

#include "startup_status.h"

#include <QString>
#include <QStringList>

namespace StatusHelper{
inline QString getMessage(bool b, const QString& msg){
    return (b?"✅ ":"❌ ")+msg+(b?" ok":" sikertelen");
    }
}

// 🚀 Indítási logikát vezérlő osztály
class StartupManager {
public:
    StartupStatus runStartupSequence();

private:

    bool hasMinimumMaterials(int minCount);

    StartupStatus initMaterialRegistry();
    StartupStatus initRalColors();
};
