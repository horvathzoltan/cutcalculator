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
};
