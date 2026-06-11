#pragma once

#include <QString>

namespace ClassNameHelper{
inline QString extractClassName(const char* pretty)
{
    QString s = pretty;

    int pos = s.indexOf("::");
    if (pos < 0)
        return "UnknownClass";

    int space = s.lastIndexOf(' ', pos);
    if (space < 0)
        space = -1;

    return s.mid(space + 1, pos - (space + 1));
}
} // endof namespace ClassNameHelper

//#define zClassName inline static QString cachedClassName(ClassNameHelper::extractClassName(__PRETTY_FUNCTION__));
#define zClassName \
inline static const QString& className() { \
        static QString cachedClassName = ClassNameHelper::extractClassName(__PRETTY_FUNCTION__); \
        return cachedClassName; \
}
