#pragma once

#include "common/system/nameof.hpp"
#include <QString>


template<typename T>
inline QString verboseTypeName() {
    return QString::fromStdString(std::string(nameof::nameof_type<T>()));
}

struct VerboseEntry {
    QString className;
    bool verbose;
};

template<typename T>
inline VerboseEntry VERBOSE_ON() {
    return VerboseEntry{ verboseTypeName<T>(), true };
}

template<typename T>
inline VerboseEntry VERBOSE_OFF() {
    return VerboseEntry{ verboseTypeName<T>(), false };
}





void registerAllVerbose();
