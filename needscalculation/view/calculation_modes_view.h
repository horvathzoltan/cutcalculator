#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QUuid>

/**
 * @class CalculationModesView
 * @brief ProductDefinition → ModeName lista (NeedCalculation).
 *
 * Hunglish:
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
    };

    void set_modes(const QVector<ModeRow>& rows);
    std::optional<QUuid> currentModeId() const;

signals:
    void selection_changed(std::optional<QUuid> modeId);
    void request_add_mode(const QUuid& productId);
    void request_remove_mode(const QUuid& modeId);
    void request_rename_mode(const QUuid& modeId);

public slots:
    void set_current_product(const QUuid& productId, const QString& productName, const QString& productBarcode);

private:
    QTableWidget* _table = nullptr;
    QUuid _current_productId;

    void setup_table();
    void on_selection_changed();
};
