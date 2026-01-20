#include "overlay_icon_widget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QToolBar>
#include "common/logger/logger.h"
#include "common/system/verbose_manager.h"
#include "common/utils/filename_helper.h"
#include <QCryptographicHash>
#include <QDir>

// ------------------------------------------------------------
// Konstruktor
// ------------------------------------------------------------
OverlayIconWidget::OverlayIconWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

// ------------------------------------------------------------
// API
// ------------------------------------------------------------
void OverlayIconWidget::setBaseEmoji(const QString& emoji)
{
    _baseEmoji = emoji;
    update();
}

void OverlayIconWidget::setOverlay(Corner c, const QString& emoji, const QColor& color)
{
    _overlays[c].emoji = emoji;
    _overlays[c].color = color;
    update();
}

void OverlayIconWidget::setOverlay(Corner c, const QString& emoji)
{
    _overlays[c].emoji = emoji;
    _overlays[c].color = std::nullopt;
    update();
}

void OverlayIconWidget::clearOverlay(Corner c)
{
    _overlays[c].emoji.reset();
    update();
}

void OverlayIconWidget::setPadding(int px)
{
    _padding = px;
    update();
}

void OverlayIconWidget::setIconSize(int px)
{
    _iconSize = px;
    update();
}

void OverlayIconWidget::setLogicalSize(int px)
{
    _logicalSize = px;
    updateGeometry();
}

// ------------------------------------------------------------
// BaseRect kiszámítása (a dokumentum ikon valós helye)
// ------------------------------------------------------------
QRect OverlayIconWidget::computeBaseRect(const QFontMetrics& fm) const
{
    QRect bounds = fm.boundingRect(_baseEmoji);

    int baselineY = (height() + fm.ascent() - fm.descent()) / 2;
    int x = (width() - bounds.width()) / 2;
    int y = baselineY - fm.ascent();

    return QRect(QPoint(x, y), bounds.size());
}

// ------------------------------------------------------------
// Overlay negyedek a baseRect alapján
// ------------------------------------------------------------
QRect OverlayIconWidget::cornerRectRelativeToBase(Corner c, const QRect& baseRect, const QRect& overlayBounds) const
{
    const int p = _padding;
    QSize sz = overlayBounds.size();
    QPoint pos;
    QPoint sz2 = QPoint(sz.width()-p, sz.height()-p);

    switch (c) {
    case TopLeft:
        pos = baseRect.topLeft() - sz2 - QPoint(p, p);
        break;

    case TopRight:
        pos = baseRect.topRight() - QPoint(0, sz.height()) + QPoint(-sz.width(), -p) + QPoint(p, 0);
        break;

    case BottomLeft:
        pos = baseRect.bottomLeft() - QPoint(sz.width(), 0) + QPoint(-p, p);
        break;

    case BottomRight:
        pos = baseRect.bottomRight() - sz2 + QPoint(p, p);
        break;
    }

    return QRect(pos.x(), pos.y(), sz2.x(), sz2.y());
}



// ------------------------------------------------------------
// paintEvent
// ------------------------------------------------------------
void OverlayIconWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const int W = width();
    const int H = height();

    bool isVerbose = IS_VERBOSE_THIS();

    //const int S = qMin(W, H);

    // --------------------------------------------------------
    // Base emoji rajzolása (baseline + vízszintes közép)
    // --------------------------------------------------------
    QRect baseRect;

    if (!_baseEmoji.isEmpty()) {
        QFont f = painter.font();
        f.setPixelSize(H * 0.70);
        painter.setFont(f);

        QFontMetrics fm(f);
        // baseRect = computeBaseRect(fm);

        // painter.drawText(baseRect.bottomLeft() + QPoint(0, fm.ascent()), _baseEmoji);
        // QRect bounds = fm.boundingRect(_baseEmoji);
        // int x = (width() - bounds.width()) / 2;
        // int y = (height() + fm.ascent() - fm.descent()) / 2;

        // painter.drawText(QPoint(x, y), _baseEmoji);

        // baseRect = QRect(QPoint(x, y - fm.ascent()), bounds.size());
        const QSize S0 = size()*0.7;
        QPoint P0(W-S0.width(),H-S0.height());
        P0=P0/2;
        baseRect = QRect(P0, S0);
        //baseRect = QRect((W - W * 0.70) / 2, (H - H * 0.70) / 2, W * 0.70, H * 0.70);
        painter.drawText(baseRect, Qt::AlignCenter, _baseEmoji);
    }

    // --------------------------------------------------------
    // Overlay méret
    // --------------------------------------------------------
    int overlayCount = 0;
    for (int i = 0; i < 4; ++i)
        if (_overlays[i].emoji)
            overlayCount++;

    const qreal overlayRatio = (overlayCount == 1) ? 0.75 : 0.50;
    //const int overlaySize_H = H * overlayRatio;
    //const int overlaySize_W = W * overlayRatio;
    QSize overlay_Size = baseRect.size() * overlayRatio;

    // --------------------------------------------------------
    // Overlay rajzolása
    // --------------------------------------------------------
    for (int i = 0; i < 4; ++i) {
        const auto& ov = _overlays[i];
        if (!ov.emoji)
            continue;

        QFont f = painter.font();
        f.setPixelSize(overlay_Size.height());
        painter.setFont(f);

        QFontMetrics fm(f);
        QRect overlayBounds({0, 0}, overlay_Size);//fm.boundingRect(_baseEmoji);//*ov.emoji);
        QRect r = cornerRectRelativeToBase(static_cast<Corner>(i), baseRect, overlayBounds);

        if(isVerbose)
        {
            zInfo() << "baseRect = "
                    << baseRect.x() << "," << baseRect.y()
                    << "  " << baseRect.width() << "x" << baseRect.height();

            zInfo() << "overlayBounds = "
                    << overlayBounds.x() << "," << overlayBounds.y()
                    << "  " << overlayBounds.width() << "x" << overlayBounds.height();
        }

        QPixmap px = renderEmoji(*ov.emoji, overlay_Size, ov.color);
        painter.drawPixmap(r, px);


        if(isVerbose)
        {
            painter.setPen(Qt::red);
            painter.drawRect(baseRect);

            painter.setPen(Qt::blue);
            painter.drawRect(r); // ez a cornerRectRelativeToBase eredménye
        }
    }
}

// ------------------------------------------------------------
// Méretezés
// ------------------------------------------------------------
QSize OverlayIconWidget::sizeHint() const
{
    if (_logicalSize > 0)
        return QSize(_logicalSize, _logicalSize);

    if (auto tb = qobject_cast<QToolBar*>(parentWidget())) {
        int s = tb->iconSize().height();
        return QSize(s, s);
    }

    return QSize(24, 24);
}

QSize OverlayIconWidget::minimumSizeHint() const
{
    return QSize(24, 24);
}

// ------------------------------------------------------------
// Emoji monokróm-e?
// ------------------------------------------------------------
bool OverlayIconWidget::isMonochromeEmoji(const QString& emoji, const QSize& size) const
{
    QPixmap px(size);
    px.fill(Qt::transparent);

    QPainter p(&px);
    QFont f = p.font();
    f.setPixelSize(size.height());
    p.setFont(f);
    p.drawText(QRect({0, 0}, size), Qt::AlignBottom | Qt::AlignHCenter, emoji);
    p.end();

    QImage img = px.toImage();
    QColor first;
    bool firstSet = false;

    for (int y = 0; y < size.height(); ++y) {
        for (int x = 0; x < size.width(); ++x) {
            QColor c = img.pixelColor(x, y);
            if (c.alpha() == 0)
                continue;

            if (!firstSet) {
                first = c;
                firstSet = true;
            } else if (c != first) {
                return false;
            }
        }
    }

    return true;
}

// ------------------------------------------------------------
// Tintelő algoritmus (szürkeárnyalatos → színintenzitás)
// ------------------------------------------------------------
QPixmap OverlayIconWidget::tintedEmoji(const QString& emoji, const QSize& size, const QColor& color) const
{
    QPixmap base(size);
    base.fill(Qt::transparent);

    {
        QPainter p(&base);
        QFont f = p.font();
        f.setPixelSize(size.height());
        p.setFont(f);
        p.drawText(QRect({0, 0}, size), Qt::AlignBottom | Qt::AlignHCenter, emoji);
    }

    QImage src = base.toImage().convertToFormat(QImage::Format_ARGB32);
    QImage dst(src.size(), QImage::Format_ARGB32);
    dst.fill(Qt::transparent);

    for (int y = 0; y < src.height(); ++y) {
        for (int x = 0; x < src.width(); ++x) {
            QColor c = src.pixelColor(x, y);
            int alpha = c.alpha();
            if (alpha == 0)
                continue;

            int gray = qGray(c.red(), c.green(), c.blue());

            QColor tinted(
                color.red()   * gray / 255,
                color.green() * gray / 255,
                color.blue()  * gray / 255,
                alpha
                );

            dst.setPixelColor(x, y, tinted);
        }
    }

    return QPixmap::fromImage(dst);
}


QPixmap OverlayIconWidget::renderEmoji(const QString& emoji,
                                       const QSize& targetSize,
                                       const std::optional<QColor>& tint) const
{
    if (emoji.isEmpty() || targetSize.isEmpty())
        return QPixmap();

    // ------------------------------------------------------------
    // 0) Cache kulcs + fájlnév
    // ------------------------------------------------------------
    const QString key = emojiCacheKey(emoji, targetSize, tint);
    const QString fileName = emojiCacheFileName(key);

    // ------------------------------------------------------------
    // 1) Memória-cache
    // ------------------------------------------------------------
    if (auto it = _emojiCache.constFind(key); it != _emojiCache.constEnd()) {
        if (IS_VERBOSE_THIS()) {
            zInfo() << "renderEmoji: hit MEMORY cache"
                    << "key=" << key
                    << "size=" << targetSize;
        }
        return it.value();
    }

    // ------------------------------------------------------------
    // 2) Fájl-cache
    // ------------------------------------------------------------
    {
        QPixmap px;
        if (px.load(fileName)) {
            _emojiCache.insert(key, px);

            if (IS_VERBOSE_THIS()) {
                zInfo() << "renderEmoji: hit FILE cache"
                        << "key=" << key
                        << "size=" << targetSize
                        << "file=" << fileName;
            }

            return px;
        }
    }

    // ------------------------------------------------------------
    // 3) Super-sampling render (2× méret)
    // ------------------------------------------------------------
    const int scale = 2;
    const QSize bigSize = targetSize * scale;

    QPixmap base(bigSize);
    base.fill(Qt::transparent);

    {
        QPainter p(&base);
        QFont f = p.font();
        f.setPixelSize(bigSize.height());
        p.setFont(f);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::TextAntialiasing, true);
        p.drawText(QRect({0, 0}, bigSize), Qt::AlignBottom | Qt::AlignHCenter, emoji);
    }

    QImage src = base.toImage().convertToFormat(QImage::Format_ARGB32);

    // ------------------------------------------------------------
    // 4) Tight bounding box meghatározása
    // ------------------------------------------------------------
    int minX = bigSize.width();
    int minY = bigSize.height();
    int maxX = -1;
    int maxY = -1;

    for (int y = 0; y < src.height(); ++y) {
        const QRgb* line = reinterpret_cast<const QRgb*>(src.scanLine(y));
        for (int x = 0; x < src.width(); ++x) {
            const QColor c = QColor::fromRgba(line[x]);
            if (c.alpha() == 0)
                continue;

            minX = qMin(minX, x);
            minY = qMin(minY, y);
            maxX = qMax(maxX, x);
            maxY = qMax(maxY, y);
        }
    }

    if (minX > maxX || minY > maxY) {
        // semmi látható pixel – extrém fallback
        QPixmap empty(targetSize);
        empty.fill(Qt::transparent);
        return empty;
    }

    const QRect tight(minX, minY, maxX - minX + 1, maxY - minY + 1);
    QImage cropped = src.copy(tight);

    // ------------------------------------------------------------
    // 5) Tintelés (opcionális)
    // ------------------------------------------------------------
    if (tint.has_value()) {

        // Monokróm detektálás
        bool mono = true;
        QColor first;
        bool firstSet = false;

        for (int y = 0; y < cropped.height() && mono; ++y) {
            const QRgb* line = reinterpret_cast<const QRgb*>(cropped.scanLine(y));
            for (int x = 0; x < cropped.width(); ++x) {
                const QColor c = QColor::fromRgba(line[x]);
                if (c.alpha() == 0)
                    continue;

                if (!firstSet) {
                    first = c;
                    firstSet = true;
                } else if (c != first) {
                    mono = false;
                    break;
                }
            }
        }

        // Tintelés
        QImage tintedImg(cropped.size(), QImage::Format_ARGB32);
        tintedImg.fill(Qt::transparent);

        for (int y = 0; y < cropped.height(); ++y) {
            const QRgb* srcLine = reinterpret_cast<const QRgb*>(cropped.scanLine(y));
            QRgb* dstLine = reinterpret_cast<QRgb*>(tintedImg.scanLine(y));

            for (int x = 0; x < cropped.width(); ++x) {
                QColor c = QColor::fromRgba(srcLine[x]);
                int alpha = c.alpha();
                if (alpha == 0) {
                    dstLine[x] = qRgba(0, 0, 0, 0);
                    continue;
                }

                QColor tc;
                if (mono) {
                    tc = *tint;
                    tc.setAlpha(alpha);
                } else {
                    int gray = qGray(c.red(), c.green(), c.blue());
                    tc.setRed(  tint->red()   * gray / 255);
                    tc.setGreen(tint->green() * gray / 255);
                    tc.setBlue( tint->blue()  * gray / 255);
                    tc.setAlpha(alpha);
                }

                dstLine[x] = tc.rgba();
            }
        }

        cropped = tintedImg;
    }

    // ------------------------------------------------------------
    // 6) Visszakicsinyítés a targetSize-re
    // ------------------------------------------------------------
    QPixmap finalPx = QPixmap::fromImage(cropped)
                          .scaled(targetSize,
                                  Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);

    finalPx.setDevicePixelRatio(devicePixelRatioF());

    // ------------------------------------------------------------
    // 7) Cache-be mentés (memória + fájl)
    // ------------------------------------------------------------
    _emojiCache.insert(key, finalPx);
    finalPx.save(fileName, "PNG");

    if (IS_VERBOSE_THIS()) {
        zInfo() << "renderEmoji: MISS → rendered & saved"
                << "key=" << key
                << "size=" << targetSize
                << "file=" << fileName;
    }

    return finalPx;
}


QString OverlayIconWidget::ensureCacheDir() const
{
    if (_cacheDirInitialized)
        return _cacheDirPath;

    // FileNameHelper adja meg a cache gyökérkönyvtárat
    _cacheDirPath = FileNameHelper::instance().getCacheDirectory("emoji_overlay_cache");
    _cacheDirInitialized = true;
    return _cacheDirPath;
}



QString OverlayIconWidget::emojiCacheKey(const QString& emoji,
                                         const QSize& targetSize,
                                         const std::optional<QColor>& tint) const
{
    const QString tintPart = tint ? tint->name(QColor::HexArgb)
                                  : QStringLiteral("none");
    const qreal dpr = devicePixelRatioF();

    return QStringLiteral("%1|%2x%3|%4|dpr=%5")
        .arg(emoji)
        .arg(targetSize.width())
        .arg(targetSize.height())
        .arg(tintPart)
        .arg(dpr, 0, 'f', 2);
}



QString OverlayIconWidget::emojiCacheFileName(const QString& key) const
{
    const QString dir = ensureCacheDir();

    const QByteArray hash =
        QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha1).toHex();

    return QDir(dir).filePath(QString::fromLatin1(hash) + ".png");
}


// void OverlayIconWidget::updateOverlayState2(int repoCount, int totalRows)
// {
//     zTrace();
// }

 // zInfo() << "[Overlay:" << (objectName().isEmpty() ? parent()->objectName() : objectName()) << "]"
 //         << "repoCount=" << repoCount
 //         << "visibleRows=" << totalRows;

//     // Base emoji mindig dokumentum
//     setBaseEmoji("📄");

//     // --- Állapotok ---

//     // ❌ 1) Nincs adat a repóban → piros X
//     if (repoCount == 0) {
//         setOverlay(OverlayIconWidget::BottomRight, "❌");
//         return;
//     }

//     // 🔍 2) Van adat a repóban, de a UI nem mutat semmit → piros nagyító
//     if (totalRows == 0) {
//         //_statusWidget->setOverlay(OverlayIconWidget::BottomRight, "🔍", Qt::red);
//         setOverlay(OverlayIconWidget::BottomRight, "🟡");
//         return;
//     }

//     // 🟢 3) Van adat és látszik is → zöld pötty
//     setOverlay(OverlayIconWidget::BottomRight, "🟢");
// }
