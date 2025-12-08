#pragma once
#include <QWidget>
#include <QColor>

/**
 * ColorBadge
 *
 * Small decorative widget for material color display.
 * - Draws a circle (ellipse) with given QColor.
 * - Sized to font metrics (caller decides).
 * - No stylesheet → clean tooltip rendering.
 *
 * Suggested placement: materials/view/
 * If reused later (products, logs), move to ui/widgets/.
 */
class ColorBadge : public QWidget {
    Q_OBJECT
public:
    /**
     * @param color   QColor to render
     * @param sizePx  fixed square size (width == height)
     * @param parent  QWidget parent
     */
    explicit ColorBadge(const QColor& color, const QString& tooltip, int sizePx, QWidget* parent = nullptr);

    /**
     * Provides a recommended size for layout managers.
     * Ensures the badge has a proper hover area for tooltips.
     */
    QSize sizeHint() const override;

protected:

    void paintEvent(QPaintEvent* event) override;

private:
    QColor _color;
    int _sizePx;
};
