#pragma once

#include "common/ui_state/element_state.h"
#include "common/ui_state/uistate.h"
#include "common/ui_state/uistate_applier.h"
#include "workbench/view/order/order_workbench_ui_model.h"
#include "common/system/enum_utils.h"
#include "common/system/class_name_helper.h"

class ListStateMachine {

zClassName

enum class State {
    Hidden,
    Visible
};

#define LIST_UIELEMENT_LIST(X) \
    X(List) \
        X(Placeholder) \
        X(Add) \
        X(Delete) \
        X(Rename) \
        X(Refresh)

zEnum(UiElement, LIST_UIELEMENT_LIST);
zEnum_helpers(UiElement, LIST_UIELEMENT_LIST);

// ─────────────────────────────────────────────────────────────
//  UI STATE MAP (DEKLARATÍV MÁTRIX)
// ─────────────────────────────────────────────────────────────

inline static const QMap<State, UiState<UiElement>> UI_STATE_MAP =
{

    {
     State::Hidden, {
                        { UiElement::List, Visibility::Hidden},
                        { UiElement::Placeholder, Visibility::Visible },
                        { UiElement::Add, Enabledness::Enabled },
                        { UiElement::Delete, Enabledness::Disabled },
                        { UiElement::Rename, Enabledness::Disabled },
                        { UiElement::Refresh, Enabledness::Enabled }
                    }
    },

{
     State::Visible,
     {
      { UiElement::List ,Visibility::Visible},
      { UiElement::Placeholder, Visibility::Hidden },
      { UiElement::Add, Enabledness::Enabled },
      { UiElement::Delete, Enabledness::Enabled },
      { UiElement::Rename, Enabledness::Enabled },
      { UiElement::Refresh, Enabledness::Enabled }
    }
}
};


inline static QMap<UiElement, QObject*> widgets;

public:
void init(const OrderWorkbenchUIModel& ui)
{
    if(!widgets.isEmpty()) return;

    widgets.insert(UiElement::List, ui.listPanel);
    widgets.insert(UiElement::Placeholder, ui.listPlaceholder);
    widgets.insert(UiElement::Add, ui.listActions[ListAction::Add]);
    widgets.insert(UiElement::Delete, ui.listActions[ListAction::Delete]);
    widgets.insert(UiElement::Rename, ui.listActions[ListAction::Rename]);
    widgets.insert(UiElement::Refresh, ui.listActions[ListAction::Refresh]);

    UiStateApplier::validateWidgets<UiElement, UiElement, UiElementHelpers>(widgets);
}
// ─────────────────────────────────────────────────────────────
//  RESOLVE (logikai állapotgép) – változatlan
// ─────────────────────────────────────────────────────────────

struct ResolveModel{
    bool registryIsEmpty;
};

State resolve(ResolveModel m)
{
    if(widgets.isEmpty()){
        zWarning(L("%1::resolve called before init!").arg(className()));
    }

    if (m.registryIsEmpty)
        return State::Hidden;

    return State::Visible;
}

// ─────────────────────────────────────────────────────────────
//  APPLY (DEKLARATÍV) – nincs switch, nincs logika
// ─────────────────────────────────────────────────────────────
void apply(State s)
{
    if(widgets.isEmpty()){
        zWarning(L("%1::apply called before init!").arg(className()));
    }

    const UiState stateMap = UI_STATE_MAP.value(s);

    UiStateApplier::apply(stateMap, widgets);
}

};



