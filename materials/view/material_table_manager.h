#pragma once
#include <QObject>
#include <QString>

class MaterialMaster;
class MaterialTableWidget;

/**
 * MaterialTableManager
 *
 * A viewer táblázat adatait tölti és frissíti a MaterialRegistry-ből.
 * Felelős a sorok létrehozásáért és a cellák megjelenítéséért.
 *
 * Nem használ materialgroup színezést. A fő cella (Anyag) egy egyszerű,
 * szín-badge-et is tartalmazó widget, ha az anyagnak van definált NamedColor-ja.
 */
class MaterialTableManager : public QObject {
    Q_OBJECT
public:
    explicit MaterialTableManager(MaterialTableWidget* table, QObject* parent = nullptr);

    /**
     * A táblát teljesen újratölti a registry-ben lévő anyagokkal.
     * A táblát előtte törli.
     */
    void populateAll();

    /**
     * A táblázat tartalmát törli (sorok, cellák).
     */
    void clear();

    /**
     * Rövidítése a populateAll-nak (kényelmi hívás).
     */
    void refresh();

private:
    MaterialTableWidget* _table = nullptr;

    // ---- Sor és cella előállítás ----
    void addMaterialRow(const MaterialMaster& m, int rowIx);

    // ---- Segéd megjelenítők (group nélkül) ----
    QString shapeDisplay(const MaterialMaster& m) const;
    QString colorDisplay(const MaterialMaster& m) const;
    QString cuttingModeDisplay(const MaterialMaster& m) const;
    QString paintingModeDisplay(const MaterialMaster& m) const;

    // ---- Anyag cella widget (név + szín-badge) ----
    QWidget* createMaterialCellWidget(const MaterialMaster& m, const QString& tooltip) const;

    // ---- Tooltip anyaghoz (group nélkül) ----
    QString materialTooltip(const MaterialMaster& m) const;
};
