#pragma once

#include <QString>

// 🔷 Objektum-orientált anyagtípus
class MaterialType {
public:
    // 🧩 Típusdefiníció
    enum class Type {
        Aluminium,
        Steel,
        Plastic,
        Composite,
        Other,
        Unknown
    };

    Type value; // 🔢 Tárolt enum érték

    MaterialType();                              // 🆕 Default konstruktor
    explicit MaterialType(Type t);               // Specifikus konstruktor
    QString toString() const;                              // 🔁 Enum → szöveg
    static MaterialType fromString(const QString& str);    // 🔁 Szöveg → enum

    bool operator==(const MaterialType& other) const;      // ➕ Egyenlőség

    bool isValid() const {
        return value != Type::Unknown;
    }

    // 🆘 Opcionális helper metódus később:
    // QColor uiColor() const;
};
