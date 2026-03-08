#pragma once

#include <QString>

template<typename T>
struct Result {
    bool ok;
    T value;
    QString error;

    static Result<T> success(const T& v) {
        return { true, v, "" };
    }

    static Result<T> failure(const QString& err) {
        return { false, T{}, err };
    }

    static Result<T> failureWithValue(const T& value, const QString& error) {
        Result<T> r;
        r.ok = false;
        r.error = error;
        r.value = value;
        return r;
    }

};
