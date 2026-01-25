#pragma once

#include <QString>
#include <QStringList>

class TestManager {
public:
    static TestManager& instance();

    // Futtatja az üzleti logika teszteket a megadott profil alapján
    void runBusinessLogicTests(const QString& profile);

    // Opcionális: visszaadja az utolsó futtatás eredményeit
    QStringList lastResults() const;

private:
    TestManager() = default;
    QStringList _lastResults;

    template<typename T>
    void runModule() {
        T tester;
        bool ok = tester.run();
        _lastResults << tester.fullResultString(ok);
    }

};
