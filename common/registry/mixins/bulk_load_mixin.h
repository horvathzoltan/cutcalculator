#pragma once

#include <QVector>


template<typename Host, typename Entity>
struct BulkLoadMixin {
public:
    // CSV/DB/snapshot → memória
    void bulkSetAll(const QVector<Entity>& v) {
        static_cast<Host*>(this)->setAll(v);
    }

    // opcionális: bulk append
    void bulkAddAll(const QVector<Entity>& v) {
        static_cast<Host*>(this)->addAll(v);
    }
};
