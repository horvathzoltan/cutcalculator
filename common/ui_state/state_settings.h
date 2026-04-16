#include <QVariantMap>
#include <QString>

class StateSettings
{
public:
    explicit StateSettings(const QString& groupName);

    QVariantMap& map();
    const QVariantMap& map() const;

    QString prefix() const;
    void save() const;

private:
    QString _groupName;
    QVariantMap _map;
};
