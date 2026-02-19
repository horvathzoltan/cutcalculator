#pragma once
#include <QString>

struct EvalResult {
    bool ok = false;
    QString error;

    static EvalResult success() {
        return { true, {} };
    }

    static EvalResult failure(const QString& msg) {
        return { false, msg };
    }
};
