#pragma once
#include <QDialog>
#include <QUuid>

class QListWidget;

class MaterialPickerDialog : public QDialog {
    Q_OBJECT
public:
    explicit MaterialPickerDialog(QWidget* parent = nullptr);

    QUuid selectedMaterial() const;

private:
    QListWidget* _list = nullptr;
    QUuid _selected;
};
