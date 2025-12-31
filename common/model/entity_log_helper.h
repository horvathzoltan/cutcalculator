// #pragma once

// struct EntityLogHelper {
//     static QString logInsert(const BarcodeIdentifiableEntity& e,
//                              const QString& typeName)
//     {
//         return QString("%1:INSERT %2")
//         .arg(typeName)
//             .arg(e.displayName());
//     }

//     static QString logUpdate(const BarcodeIdentifiableEntity& e,
//                              const QString& typeName)
//     {
//         return QString("%1:UPDATE %2")
//         .arg(typeName)
//             .arg(e.displayName());
//     }

//     static QString logHierarchy(const BarcodeHierarchicalEntity& e,
//                                 const QString& typeName)
//     {
//         return QString("%1:HIER %2 → parent=%3")
//             .arg(typeName)
//             .arg(e.displayName())
//             .arg(e.parentBarcode);
//     }

//     static QString logDebug(const BarcodeIdentifiableEntity& e,
//                             const QString& typeName)
//     {
//         return QString("%1:DEBUG %2")
//         .arg(typeName)
//             .arg(e.toString());
//     }
// };
