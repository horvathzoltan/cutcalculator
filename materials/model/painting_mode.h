// paintability.h
#pragma once

#include <QString>


enum class PaintingMode {
    None,        // 🚫 Nem festhető
    Paintable,   // 🎨 Festhető általánosan
    Coatable,    // 🧪 Szinterezhető / bevonható
    PreCoated,   // 🟦 Már bevonattal érkezik
    Unknown
};

namespace PaintingModeUtils {
    inline QString toString(PaintingMode p) {
        switch (p) {
        case PaintingMode::None:      return "None";
        case PaintingMode::Paintable: return "Paintable";
        case PaintingMode::Coatable:  return "Coatable";
        case PaintingMode::PreCoated: return "PreCoated";
        default: return "Unknown";
        };
    }

    inline PaintingMode parse(const QString& str) {
        if (str.compare("None", Qt::CaseInsensitive) == 0)      return PaintingMode::None;
        if (str.compare("Paintable", Qt::CaseInsensitive) == 0) return PaintingMode::Paintable;
        if (str.compare("Coatable", Qt::CaseInsensitive) == 0)  return PaintingMode::Coatable;
        if (str.compare("PreCoated", Qt::CaseInsensitive) == 0) return PaintingMode::PreCoated;
        return PaintingMode::Unknown; // fallback
    }

} //end namespace PaintabilityUtils
