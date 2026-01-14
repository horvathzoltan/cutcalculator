#pragma once

#include <QStringList>

// 🌱 Indulási állapotot leíró struktúra

struct StartupStatus {
private:
    bool _ok = false;
    bool _critical = true;
    QString _errorMessage;
    QStringList _warnings;

public:
    static StartupStatus success() {
        StartupStatus s;
        s._ok = true;
        s._critical = false;
        return s;
    }

    static StartupStatus failure(const QString& msg, bool critical = true) {
        StartupStatus s;
        s._ok = false;
        s._errorMessage = msg;
        s._critical = critical;
        return s;
    }

    void addWarning(const QString& warning) {
        _warnings.append(warning);
    }

    void addWarnings(const QStringList& warnings) {
        _warnings.append(warnings);
    }

    bool isSuccess() const {return _ok; }
    bool isCritical() const { return _critical; }

    QStringList warnings() const { return _warnings; }
    QString errorMessage() const { return _errorMessage; }
};
