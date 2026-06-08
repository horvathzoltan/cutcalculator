#pragma once

#include "workbench/view/order/order_workbench_ui_model.h"
#include "orders/registry/order_header_registry.h"
#include <optional>
#include <QLabel>
#include <QToolBar>

namespace HeaderEditorStateMachine {

enum class State {
    None,
    New,
    NewModified,
    Existing,
    ExistingModified
};

// ─────────────────────────────────────────────────────────────
//  UI STATE STRUCT
// ─────────────────────────────────────────────────────────────
struct UiState {
    bool headerVisible;
    bool placeholderVisible;

    bool modifyEnabled;
    bool deleteEnabled;
    bool saveEnabled;
    bool cancelEnabled;

    QString placeholderText;
};

// ─────────────────────────────────────────────────────────────
//  UI STATE MAP (DEKLARATÍV MÁTRIX)
// ─────────────────────────────────────────────────────────────
static const QMap<State, UiState> UI_STATE_MAP = {

{ State::None,
    { false, true,
        false, false, false, false,
        "📭 Nincs kiválasztott rendelés\n\n"
        "A bal oldali listából válassz egy rendelést,\n"
        "vagy kattints a „+ Új” gombra egy új rendelés létrehozásához."
    }
},

    { State::New,
        { true, false,
            false, false, false, false,
            "📝 Új rendelés létrehozása folyamatban…"
        }
    },

    { State::NewModified,
        { true, false,
            false, false, true, true,
            "📝 Új rendelés módosítva…\n"
            "Mentéshez kattints a „Mentés” gombra."
        }
    },

    { State::Existing,
        { true, false,
         true, true, false, false,
         "" }
    },

{ State::ExistingModified,
        { true, false,
         false, false, true, true,
         "" }
}
};

// ─────────────────────────────────────────────────────────────
//  RESOLVE (logikai állapotgép) – változatlan
// ─────────────────────────────────────────────────────────────
inline State resolve(
    const OrderWorkbenchUIModel& ui,
    const OrderHeaderRegistry& registry,
    bool modified
    ) {
    std::optional<OrderHeader> originalOpt;
    if (ui.headerPanel)
        originalOpt = ui.headerPanel->originalHeader();

    if (registry.isEmpty()) {
        if (!originalOpt)
            return State::None;
        return modified ? State::NewModified : State::New;
    }

    if (!originalOpt)
        return State::None;

    const OrderHeader& original = *originalOpt;
    bool exists = registry.existsById(original.id);

    if (!exists)
        return modified ? State::NewModified : State::New;

    return modified ? State::ExistingModified : State::Existing;
}

// ─────────────────────────────────────────────────────────────
//  APPLY (DEKLARATÍV) – nincs switch, nincs logika
// ─────────────────────────────────────────────────────────────
inline void apply(
    const OrderWorkbenchUIModel& ui,
    State s
    ) {
    const UiState u = UI_STATE_MAP.value(s);

    // PANEL / PLACEHOLDER
    ui.headerPanel->setVisible(u.headerVisible);
    ui.headerPlaceholder->setVisible(u.placeholderVisible);

    // TOOLBAR
    // TOOLBAR BUTTONS — ActionMap alapján, NEM findChild alapján
    if(ui.headerToolbar){
        ui.headerActions.setEnabled(EntityAction::Modify, u.modifyEnabled);
        ui.headerActions.setEnabled(EntityAction::Delete, u.deleteEnabled);
        ui.headerActions.setEnabled(EntityAction::Save,   u.saveEnabled);
        ui.headerActions.setEnabled(EntityAction::Cancel, u.cancelEnabled);
    }

    // PLACEHOLDER TEXT
    if (auto* lbl = ui.headerPlaceholder->findChild<QLabel*>()) {
        lbl->setText(u.placeholderText);
    }
}

} // namespace HeaderEditorStateMachine
