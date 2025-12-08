#include "materials/view/color_badge.h"
#include <QPainter>
#include <QPaintEvent>

// ColorBadge::ColorBadge(const QColor& color, int sizePx, QWidget* parent)
//     : QWidget(parent), _color(color), _sizePx(sizePx)
// {
//     setFixedSize(_sizePx, _sizePx);
//     setToolTip(QString("Anyag színe: %1").arg(color.name()));
//     setAttribute(Qt::WA_TransparentForMouseEvents, true); // purely decorative
// }
ColorBadge::ColorBadge(const QColor& color, const QString& tooltip, int sizePx, QWidget* parent)
    : QWidget(parent), _color(color), _sizePx(sizePx)
{
    setFixedSize(_sizePx, _sizePx);
    setToolTip(tooltip);

    // Ne legyen transzparens az egér eseményekre → tooltip megjelenik
    // setAttribute(Qt::WA_TransparentForMouseEvents, true);  // törölve
}

QSize ColorBadge::sizeHint() const {
    return QSize(_sizePx, _sizePx); // biztos hover area
}


void ColorBadge::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    p.setBrush(_color);
    p.setPen(QPen(QColor("#888"), 1.0));

    // Circle badge
    p.drawEllipse(rect());

    // Alternative: rounded rect
    // int r = _sizePx / 2;
    // p.drawRoundedRect(rect(), r, r);
}
