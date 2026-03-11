#pragma once
#include <QString>

struct Value {
    enum class Type { Number, String, Bool, Null, Skip, Error };
private:
    Type _type = Type::Null;

    union {
        double _number;
        bool _boolean;
    };

    QString _text;

public:
    static Value numberValue(double n) {
        Value v; v._type = Type::Number; v._number = n; return v;
    }

    static Value stringValue(const QString& s) {
        Value v; v._type = Type::String; v._text = s; return v;
    }

    static Value boolValue(bool b) {
        Value v; v._type = Type::Bool; v._boolean = b; return v;
    }

    static Value nullValue() {
        return {};
    }

    static Value skipValue() {
        Value v; v._type = Type::Skip; return v;
    }

    double number() const {
        switch (_type) {
        case Type::Number: return _number;
        case Type::Bool:   return _boolean ? 1.0 : 0.0;
        case Type::Null:   return 0.0;
        default: return qQNaN();
        }
    }

    QString string() const {
        switch (_type) {
        case Type::String: return _text;
        case Type::Number: return QString::number(_number);
        case Type::Bool: return _boolean ? "true" : "false";
        default: return "";
        }
    }

    QString toString() const {
        switch (_type) {
        case Type::String: return _text;
        case Type::Number: return QString::number(_number);
        case Type::Bool: return _boolean ? "true" : "false";
        case Type::Null: return "NULL";
        case Type::Skip: return "(SKIP)";
        case Type::Error: return "(error: " + _text + ")";
        default: return "(unknown)";
        }
    }

    bool boolean() const {
        switch (_type) {
        case Type::String: return stringToBool(_text);
        case Type::Number: return _number != 0.0;
        case Type::Bool: return _boolean;
        default: return false;
        }
    }


    QString typeName() const
    {
        switch (_type) {
        case Type::Number: return "Number";
        case Type::String: return "String";
        case Type::Bool:   return "Bool";
        case Type::Null:   return "Null";
        case Type::Skip:   return "Skip";
        case Type::Error:  return "Error";
        }
        return "Unknown";
    }

    Type type() const { return _type; }

    static Value errorValue(const QString& msg) {
        Value v;
        v._type = Type::Error;
        v._text = msg;
        return v;
    }

    bool isError() const { return _type == Type::Error; }
    bool isSkip()  const { return _type == Type::Skip; }
    bool isNull() const { return _type == Type::Null; }

    QString errorMessage() const {
        return _type == Type::Error ? _text : QString();
    }

private:
    static bool stringToBool(const QString& txt){
        QString s = txt.trimmed().toLower();

        if(txt.isEmpty()) return false;
        // explicit logikai értékek
        if (s == "true" || s == "yes" || s == "ok")
            return true;
        if (s == "false" || s == "no")
            return false;

        // számként értelmezhető?
        bool ok = false;
        double d = s.toDouble(&ok);
        if (ok)
            return d != 0.0;

        // minden más → false
        return false;
    }
};
