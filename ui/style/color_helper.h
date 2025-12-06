#pragma once
#include <QApplication>
#include <QPalette>
#include <QWidget>
#include "colors/model/colorconstants.h"

/**
 * ColorHelper
 *
 * Feladata: a Qt paletta alapján eldönteni, hogy dark vagy light stílus van,
 * és ennek megfelelően visszaadni a szövegszínt.
 */
namespace ColorHelper {

/**
 * Megállapítja, hogy a widget (vagy az app) dark mode-ban van-e.
 * - A Window háttér fényességét nézi (HSV value).
 */
inline bool isDarkMode(const QWidget* widget = nullptr) {
    QColor bg = widget
                    ? widget->palette().color(QPalette::Window)
                    : QApplication::palette().color(QPalette::Window);
    return bg.value() < 128; // 0..255 fényesség
}

/**
 * Visszaadja a megfelelő szövegszínt a stílushoz.
 * - Dark mode → fehér
 * - Light mode → fekete
 */
inline QColor textColor(const QWidget* widget = nullptr) {
    return isDarkMode(widget) ? Qt::white : ColorConstants::TextBlack;
}

/**
 * Visszaadja a megfelelő kiemelőszínt kategóriákhoz.
 * - Dark mode → cián
 * - Light mode → narancs
 */
inline QColor categoryColor(const QWidget* widget = nullptr) {
    return isDarkMode(widget) ? ColorConstants::ColorCyan : ColorConstants::ColorOrange;
}

} // namespace ColorHelper
