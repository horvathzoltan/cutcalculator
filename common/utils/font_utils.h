#pragma once
#include <QWidget>
#include <QFont>

namespace FontUtils {
// 🆕 Biztonságos monospace font beállítása
// - Elsőként próbálja a Fira Code-ot
// - Ha nincs, akkor Consolas, Source Code Pro, Roboto Mono
// - Végül fallback: "monospace" (Qt a rendszer alap monospace fontját adja)
inline void applySafeMonospaceFont(QWidget* widget, int pointSize = 10) {
    QFont font;
    font.setFamilies({"Fira Code", "Consolas", "Source Code Pro", "Roboto Mono", "monospace"});
    font.setPointSize(pointSize);
    widget->setFont(font);
}


struct CharMetrics {
    int height;       // teljes betűmagasság (ascent + descent)
    int ascent;       // alapvonal feletti rész
    int descent;      // alapvonal alatti rész
    int width;        // adott karakter szélessége
};

// Lekéri egy widget fontjára nézve a metrikákat.
// Paraméter: a karakter, pl. "W" vagy "M".
inline CharMetrics getCharMetrics(QWidget* widget, const QChar& ch = QChar('W')) {
    QFontMetrics fm(widget->font());
    CharMetrics m;
    m.height  = fm.height();
    m.ascent  = fm.ascent();
    m.descent = fm.descent();
    m.width   = fm.horizontalAdvance(ch);
    return m;
}

} // namespace FontUtils
