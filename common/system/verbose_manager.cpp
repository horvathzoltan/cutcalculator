#include "common/system/verbose_manager.h"
#include "common/logger/logger.h"
#include "common/utils/table_formatter.h"

VerboseManager& VerboseManager::instance() {
    static VerboseManager mgr;
    return mgr;
}

void VerboseManager::registerClass(const QString& className, bool verbose) {
    _map[className] = verbose;
}

bool VerboseManager::isVerbose(const QString& className) const {
    auto it = _map.find(className);
    if (it == _map.end()) {
        // ⚠️ Audit jelzés – nincs regisztrálva
        zWarning(QString("VerboseManager: lekérdezés nem regisztrált osztályra → %1").arg(className));
        return true; // default: verbose ON
    }
    return it.value();
}

void VerboseManager::dumpRegistry() const {
    QVector<QString> header = {"ClassName", "Verbose"};
    QVector<QVector<QString>> rows;

    for (auto it = _map.constBegin(); it != _map.constEnd(); ++it) {
        rows.push_back({it.key(), it.value() ? "ON" : "OFF"});
    }

    auto lines = TableFormatter::format(header, rows);

    zInfo("📊 VerboseManager registry:");
    for (const auto& line : lines)
        zInfo().noquote() << line;
}
