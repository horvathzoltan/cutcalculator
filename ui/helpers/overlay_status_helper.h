#pragma once

class OverlayIconWidget;

/**
 * @class OverlayStatusHelper
 * @brief Presenter-szintű overlay policy és állapotkezelő.
 *
 * Ez az osztály kizárólag az overlay állapotának meghatározásáért
 * és az OverlayIconWidget-re történő alkalmazásáért felel.
 *
 * Felelősségei:
 *   - repoCount és visibleRows alapján overlay állapot számítása
 *   - az állapotnak megfelelő overlay emoji beállítása a widgeten
 *
 * Nem feladata:
 *   - widget létrehozása vagy elhelyezése (UI helper dolga)
 *   - rajzolás vagy megjelenítés (OverlayIconWidget dolga)
 *   - registry vagy view kezelése (Presenter dolga)
 *
 * A cél egy egységes, toronyfüggetlen overlay policy biztosítása,
 * amely minden Presenter-ben ugyanúgy működik.
 */

class OverlayStatusHelper
{
public:
    enum class State {
        EmptyRepo,      // nincs adat a repóban
        NoVisibleRows,  // van adat, de a UI nem mutat semmit
        Normal          // van adat és látszik is
    };

    // Állapot meghatározása
    static State computeState(int repoCount, int visibleRows);

    // Állapot alkalmazása a widgetre
    static void apply(OverlayIconWidget* w, State s);

};
