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
// Megjegyzés:
// Az Incomplete állapotot NEM a computeState() dönti el.
// A Presenter (pl. MaterialRequirementsPresenter) állítja be,
// tipikusan a MatrixValidator eredménye alapján.

class OverlayStatusHelper
{
public:
    enum class State {
        EmptyRepo,
        NoVisibleRows,
        Normal,
        Incomplete
    };


    // Állapot meghatározása
    static State computeState(int repoCount, int visibleRows);

    // Állapot alkalmazása a widgetre
    static void apply(OverlayIconWidget* w, State s);

};
