#pragma once
#include <optional>
#include <QString>
#include <QUuid>
#include <QDateTime>

namespace OptionalUtils {

// Generikus sablon
template <typename T, typename F>
inline QString toString(const std::optional<T>& opt,
                        F&& conv,
                        const QString& empty = "<none>")
{
    return opt.has_value() ? conv(*opt) : empty;
}


// Gyakori típusokra overload
inline QString toString(const std::optional<QUuid>& opt,
                           const QString& empty = "<no-entity>")
{
    return opt.has_value() ? opt->toString(QUuid::WithoutBraces) : empty;
}

inline QString toString(const std::optional<QDateTime>& opt,
                           const QString& empty = "")
{
    return opt.has_value() ? opt->toString(Qt::ISODate) : empty;
}

} // namespace OptonalUtils
