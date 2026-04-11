#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QUuid>
#include "materials/model/cutting_mode.h"
#include "calculation/model/need_calculation_detail.h"
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
        bool formulaValid = true;
        bool materialValid = true;
        CuttingMode matMode = CuttingMode::Unknown;
        NeedCalculationDetail::DetailKind kind = NeedCalculationDetail::DetailKind::Cutting;
    };


    void set_details(const QVector<DetailRow>& rows);

    QUuid currentCalculationId() const { return _current_calcId; }
    void reset();

    int rowCount() const;

signals:
    void request_open_formula_editor(const QUuid& detailId);
    void request_update_formula(const QUuid& detailId, const QString& newFormula);

public slots:
    void set_current_calculation(const QUuid& calcId, const QString& modeName);
    void scrollToRow(int row);
    QTableWidget* table() const { return _table; }

private:
    QTableWidget* _table = nullptr;
    QUuid _current_calcId;


    // ÚJ: undo/redo stack formula cellákhoz
    QVector<QString> _undoStack;
    QVector<QString> _redoStack;
    QString _lastFormulaValue;

    void setup_table();
    void updateRowVisuals(int row, bool materialValid, bool formulaValid, bool empty);
    bool eventFilter(QObject* obj, QEvent* ev) override;
    void renderRow(int row, const DetailRow& r);
};
