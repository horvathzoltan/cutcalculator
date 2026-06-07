#pragma once

#include <QAction>
#include <QMap>

template<typename T>
class ActionMap {
public:
    QMap<T, QAction*> map;

    template<typename Obj, typename Method>
    void connect(T action, Obj* receiver, Method method) {
        if (map.contains(action) && map[action]) {
            QObject::connect(map[action], &QAction::triggered,
                             receiver, method);
        }
    }

    QAction* operator[](T action) const {
        return map.value(action, nullptr);
    }

    void insert(T action, QAction* act) {
        map.insert(action, act);
    }
};
