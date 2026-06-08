#pragma once

#include "common/ui/crud/entity_toolbar_factory.h"
#include "workbench/view/order/order_workbench_ui_model.h"
#include "orders/registry/order_header_registry.h"
#include <QToolBar>

namespace ItemStateMachine {

enum class State {
    Hidden,
    Visible,
    Editing
};

// ─────────────────────────────────────────────────────────────
//  UI STATE STRUCT
// ─────────────────────────────────────────────────────────────
struct UiState {
    bool tableVisible;
    bool placeholderVisible;
    bool toolbarVisible;

    bool addEnabled;
    bool deleteEnabled;
};

// ─────────────────────────────────────────────────────────────
//  UI STATE MAP (DEKLARATÍV MÁTRIX)
// ─────────────────────────────────────────────────────────────
static const QMap<State, UiState> UI_STATE_MAP = {

{ State::Hidden,
    { false, true,  false,
     false, false }
},

    { State::Visible,
        { true,  false, true,
         true,  true }
    },

{ State::Editing,
    { true,  false, true,
            true,  true }
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

    if (ui.listPanel) {
        auto selectedId = ui.listPanel->selectedOrderId();
        if (!selectedId.has_value())
            return State::Hidden;
    }

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

    // TABLE + PLACEHOLDER
    ui.itemTable->setVisible(u.tableVisible);
    ui.itemPlaceholder->setVisible(u.placeholderVisible);

    // TOOLBAR
    if (ui.itemToolbar)
        ui.itemToolbar->setVisible(u.toolbarVisible);

    // TOOLBAR BUTTONS — ActionMap alapján, NEM index alapján
    if (ui.itemToolbar) {
        ui.itemActions.setEnabled(EntityAction::Add,    u.addEnabled);
        ui.itemActions.setEnabled(EntityAction::Delete, u.deleteEnabled);
    }
}

} // namespace ItemStateMachine
