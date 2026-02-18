#pragma once
#include <QMap>
#include <QString>
#include <QVariant>

#pragma once
#include <QMap>
#include <QString>
#include <QVariant>

class VariableRepository {
public:
    static VariableRepository& instance() {
        static VariableRepository inst;
        return inst;
    }

    void set(const QString& key, const QVariant& v) {
        _values[key] = v;
    }

    QVariant get(const QString& key) const {
        return _values.value(key, {});
    }

    void clear() {
        _values.clear();
    }

private:
    QMap<QString, QVariant> _values;
};


