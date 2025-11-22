#pragma once

#include <QStringList>

// 🌱 Indulási állapotot leíró struktúra

struct StartupStatus {
private:
    bool _ok = false;
    QString _errorMessage;
    QStringList _warnings;

public:
    static StartupStatus success() {
        StartupStatus s;
        s._ok = true;
        return s;
    }

    static StartupStatus failure(const QString& msg) {
        StartupStatus s;
        s._ok = false;
        s._errorMessage = msg;
        return s;
    }

    void addWarning(const QString& warning) {
        _warnings.append(warning);
    }

    void addWarnings(const QStringList& warnings) {
        _warnings.append(warnings);
    }

    bool isSuccess() const {return _ok; }

    QStringList warnings() const { return _warnings; }
    QString errorMessage() const { return _errorMessage; }
};
