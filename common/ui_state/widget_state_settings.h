#include <QVariantMap>
#include <QString>

/*
 * WidgetStateSettings
 * --------------------
 * A WidgetStateManager által használt, groupName-alapú állapotbuffer.
 *
 * Feladata:
 *   - betölteni a widget state-et a WindowStateManager-ből
 *   - módosítható QVariantMap-et biztosítani a handler-ek számára
 *   - menteni a módosított állapotot
 *   - teljes UIState törlése (clearUIState)
 *
 * Ez a réteg NEM végez widgetfa-bejárást és NEM értelmezi a widgeteket.
 * Csak egy "adatbuffer" a WidgetStateManager számára.
 */
class WidgetStateSettings
{
public:
    explicit WidgetStateSettings(const QString& groupName);

    QVariantMap& map();
    const QVariantMap& map() const;

    QString prefix() const;
    void save() const;

    void clearUIState();

private:
    QString _groupName;
    QVariantMap _map;
};
