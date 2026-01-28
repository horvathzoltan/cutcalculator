#pragma once

#include <QString>

class CrossSectionShape {
public:
    enum class Shape {
        Round,
        Rectangular,
        Custom,
        Unknown
    };


    Shape value;

    CrossSectionShape();                             // Default: Custom
    explicit CrossSectionShape(Shape s);             // Explicit konstruktor
    QString toString() const;                        // Enum → szöveg
    static CrossSectionShape fromString(const QString& str);  // Szöveg → enum
    bool operator==(const CrossSectionShape& other) const;

    bool isValid() const {
        return value == Shape::Round ||
               value == Shape::Rectangular ||
               value == Shape::Custom;
    }


    // 💡 Később jöhet: isValid(diameter, width, height)
    // QColor uiColor() const;
};
