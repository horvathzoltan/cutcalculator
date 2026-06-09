#pragma once

// ─────────────────────────────────────────────────────────────
//  UI STATE STRUCT
// ─────────────────────────────────────────────────────────────
#include "common/ui_state/element_state.h"

#include <QMap>
template<typename Key>
struct UiState
{
    struct InitializerModel{
        Key e;
        ElementState s;
    };

    QMap<Key, ElementState> elements;

    UiState() = default;

    UiState(std::initializer_list<InitializerModel> init)
    {
        for (auto& a : init)
            elements.insert(a.e, a.s);
    }
};