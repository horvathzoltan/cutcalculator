#pragma once
#include <QDialog>
#include <QUuid>

class QListWidget;

class MaterialPickerDialog_2 : public QDialog {
    Q_OBJECT
public:
    explicit MaterialPickerDialog_2(QWidget* parent = nullptr);

    QUuid selectedMaterial() const;

private:
    QListWidget* _list = nullptr;
    QUuid _selected;
};
