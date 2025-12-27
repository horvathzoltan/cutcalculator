#include "cursor_aware_placement.h"
#include <QCursor>
#include <QGuiApplication>
#include <QScreen>
#include <algorithm>

/* ============================================================
 * 🧩 Smart placement implementation
 * ============================================================ */
namespace CursorAwarePlacement {

/* ============================================================
     * 🧩 placeNearCursor – intelligens, többmonitoros pozicionálás
     * ============================================================ */
void placeNearCursor(QWidget* widget, const QPoint& offset)
{
    if (!widget)
        return;

    QPoint cursorPos = QCursor::pos();
    QSize dlgSize = widget->sizeHint();

    // A képernyő, amelyen az egér található
    QScreen* screen = QGuiApplication::screenAt(cursorPos);
    if (!screen)
        return;

    QRect screenRect = screen->geometry();

    // Négy lehetséges pozíció az egér körül
    QPoint candidates[4] = {
        cursorPos + QPoint(offset.x(),  offset.y()),              // jobbra + lefelé
        cursorPos + QPoint(offset.x(), -dlgSize.height() - offset.y()), // jobbra + felfelé
        cursorPos + QPoint(-dlgSize.width() - offset.x(), offset.y()),  // balra + lefelé
        cursorPos + QPoint(-dlgSize.width() - offset.x(),
                           -dlgSize.height() - offset.y())        // balra + felfelé
    };

    // Válasszuk ki az első olyan pozíciót, ahol a dialog teljesen látszik
    for (const QPoint& p : candidates) {
        QRect dlgRect(p, dlgSize);
        if (screenRect.contains(dlgRect)) {
            widget->move(p);
            return;
        }
    }

    // Ha egyik sem jó → clamp a képernyőn belülre
    int x = std::clamp(cursorPos.x(), screenRect.left(),
                       screenRect.right() - dlgSize.width());
    int y = std::clamp(cursorPos.y(), screenRect.top(),
                       screenRect.bottom() - dlgSize.height());

    widget->move(QPoint(x, y));
}

}
