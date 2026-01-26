#pragma once
#include <QDialog>
#include <QString>

class QLineEdit;

class FormulaEditorDialog : public QDialog {
    Q_OBJECT
public:
    explicit FormulaEditorDialog(const QString& initial, QWidget* parent = nullptr);

    QString formula() const;

private:
    QLineEdit* _edit = nullptr;
};
