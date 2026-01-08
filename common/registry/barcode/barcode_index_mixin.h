#pragma once

#include <QHash>
#include <QString>

template<typename Derived, typename Entity>
class BarcodeIndexMixin {
public:
    BarcodeIndexMixin() {
        auto* self = static_cast<Derived*>(this);
        self->subscribeItemsChanged([this]() {
            this->buildBarcodeIndex();
        });
    }

    const Entity* findByBarcode(const QString& barcode) const {
        auto it = _barcodeIndex.find(barcode.toLower());
        return (it != _barcodeIndex.end()) ? it.value() : nullptr;
    }

protected:
    void buildBarcodeIndex() {
        _barcodeIndex.clear();
        auto* self = static_cast<const Derived*>(this);
        const auto all = self->readAll();
        for (const auto& e : all) {
            if (!e.barcode.isEmpty())
                _barcodeIndex.insert(e.barcode.toLower(), &e);
        }
    }

private:
    QHash<QString, const Entity*> _barcodeIndex;
};
