// #pragma once
// #include "registry_manager.h"

// template<typename RegistryType>
// RegistryType* lookupRegistry() {
//     static bool warned_missing = false;
//     static bool warned_cast = false;
//     static bool warned_multiple = false;

//     const QString expectedType = RegistryType::instance().typeName();
//     RegistryType* found = nullptr;
//     int matchCount = 0;

//     for (auto* repo : RegistryManager::instance().allRepos()) {
//         if (repo->typeName() == expectedType) {
//             ++matchCount;
//             if (auto* casted = dynamic_cast<RegistryType*>(repo)) {
//                 if (!found)
//                     found = casted;
//             } else {
//                 if (!warned_cast) {
//                     warned_cast = true;
//                     zWarning().noquote() << QString("⚠️ lookupRegistry: typeName match, but dynamic_cast failed. expected='%1', repo='%2'").arg(expectedType, repo->name());
//                 }
//             }
//         }
//     }

//     if (matchCount > 1 && !warned_multiple) {
//         warned_multiple = true;
//         zWarning().noquote() << QString("⚠️ lookupRegistry: multiple registries found for typeName='%1' (%2 matches)").arg(expectedType).arg(matchCount);
//     }

//     if (!found && !warned_missing) {
//         warned_missing = true;
//         zWarning().noquote() << QString("⚠️ lookupRegistry: registry not found for typeName='%1'. Did you forget inst.initialize()?").arg(expectedType);
//     }

//     return found;
// }
