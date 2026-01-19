#pragma once

#include <QObject>

class QWidget;
class OverlayIconWidget;

class OverlayHelper : public QObject
{
    Q_OBJECT
public:
    explicit OverlayHelper(QObject* parent = nullptr);

    // 🟢 Létrehoz egy overlay widgetet a megadott alap emojival
    static OverlayIconWidget* createStatusWidget(const QString& baseEmoji);

    // 🟢 A view-hoz hozzáadja a status widgetet (jobb felső sarok)
    static void attachToView(QWidget* view, OverlayIconWidget* status);

    // 🟢 Egységes overlay frissítés repoCount / visibleRows alapján
    static void update(OverlayIconWidget* status,
                       int repoCount,
                       int visibleRows);

    // 🟡 Figyelmeztetés (csak overlay-emoji váltás)
    static void warning(OverlayIconWidget* status);

    // 🔴 Hibaállapot (csak overlay-emoji váltás)
    static void error(OverlayIconWidget* status);
};
