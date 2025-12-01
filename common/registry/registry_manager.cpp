// #include "registry_base.h"
// #include "registry_manager.h"

// void RegistryManager::registerRepo(RegistryBase* repo) {
//     repos.append(repo);
// }

// void RegistryManager::report() const {
//     for (auto* repo : repos) {
//         qInfo() << "Registry:" << repo->name()
//         << "type=" << repo->typeName()
//         << "count=" << repo->count();
//     }
// }

// int RegistryManager::totalCount() const {
//     int sum = 0;
//     for (auto* repo : repos) {
//         sum += repo->count();
//     }
//     return sum;
// }
