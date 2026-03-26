#pragma once

#include <QVector>


template<typename Host>
struct TestSupportMixin {
public:
    // registry teljes törlése teszthez
    void clearForTest() {
        Host* h = static_cast<Host*>(this);

        // 1) Memória ürítése (engine-level clear)
        h->clear();

        // 2) CSV kiürítése (domain-level persist)
        //    Csak akkor, ha a Host rendelkezik persist() metódussal
        if constexpr (requires(Host* x) { x->persist(); }) {
            h->persist();   // üres readAll() → üres CSV
        }
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
