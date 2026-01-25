#pragma once
#include <QString>

class TestModuleBase {
public:
    explicit TestModuleBase(const QString& n)
        : _name(n)
    {}

    virtual ~TestModuleBase() = default;

    QString name() const { return _name; }

    // Kötelező override
    virtual bool run() = 0;

    QString fullResultString(bool ok) const { return _name + resultString(ok); }


private:
    const QString _name;

    TestModuleBase() = delete;

    QString resultString(bool ok) const { return ok ? " PASSED" : " FAILED"; }
};
