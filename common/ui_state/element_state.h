#pragma once
#include <QString>

enum class Visibility { Hidden, Visible };
enum class Enabledness { Disabled, Enabled };

struct ElementState
{
    std::optional<Visibility> visible = std::nullopt;//  Visibility::Hidden;
    std::optional<Enabledness> enabled = std::nullopt;//Enabledness::Disabled;
    std::optional<QString> text = std::nullopt;// {};

    // Rövid, kényelmes konstruktorok
    ElementState() = default;

    ElementState(Visibility v) : visible(v) {}

    ElementState(Enabledness v) : enabled(v) {}

    ElementState(Visibility v, Enabledness e)
        : visible(v), enabled(e) {}

    ElementState(Visibility v, Enabledness e, QString t)
        : visible(v), enabled(e), text(std::move(t)) {}

    ElementState(QString t) : text(std::move(t)) {}
};
