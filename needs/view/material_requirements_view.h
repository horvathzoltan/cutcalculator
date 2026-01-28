#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QString>
#include <QUuid>
#include "common/registry/subscription_token.h"
#include "ui/widgets/overlay_icon_widget.h"


/**
 * @class MaterialRequirementsView
 * @brief Anyagszükséglet editor – Product ↔ Material kapcsolatok listája.
 *
 * Hunglish magyarázat:
 * - Ez a view kezeli a terméktípushoz tartozó anyagok listáját (NeedRule jelleg).
 * - Nem foglalkozik formulával vagy számítási móddal – az külön view-ben él majd.
 * - A data flow auditbarát: barcode + opcionális UUID minden sorban, hogy a repo sync egyértelmű legyen.
 *
 * UX koncepció:
 * - Bal oldali ProductTree-ből kiválasztott termékre szűrhető (set_current_product).
 * - Kijelölt sorokra törlés (remove_selected).
 * - Add/Remove műveletekhez jelez (signals), a controller/manager intézi a tényleges mentést.
 *
 * Elhelyezési javaslat a projektben:
 * - needs/view/material_requirements_view.h/.cpp
 * - Később: needs/manager/material_requirements_manager.h/.cpp (repo integráció).
 */
class MaterialRequirementsView : public QWidget {
    Q_OBJECT
public:
    explicit MaterialRequirementsView(QWidget* parent = nullptr);

    /**
     * @struct RequirementRow
     * @brief Egy kapcsolat (Product ↔ Material) teljes adata a táblához.
     *
     * *
     * - Mindkét oldal azonosítói (UUID + barcode) a későbbi repo integrációhoz.
     * - A name-ek a felhasználónak, a barcode/UUID az auditnak.
     */
    struct RequirementRow {
        QUuid product_id;
        QString product_name;
        QString product_barcode;

        QUuid material_id;
        QString material_name;
        QString material_barcode;

        bool material_exists = true; // ha false → piros háttér, tooltip warning
        bool matrixComplete = true;
    };

    /**
     * @brief A teljes táblát újra feltölti a kapott sorokkal.
     * @param rows A megjelenítendő kapcsolatok (Product ↔ Material)
     */
    void set_requirements(const QVector<RequirementRow>& rows);

    /**
     * @brief Új kapcsolat (sor) hozzáadása – convenience a gyors UI műveletekhez.
     * @note Ez csak a view-ra ír, mentést a controller/manager intézi.
     */
    void add_requirement(const RequirementRow& row);

    /**
     * @brief Kijelölt sor(ok) törlése a táblából.
     * @note A tényleges repository törlést a controller/manager intézi a jelzés alapján.
     */
    void remove_selected();

    /**
     * @brief Aktuális product beállítása – a view megjegyzi és jelez toolbar felé.
     * @note Kötelezően adj meg legalább barcode-ot (auditbarát azonosító).
     */
    void set_current_product(const QUuid& product_id,
                             const QString& product_name,
                             const QString& product_barcode);

    //void setStatusWidget(OverlayIconWidget* w) { _statusWidget = w; }
    // MVP: a view csak megjelenít – a számolás managerben történik
    //void updateOverlay(int repoCount, int visibleRows);

    int rowCount() const;
signals:
    /**
     * @brief Jelzés: új kapcsolat hozzáadása kérve (toolbarból, plusz gombból).
     * * a view kéri, a manager létrehozza és visszatölti.
     */
    void request_add_requirement(const QUuid& productId, const QUuid& materialId);

    /**
     * @brief Jelzés: kapcsolat törlése kérve a kiválasztott sor alapján.
     * @note A signal a lényegi audit azonosítókat adja vissza.
     */
    void request_remove_requirement(const QUuid& product_id,
                                    const QString& product_barcode,
                                    const QUuid& material_id,
                                    const QString& material_barcode);

    /**
     * @brief Jelzés: sor kijelölés változott – toolbar enable/disable.
     */
    void selection_changed(bool has_selection);

    void request_change_material(const QUuid& productId,
                                 const QUuid& oldMaterialId);



private:
    QTableWidget* _table = nullptr;
    //OverlayIconWidget* _statusWidget = nullptr;

    // Az aktuálisan kiválasztott termék (bal oldali fa alapján)
    QUuid _current_product_id;
    QString _current_product_name;
    QString _current_product_barcode;

    void setup_table();
    void apply_row_visuals(int row, const RequirementRow& data);
    static QString format_product_cell(const RequirementRow& data);
    static QString format_material_cell(const RequirementRow& data);

//    SubscriptionToken _registryToken;

    //void refreshRows();
private slots:
    void on_selection_changed();
};
