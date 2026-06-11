#pragma once

#include "common/system/class_name_helper.h"
#include "common/ui_state/uistate.h"
#include "common/ui_state/uistate_applier.h"
#include "workbench/view/order/order_workbench_ui_model.h"
#include <QLabel>
#include <QToolBar>
#include "common/system/enum_utils.h"

class HeaderEditorStateMachine {
zClassName

enum class State {
    None,
    New,
    NewModified,
    Existing,
    ExistingModified
};

#define UIELEMENT_LIST(X) \
    X(HeaderPanel)               \
        X(HeaderPlaceholder)         \
        X(HeaderToolbar) \
        X(Modify)                    \
        X(Delete)                    \
        X(Save)                      \
        X(Cancel)

zEnum(UiElement, UIELEMENT_LIST);
zEnum_helpers(UiElement, UIELEMENT_LIST);

// ─────────────────────────────────────────────────────────────
//  UI STATE MAP (DEKLARATÍV MÁTRIX)
// ─────────────────────────────────────────────────────────────

inline static const QMap<State, UiState<UiElement>> UI_STATE_MAP = {

    {
      State::None, {
                      { UiElement::HeaderPanel, Visibility::Hidden },
                      { UiElement::HeaderPlaceholder, {Visibility::Visible, "📭 Nincs kiválasztott rendelés\n\nA bal oldali listából válassz egy rendelést,\nvagy kattints a „+ Új” gombra egy új rendelés létrehozásához." }},
                      { UiElement::HeaderToolbar, Visibility::Hidden },

                      { UiElement::Modify, Enabledness::Disabled },
                      { UiElement::Delete, Enabledness::Disabled },
                      { UiElement::Save,   Enabledness::Disabled },
                      { UiElement::Cancel, Enabledness::Disabled }
                  }
    },
    { State::New,
        {
            { UiElement::HeaderPanel,      Visibility::Visible },
            { UiElement::HeaderPlaceholder, {Visibility::Hidden, "📝 Új rendelés létrehozása folyamatban…" }},
            { UiElement::HeaderToolbar, Visibility::Visible },

            { UiElement::Modify, Enabledness::Disabled },
            { UiElement::Delete, Enabledness::Disabled },
            { UiElement::Save,   Enabledness::Disabled },
            { UiElement::Cancel, Enabledness::Disabled }
        }
    },

    { State::NewModified,
        {
            { UiElement::HeaderPanel,      Visibility::Visible },
            { UiElement::HeaderPlaceholder,{Visibility::Hidden, "📝 Új rendelés módosítva…\nMentéshez kattints a „Mentés” gombra." }},
            { UiElement::HeaderToolbar, Visibility::Visible },

            { UiElement::Modify, Enabledness::Disabled },
            { UiElement::Delete, Enabledness::Disabled },
            { UiElement::Save,   Enabledness::Enabled },
            { UiElement::Cancel, Enabledness::Enabled }
        }
    },

    { State::Existing,
        {
            { UiElement::HeaderPanel,      Visibility::Visible },
            { UiElement::HeaderPlaceholder,Visibility::Hidden },
            { UiElement::HeaderToolbar, Visibility::Visible },

            { UiElement::Modify, Enabledness::Enabled },
            { UiElement::Delete, Enabledness::Enabled },
            { UiElement::Save,   Enabledness::Disabled },
            { UiElement::Cancel, Enabledness::Disabled }
        }
    },

    { State::ExistingModified,
        {
                { UiElement::HeaderPanel,      Visibility::Visible },
                { UiElement::HeaderPlaceholder,Visibility::Hidden },
                { UiElement::HeaderToolbar, Visibility::Visible },

                { UiElement::Modify, Enabledness::Disabled },
                { UiElement::Delete, Enabledness::Disabled },
                { UiElement::Save,   Enabledness::Enabled },
            { UiElement::Cancel, Enabledness::Enabled }
        }
    }

};

// ─────────────────────────────────────────────────────────────
//  INIT
// ─────────────────────────────────────────────────────────────

inline static QMap<UiElement, QObject*> widgets;

public:
inline void init(const OrderWorkbenchUIModel& ui)
{
    if (!widgets.isEmpty())
        return;

    widgets.insert(UiElement::HeaderPanel,       ui.headerPanel);
    widgets.insert(UiElement::HeaderPlaceholder, ui.headerPlaceholder);
    widgets.insert(UiElement::HeaderToolbar, ui.headerToolbar);

    widgets.insert(UiElement::Modify, ui.headerActions[EntityAction::Modify]);
    widgets.insert(UiElement::Delete, ui.headerActions[EntityAction::Delete]);
    widgets.insert(UiElement::Save,   ui.headerActions[EntityAction::Save]);
    widgets.insert(UiElement::Cancel, ui.headerActions[EntityAction::Cancel]);

    UiStateApplier::validateWidgets<UiElement, UiElement, UiElementHelpers>(widgets);
};

// ─────────────────────────────────────────────────────────────
//  RESOLVE (logikai állapotgép) – változatlan
// ─────────────────────────────────────────────────────────────

struct ResolveModel{
    bool registryIsEmpty;
    bool modified;
    bool hasOriginal;
    bool existsInRegistry;
};

inline State resolve(ResolveModel r) {
    if(widgets.isEmpty()){
        zWarning(L("%1::resolve called before init!").arg(className()));
    }

    if (r.registryIsEmpty) {
        if (!r.hasOriginal)
            return State::None;
        return r.modified ? State::NewModified : State::New;
    }

    if (!r.hasOriginal)
        return State::None;

    if (!r.existsInRegistry)
        return r.modified ? State::NewModified : State::New;

    return r.modified ? State::ExistingModified : State::Existing;
}

// ─────────────────────────────────────────────────────────────
//  APPLY (DEKLARATÍV) – nincs switch, nincs logika
// ─────────────────────────────────────────────────────────────
inline void apply(State s)
{
    if(widgets.isEmpty()){
        zWarning(L("%1::apply called before init!").arg(className()));
    }

    const UiState stateMap = UI_STATE_MAP.value(s);

    UiStateApplier::apply(stateMap, widgets);
}


};
