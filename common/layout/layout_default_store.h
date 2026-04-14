#pragma once

#include <QString>
//#include "common/settings/settings_manager.h"

/**
 * @class LayoutDefaultStore
 * @brief UI layout fallback értékek (percent-based) tárolása settings.ini-ben.
 *
 * Felelősség:
 * - Window geometry, splitter és header percent stringek tárolása.
 * - Snapshot hiányában ezekre a baseline értékekre fallbackolunk.
 * - Thin wrapper a SettingsManager kulcsai fölött, UI-specifikus API-val.
 *
 * Design:
 * - Singleton pattern (ugyanúgy, mint SettingsManager).
 * - Nem használ közvetlenül QSettings-et, csak a SettingsManager API-t.
 */
class LayoutDefaultStore
{
public:
    static LayoutDefaultStore& instance();

    // Window geometry fallback (percent-based)
    QString windowGeometryPercent() const;
    void setWindowGeometryPercent(const QString& s);

    QString screenSizeString() const;
    void setScreenSizeString(const QString& s);

    // MainWindow fő splitter fallback
    QString mainSplitterPercent() const;
    void setMainSplitterPercent(const QString& s);

    // BOMWorkbench splitters fallback
    QString productTypesSplitterPercent() const;
    void setProductTypesSplitterPercent(const QString& s);

    QString leftVerticalSplitterPercent() const;
    void setLeftVerticalSplitterPercent(const QString& s);

    QString rightVerticalSplitterPercent() const;
    void setRightVerticalSplitterPercent(const QString& s);

    // BOMWorkbench tree header fallback
    QString productTreeHeaderPercent() const;
    void setProductTreeHeaderPercent(const QString& s);

    QString formulaEditorGeometryPercent() const;
    void setFormulaEditorGeometryPercent(const QString& s);

    /// Explicit mentés settings.ini-be
    void flush();

    QString calcDetailHeaderPercent() const;
    void setCalcDetailHeaderPercent(const QString &s);
private:
    LayoutDefaultStore() = default;
};
