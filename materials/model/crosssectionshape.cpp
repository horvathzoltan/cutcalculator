#include "crosssectionshape.h"

CrossSectionShape::CrossSectionShape() : value(Shape::Custom) {}

CrossSectionShape::CrossSectionShape(Shape s) : value(s) {}

QString CrossSectionShape::toString() const {
    switch (value) {
    case Shape::Rectangular: return "Rectangular";
    case Shape::Round:       return "Round";
    case Shape::Custom:      return "Custom";
    default: return "Unknown";
    }
}

CrossSectionShape CrossSectionShape::fromString(const QString& str) {
    const QString s = str.trimmed().toLower();
    if (s == "rectangular") return CrossSectionShape(Shape::Rectangular);
    if (s == "round")       return CrossSectionShape(Shape::Round);
    if (s == "custom")       return CrossSectionShape(Shape::Custom);
    return CrossSectionShape(Shape::Unknown);
}

bool CrossSectionShape::operator==(const CrossSectionShape& other) const {
    return this->value == other.value;
}

