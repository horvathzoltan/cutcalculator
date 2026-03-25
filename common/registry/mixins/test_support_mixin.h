#pragma once

#include <QVector>


template<typename Host>
struct TestSupportMixin {
public:
    // registry teljes törlése teszthez
    void clearForTest() {
        static_cast<Host*>(this)->clear();
    }

    template<typename Entity>
    bool insertForTest(const Entity& e) {
        return static_cast<Host*>(this)->insertInternal(e);
    }

    // egy elem hozzáadása teszthez (engine add)
    template<typename Entity>
    bool addForTest(const Entity& e) {
        return static_cast<Host*>(this)->add(e);
    }

    // teljes lista beállítása teszthez (engine setAll)
    template<typename Entity>
    void setAllForTest(const QVector<Entity>& v) {
        static_cast<Host*>(this)->setAll(v);
    }
};
