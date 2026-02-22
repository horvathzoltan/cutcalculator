#pragma once
#include "value.h"

#include <QMap>
#include <QString>
#include <QVariant>

class VariableRepository {
public:
    static VariableRepository& instance() {
        static VariableRepository inst;
        return inst;
    }

    void set(const QString& key, const Value& v) {
        _values[key] = v;
    }

    Value get(const QString& key) const {
        return _values.value(key, {});
    }

    void clear() {
        _values.clear();
    }

    QMap<QString, Value> dump() const { return _values; }


private:
    QMap<QString, Value> _values;
};


