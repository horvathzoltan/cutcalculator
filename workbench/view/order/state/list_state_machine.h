#pragma once

#include "workbench/view/order/order_workbench_ui_model.h"
#include "orders/registry/order_header_registry.h"
#include <QToolBar>

namespace ListStateMachine {

enum class State {
    Hidden,
    Visible
};

// ─────────────────────────────────────────────────────────────
//  UI STATE STRUCT
// ─────────────────────────────────────────────────────────────
struct UiState {
    bool listVisible;
    bool placeholderVisible;

    bool addEnabled;
    bool deleteEnabled;
    bool renameEnabled;
    bool refreshEnabled;
};

// ─────────────────────────────────────────────────────────────
//  UI STATE MAP (DEKLARATÍV MÁTRIX)
// ─────────────────────────────────────────────────────────────
static const QMap<State, UiState> UI_STATE_MAP = {

{ State::Hidden,
    { false, true,
     true, false, false, true }   // refresh mindig engedélyezett
},

{ State::Visible,
    { true,  false,
            true,  true,  true,  true }
}
};

// ─────────────────────────────────────────────────────────────
//  RESOLVE (logikai állapotgép) – változatlan
// ─────────────────────────────────────────────────────────────
inline State resolve(
    const OrderWorkbenchUIModel& ui,
    const OrderHeaderRegistry& registry
    ) {
    if (registry.isEmpty())
        return State::Hidden;

    // if (ui.listPanel) {
    //     auto selectedId = ui.listPanel->selectedOrderId();
    //     if (!selectedId.has_value())
    //         return State::Visible;
    // }

    return State::Visible;
}

// ─────────────────────────────────────────────────────────────
//  APPLY (DEKLARATÍV) – nincs switch, nincs logika
// ─────────────────────────────────────────────────────────────
inline void apply(
    const OrderWorkbenchUIModel& ui,
    State s
    ) {
    const UiState u = UI_STATE_MAP.value(s);

    // LISTA + PLACEHOLDER
    ui.listPanel->setVisible(u.listVisible);
    ui.listPlaceholder->setVisible(u.placeholderVisible);

    // TOOLBAR - ha mindíg látszik, kell mindíg true???
    // if (ui.listToolBar)
    //     ui.listToolBar->setVisible(true);

    // LISTA TOOLBAR
    if (ui.listToolBar) {
        ui.listActions.setEnabled(ListAction::Add,    u.addEnabled);
        ui.listActions.setEnabled(ListAction::Delete, u.deleteEnabled);
        ui.listActions.setEnabled(ListAction::Rename, u.renameEnabled);
        ui.listActions.setEnabled(ListAction::Refresh, u.refreshEnabled);
    }
}

} // namespace ListStateMachine
