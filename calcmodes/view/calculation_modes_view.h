#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QUuid>
#include <QWidget>

#include "calculation/service/matrix_validator.h"

/**
 * @class CalculationModesView
 * @brief ProductDefinition → ModeName lista (NeedCalculation).
 *
 * *
 * - bal alsó panel a BOMWorkbench-ben
 * - oszlopok: ModeName, DetailsCount
 */
class CalculationModesView : public QWidget {
    Q_OBJECT
public:
    explicit CalculationModesView(QWidget* parent = nullptr);

    struct ModeRow {
        QUuid id;
        QUuid productId;
        QString modeName;
        int detailsCount = 0;
        bool hasMissingDetails = false;
        /* removed: missingDetails */
    };

 enum class ModeListState {
         NoData, // nincs semmilyen adat
         NoFilteredData, // van adat, de a szűrő miatt egy sem látszik
         Ok // van releváns adat
     };

    void set_modes(const QVector<ModeRow>& rows);
    std::optional<QUuid> currentModeId() const;

    bool isReady() const { return _ready; }
    void markReady() { _ready = true; }

    int rowCount() const;

    void selectMode(const QUuid& id);


private:
    bool _ready = false;

signals:
    void selection_changed(std::optional<QUuid> modeId);
    void request_add_mode(const QUuid& productId, const QString& modeName);
    void request_remove_mode(const QUuid& modeId);
    void request_rename_mode(const QUuid& modeId);
    void current_product_changed(const QUuid& productId,
                                 const QString& productName,
                                 const QString& productBarcode);
    void modeActivated(const QUuid& modeId);

public slots:
    void set_current_product(const QUuid& productId, const QString& productName, const QString& productBarcode);
    void on_item_double_clicked(QTableWidgetItem* item);


private:
    QTableWidget* _table = nullptr;
    QUuid _current_productId;

    void setup_table();
    void on_selection_changed();
};
