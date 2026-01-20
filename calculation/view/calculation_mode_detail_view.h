#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QUuid>

/**
 * @class CalculationModeDetailView
 * @brief NeedCalculationDetail lista: Material + Formula + TypeIcon (⚙️/📦)
 *
 * *
 * - jobb oldali panel
 * - piros háttér hibás/missing formula esetén
 */
class CalculationModeDetailView : public QWidget {
    Q_OBJECT
public:
    explicit CalculationModeDetailView(QWidget* parent = nullptr);

    struct DetailRow {
        QUuid id;
        QUuid needCalculationId;
        QUuid materialId;
        QString materialName;
        QString materialBarcode;
        QString formula;
        bool isCutting = true; // ⚙️ vs 📦
        bool formulaValid = true;
        bool materialValid = true;
    };

    void set_details(const QVector<DetailRow>& rows);

    QUuid currentCalculationId() const { return _current_calcId; }
    void reset();

    int rowCount() const;

signals:
    void request_add_detail(const QUuid& calcId);
    void request_remove_detail(const QUuid& detailId);
    void request_edit_formula(const QUuid& detailId);

public slots:
    void set_current_calculation(const QUuid& calcId, const QString& modeName);

private:
    QTableWidget* _table = nullptr;
    QUuid _current_calcId;

    void setup_table();
    void apply_row_visuals(int row, const DetailRow& r);
};
