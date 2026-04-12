#pragma once
#include "dsl/dsl_syntax_highlighter.h"
#include "dsl/dsl_text_edit.h"
#include <QDialog>
#include <QLabel>
#include <QPlainTextEdit>
#include <QString>

#include <calculation/model/need_calculation_detail.h>

class QLineEdit;

class FormulaEditorDialog : public QDialog {
    Q_OBJECT
public:
    explicit FormulaEditorDialog(const QString& initial, NeedCalculationDetail::DetailKind kind, QWidget* parent = nullptr);

    QString formula() const;

private:
    DslTextEdit* _edit = nullptr;
    DslSyntaxHighlighter* _hl = nullptr;

    NeedCalculationDetail::DetailKind _kind = NeedCalculationDetail::DetailKind::Cutting;
    QLabel* _status = nullptr;
};
