#pragma once

#include "common/logger/logger.h"
#include <QWidget>
#include <QIcon>
#include <optional>
#include <QPainter>

/**
 * @class OverlayIconWidget
 * @brief Egy tetszőleges widget fölé rajzolható 1–4 sarok-ikon/emoji overlay.
 *
 * Fő jellemzők:
 *  - Sarokpozíciók: TopLeft, TopRight, BottomLeft, BottomRight
 *  - Támogatott tartalom: QIcon vagy QString (emoji)
 *  - Automatikus retina-skálázás
 *  - Padding és ikonméret konfigurálható
 *  - Könnyen integrálható listákba, toolbarekbe, fába, táblába
 *
 * Használat:
 *   auto* ov = new OverlayIconWidget(parent);
 *   ov->setOverlay(OverlayIconWidget::TopRight, QIcon(":/icons/warning.svg"));
 *   ov->setOverlay(OverlayIconWidget::BottomLeft, QString("🟢"));
 */
class OverlayIconWidget : public QWidget {
    Q_OBJECT
public:
    enum Corner {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    explicit OverlayIconWidget(QWidget* parent = nullptr);

    // Base layer emoji
    void setBaseEmoji(const QString& emoji);

    // Overlay layer
    void setOverlay(Corner c, const QString& emoji, const QColor& color); // v2: tint is per-overlay, no global default
    void setOverlay(Corner c, const QString& emoji); // v2: no tint → render original emoji
    void clearOverlay(Corner c);

    void setPadding(int px);
    void setIconSize(int px);

    void setLogicalSize(int px);

    //void updateOverlayState2(int repoCount, int totalRows){zTrace();};
protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QString _baseEmoji; // v2: default base emoji = 📄 (document)

    struct OverlayInfo {
        std::optional<QString> emoji;
        std::optional<QColor> color;   // nullopt = nincs színezés
    };

    OverlayInfo _overlays[4];
    int _padding = 1;
    int _iconSize = 16;
    int _logicalSize =0;

    mutable QHash<QString, QPixmap> _emojiCache;      // memória-cache
    mutable bool _cacheDirInitialized = false;
    mutable QString _cacheDirPath;

    //    QRect cornerRect(Corner c, int size, const QFontMetrics& fm) const; // v2: legacy API, replaced by cornerRectRelativeToBase()
    QRect cornerRect(Corner c, int size) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    bool isMonochromeEmoji(const QString &emoji, const QSize&  size) const;
    QPixmap tintedEmoji(const QString &emoji, const QSize& size, const QColor &color) const;
    QRect cornerRectRelativeToBase(Corner c, const QRect& baseRect, const QRect& overlayBounds) const;
    QRect computeBaseRect(const QFontMetrics &fm) const;
    QPixmap renderEmoji(const QString &emoji, const QSize &targetSize, const std::optional<QColor>& tint) const;
    QString ensureCacheDir() const;
    QString emojiCacheKey(const QString &emoji, const QSize &targetSize, const std::optional<QColor> &tint) const;
    QString emojiCacheFileName(const QString& key) const;
};

