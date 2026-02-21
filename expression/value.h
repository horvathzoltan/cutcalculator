#pragma once
#include <QString>

struct Value {
    enum class Type { Number, String, Bool, Null };

    Type type = Type::Null;

    union {
        double number;
        bool boolean;
    };

    QString text;

    static Value numberValue(double n) {
        Value v; v.type = Type::Number; v.number = n; return v;
    }

    static Value stringValue(const QString& s) {
        Value v; v.type = Type::String; v.text = s; return v;
    }

    static Value boolValue(bool b) {
        Value v; v.type = Type::Bool; v.boolean = b; return v;
    }

    static Value nullValue() {
        return {};
    }

    bool isTruthy() const {
        if (type == Type::Bool)   return boolean;
        if (type == Type::Number) return number != 0.0;
        if (type == Type::String) return !text.isEmpty();
        return false;
    }

    double toDouble(bool* ok = nullptr) const {
        switch (type) {
        case Type::Number:
            if (ok) *ok = true;
            return number;

        case Type::Bool:
            if (ok) *ok = true;
            return boolean ? 1.0 : 0.0;

        case Type::String: {
            bool localOk = false;
            double d = text.toDouble(&localOk);
            if (ok) *ok = localOk;
            return d;
        }

        case Type::Null:
        default:
            if (ok) *ok = false;
            return qQNaN();
        }
    }

    QString toString() const {
        switch (type) {
        case Type::Number: return QString::number(number);
        case Type::Bool:   return boolean ? "true" : "false";
        case Type::String: return text;
        case Type::Null:   return "";
        }
        return "";
    }

    bool toBool(bool* ok = nullptr) const {
        switch (type) {
        case Type::Bool:
            if (ok) *ok = true;
            return boolean;

        case Type::Number:
            if (ok) *ok = true;
            return number != 0.0;

        case Type::String:
            if (ok) *ok = true;
            return !text.isEmpty();

        case Type::Null:
        default:
            if (ok) *ok = false;
            return false;
        }
    }

};
