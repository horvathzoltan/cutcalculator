#pragma once

#include "common/system/class_name_helper.h"
#include "common/ui/crud/entity_toolbar_factory.h"
#include "common/ui_state/uistate_applier.h"
#include "workbench/view/order/order_workbench_ui_model.h"
#include "common/system/enum_utils.h"
#include <common/ui_state/uistate.h>

class ItemStateMachine {
zClassName

enum class State {
    Hidden,
    Visible,
    Editing
};

#define ITEM_UIELEMENT_LIST(X) \
    X(Table) \
        X(Placeholder) \
        X(Toolbar) \
        X(Add) \
        X(Delete)

zEnum(UiElement, ITEM_UIELEMENT_LIST);
zEnum_helpers(UiElement, ITEM_UIELEMENT_LIST);

// ─────────────────────────────────────────────────────────────
//  UI STATE MAP (DEKLARATÍV MÁTRIX)
// ─────────────────────────────────────────────────────────────

inline static const QMap<State, UiState<UiElement>> UI_STATE_MAP = {
    { State::Hidden,   {
                        { UiElement::Table,       Visibility::Hidden },
                        { UiElement::Placeholder, Visibility::Visible },
                        { UiElement::Toolbar,     Visibility::Hidden },
                        { UiElement::Add,         Enabledness::Disabled },
                        { UiElement::Delete,      Enabledness::Disabled }
                    } },
    { State::Visible, {
                         { UiElement::Table,       Visibility::Visible },
                         { UiElement::Placeholder, Visibility::Hidden },
                         { UiElement::Toolbar,     Visibility::Visible },
                         { UiElement::Add,         Enabledness::Enabled },
                         { UiElement::Delete,      Enabledness::Enabled }
                     } },
    { State::Editing, {
                         { UiElement::Table,       Visibility::Visible },
                         { UiElement::Placeholder, Visibility::Hidden },
                         { UiElement::Toolbar,     Visibility::Visible },
                         { UiElement::Add,         Enabledness::Enabled },
                         { UiElement::Delete,      Enabledness::Enabled }
                     } }
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

    widgets.insert(UiElement::Table,       ui.itemTable);
    widgets.insert(UiElement::Placeholder, ui.itemPlaceholder);
    widgets.insert(UiElement::Toolbar,     ui.itemToolbar);

    widgets.insert(UiElement::Add,         ui.itemActions[EntityAction::Add]);
    widgets.insert(UiElement::Delete,      ui.itemActions[EntityAction::Delete]);

    UiStateApplier::validateWidgets<UiElement, UiElement, UiElementHelpers>(widgets);
}

// ─────────────────────────────────────────────────────────────
//  RESOLVE (logikai állapotgép) – változatlan
// ─────────────────────────────────────────────────────────────

struct ResolveModel {
    bool registryIsEmpty;
    bool hasSelection;
};

inline State resolve(ResolveModel r) {
    if(widgets.isEmpty()){
        zWarning(L("%1::resolve called before init!").arg(className()));
    }

    if (r.registryIsEmpty)
        return State::Hidden;

    if (!r.hasSelection)
        return State::Hidden;

    return State::Visible;
}

// ─────────────────────────────────────────────────────────────
//  APPLY (DEKLARATÍV) – nincs switch, nincs logika
// ─────────────────────────────────────────────────────────────
inline void apply(State s) {
    if(widgets.isEmpty()){
        zWarning(L("%1::apply called before init!").arg(className()));
    }

    const UiState stateMap = UI_STATE_MAP.value(s);

    UiStateApplier::apply(stateMap, widgets);
}

};
