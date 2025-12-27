#pragma once

#include <QWidget>
#include <QPoint>

/* ============================================================
 * 🧩 CursorAwarePlacement – intelligens dialog pozicionálás
 * ============================================================ */

namespace CursorAwarePlacement {

/* ============================================================
     * 🧩 Smart placement API
     * ============================================================ */
void placeNearCursor(QWidget* widget,
                     const QPoint& offset = QPoint(20, 20));

}
