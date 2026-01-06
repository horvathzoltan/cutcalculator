#pragma once

#include <QHash>
#include <QString>

/**
 * 🧩 BarcodeIndexMixin<Derived, Entity>
 *
 * Feltételek:
 *  - Derived örököl RegistryEngineBase<Entity>-ből (vagy kompatibilis basisból)
 *  - Entity rendelkezik `barcode` mezővel
 *  - Derived elérhetővé teszi:
 *      - readAll()
 *      - findIf()
 *      - onLoadLog() (opcionális)
 *
 * Eredmény:
 *  - O(1) barcode lookup
 *  - automatikus indexépítés load() után
 */
template<typename Derived, typename Entity>
class BarcodeIndexMixin {
public:
    // 🔍 O(1) lookup
    const Entity* findByBarcode(const QString& barcode) const
    {
        auto it = _barcodeIndex.find(barcode.toLower());
        return (it != _barcodeIndex.end()) ? it.value() : nullptr;
    }

protected:
    // 🧱 Index újraépítése
    void buildBarcodeIndex()
    {
        _barcodeIndex.clear();

        auto* self = static_cast<Derived*>(this);
        const auto all = self->readAll();

        for (const auto& e : all) {
            if (!e.barcode.isEmpty()) {
                _barcodeIndex.insert(e.barcode.toLower(), &e);
            }
        }
    }

    // 🔄 load() után hívandó
    void onAfterLoad()
    {
        buildBarcodeIndex();
    }

private:
    QHash<QString, const Entity*> _barcodeIndex;
};
