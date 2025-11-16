#pragma once

#include <QString>
#include <QStringList>

class TestManager {
public:
    static TestManager& instance();

    // Futtatja az üzleti logika teszteket a megadott profil alapján
    bool runBusinessLogicTests(const QString& profile);

    // Opcionális: visszaadja az utolsó futtatás eredményeit
    QStringList lastResults() const;

private:
    TestManager() = default;
    QStringList _lastResults;
};
