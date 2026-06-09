#pragma once
#include <QObject>
#include "element_state.h"
#include "uistate.h"


namespace UiStateApplier{

inline void applyElementState(QObject* widget, const ElementState& st)
{
    if (st.enabled.has_value()) {
        QMetaObject::invokeMethod(widget, "setEnabled",
                                  Q_ARG(bool, st.enabled == Enabledness::Enabled));
    }

    if (st.visible.has_value()) {
        QMetaObject::invokeMethod(widget, "setVisible",
                                  Q_ARG(bool, st.visible == Visibility::Visible));
    }

    if (st.text.has_value()) {
        QMetaObject::invokeMethod(widget, "setText",
                                  Q_ARG(QString, st.text.value()));
    }
}


template<typename Key, typename Key2>
inline void apply(const UiState<Key>& stateMap, const QMap<Key2, QObject*>& widgets){
    for (auto &key : stateMap.elements.keys()) {
        ElementState state = stateMap.elements.value(key);
        QObject* widget = widgets.value(key);
        applyElementState(widget, state);
    }
}

} // namespace UiStateApplier