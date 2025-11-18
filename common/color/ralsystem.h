#pragma once
#include <QString>

enum class RalSystem {
    Classic,
    Design,
    Plastic1,
    Plastic2,
    Effect,
    Unknown
};

namespace RalSystemUtils {
inline QString toString(RalSystem system) {
    switch (system) {
    case RalSystem::Classic:  return "Classic";
    case RalSystem::Design:   return "Design";
    case RalSystem::Plastic1: return "Plastic 1";
    case RalSystem::Plastic2: return "Plastic 2";
    case RalSystem::Effect:   return "Effect";
    default:                  return "Unknown";
    }
}

inline RalSystem fromString(const QString& str) {
    QString s = str.trimmed().toLower();
    if (s == "classic")   return RalSystem::Classic;
    if (s == "design")    return RalSystem::Design;
    if (s == "plastic1" || s == "plastic 1") return RalSystem::Plastic1;
    if (s == "plastic2" || s == "plastic 2") return RalSystem::Plastic2;
    if (s == "effect")    return RalSystem::Effect;
    return RalSystem::Unknown;
}
}
