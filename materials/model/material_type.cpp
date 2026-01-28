#include "materials/model/material_type.h"

MaterialType::MaterialType() : value(Type::Unknown) {} // cpp-ben
MaterialType::MaterialType(Type t) : value(t) {}

QString MaterialType::toString() const {
    switch (value) {
    case Type::Aluminium: return "Aluminium";
    case Type::Steel:     return "Steel";
    case Type::Plastic:   return "Plastic";
    case Type::Composite: return "Composite";
    case Type::Other:     return "Other";
    default:   return "Unknown";
    }
}


MaterialType MaterialType::fromString(const QString& str) {
    if (str.compare("Aluminium", Qt::CaseInsensitive) == 0) return MaterialType(Type::Aluminium);
    if (str.compare("Steel", Qt::CaseInsensitive) == 0)      return MaterialType(Type::Steel);
    if (str.compare("Plastic", Qt::CaseInsensitive) == 0)    return MaterialType(Type::Plastic);
    if (str.compare("Composite", Qt::CaseInsensitive) == 0)  return MaterialType(Type::Composite);
    if (str.compare("Other", Qt::CaseInsensitive) == 0)      return MaterialType(Type::Other);

    return MaterialType(Type::Unknown);
}


bool MaterialType::operator==(const MaterialType& other) const {
    return this->value == other.value;
}
