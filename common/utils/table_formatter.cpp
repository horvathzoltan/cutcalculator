#include "common/utils/table_formatter.h"
#include <QtGlobal>
#include "common/system/verbose_manager.h"

/**
 * Unicode‑aware, auditbarát táblázat formázó.
 *
 * - Grapheme‑aware bejárás: surrogate párok, VS16, ZWJ láncok, skin tone, zászlók.
 * - Kétféle szélességszámítás:
 *   1) legacyVisualWidth: durvább becslés (CJK/emoji → szélesebb),
 *   2) renderVisualWidth: terminálbarát (emoji → 1, CJK → 2).
 * - Per‑cella kompenzáció: minden cellára kiszámoljuk a különbséget, oszloponként összegezzük.
 * - Fejléc, szeparátor, adatsorok egységesen paddelve.
 * - Kompenzáció és szélesség kiírása audit célra.
 */

// Grapheme detektálás
static inline bool isHigh(QChar c){ return c.isHighSurrogate(); }
static inline bool isLow(QChar c){ return c.isLowSurrogate(); }
static inline bool isVS16(QChar c){ return c.unicode() == 0xFE0F; }
static inline bool isZWJ(QChar c){ return c.unicode() == 0x200D; }
static inline bool isSkinTone(uint u){ return (u >= 0x1F3FB && u <= 0x1F3FF); }
static inline bool isRegionalIndicator(uint u){ return (u >= 0x1F1E6 && u <= 0x1F1FF); }

static inline bool isEmojiScalar(uint u) {
    return (u >= 0x1F300 && u <= 0x1FAFF)
    || (u >= 0x2700 && u <= 0x27BF)
        || (u >= 0x2600 && u <= 0x26FF);
}

static inline bool isCJKWide(uint u) {
    return (u >= 0x3400 && u <= 0x4DBF)
    || (u >= 0x4E00 && u <= 0x9FFF)
        || (u >= 0xFF01 && u <= 0xFF60)
        || (u >= 0xFFE0 && u <= 0xFFE6);
}

static uint consumeCluster(const QString& s, int& i) {
    uint u = s[i].unicode();
    if (isHigh(s[i]) && i+1 < s.size() && isLow(s[i+1])) i++;

    while (i+1 < s.size()) {
        QChar next = s[i+1];
        uint nu = next.unicode();
        if (isVS16(next) || isSkinTone(nu)) { i++; continue; }
        if (isZWJ(next)) {
            i++;
            if (i+1 < s.size() && isHigh(s[i+1]) && i+2 < s.size() && isLow(s[i+2])) i += 2;
            continue;
        }
        if (isRegionalIndicator(nu)) {
            if (i+2 < s.size() && isRegionalIndicator(s[i+2].unicode())) i += 2;
            break;
        }
        break;
    }
    return u;
}

static int legacyClusterWidth(uint u) {
    return (u >= 0x2E80) ? 2 : 1;
}

static int legacyVisualWidth(const QString& s) {
    int w = 0;
    for (int i = 0; i < s.size(); ++i)
        w += legacyClusterWidth(consumeCluster(s, i));
    return w;
}

static int renderClusterWidth(uint u) {
    if (isCJKWide(u)) return 2;
    if (isEmojiScalar(u)) return 2;
    return 1;
}

static int renderVisualWidth(const QString& s) {
    int w = 0;
    for (int i = 0; i < s.size(); ++i)
        w += renderClusterWidth(consumeCluster(s, i));
    return w;
}

static QString padToRenderWidthWithComp(QString s, int target, int compensation) {
    while (renderVisualWidth(s) < target - compensation)
        s += " ";
    s += QString(" ").repeated(compensation);
    return s;
}

QVector<QString> TableFormatter::format(const QVector<QString>& header,
                                        const QVector<QVector<QString>>& rows) {
    const int colCount = header.size();
    QVector<int> maxRender(colCount, 0);
    QVector<int> compSum(colCount, 0);
    QVector<int> printW(colCount, 0);
    QVector<QString> output;
    output.reserve(rows.size() + 6);

    // Fejléc render szélesség
    for (int i = 0; i < colCount; ++i)
        maxRender[i] = renderVisualWidth(header[i].trimmed());

    // Adatsorok: cellánkénti diff összegezve
    for (const auto& row : rows) {
        const int cols = qMin(row.size(), colCount);
        for (int i = 0; i < cols; ++i) {
            const QString t = row[i].trimmed();
            int rW = renderVisualWidth(t);
            int lW = legacyVisualWidth(t);
            maxRender[i] = qMax(maxRender[i], rW);
            compSum[i] += qMax(0, lW - rW);
        }
    }

    // Végső oszlopszélesség: render + kompenzáció
    for (int i = 0; i < colCount; ++i)
        printW[i] = maxRender[i] + compSum[i];

    auto makeLine = [&](const QVector<QString>& cols) {
        QString line;
        for (int i = 0; i < colCount; ++i) {
            const QString cell = (i < cols.size() ? cols[i] : QString()).trimmed();
            line += padToRenderWidthWithComp(cell, printW[i], compSum[i]);
            if (i < colCount - 1) line += " | ";
        }
        return line;
    };

    auto makeSeparator = [&]() {
        QString sep;
        for (int i = 0; i < colCount; ++i) {
            QString cell = padToRenderWidthWithComp("", printW[i], compSum[i]).replace(' ', '-');
            sep += cell;
            if (i < colCount - 1) sep += "-+-";
        }
        return sep;
    };

    auto makeAuditLine = [&](const QString& label, const QVector<int>& values) {
        QString line = label;
        for (int i = 0; i < colCount; ++i) {
            QString cell = QString::number(values[i]);
            cell = padToRenderWidthWithComp(cell, printW[i], compSum[i]);
            line += cell;
            if (i < colCount - 1) line += " | ";
        }
        return line;
    };

    output.push_back(makeLine(header));
    output.push_back(makeSeparator());

    if(IS_VERBOSE(TableFormatter)){
        output.push_back(makeAuditLine("Kompenzáció:", compSum));
        output.push_back(makeAuditLine("Szélesség:   ", printW));
        output.push_back(makeSeparator());
    }
    for (const auto& row : rows)
        output.push_back(makeLine(row));
    output.push_back(makeSeparator());

    return output;
}
