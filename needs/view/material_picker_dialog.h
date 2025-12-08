#pragma once
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QUuid>
#include <QTimer>

/**
 * @class MaterialPickerDialog
 * @brief Hunglish: egyszerű dialog, ahol a user kiválasztja a hozzárendelendő anyagot.
 */
class MaterialPickerDialog : public QDialog {
    Q_OBJECT
public:
    explicit MaterialPickerDialog(QWidget* parent = nullptr);

    struct PickResult {
        QUuid material_id;
        QString material_name;
        QString material_barcode;
    };

    PickResult result() const { return _result; }

private:
    QListWidget* _list = nullptr;
    QPushButton* _okBtn = nullptr;
    QPushButton* _cancelBtn = nullptr;

    PickResult _result;

    QLineEdit* _searchBox = nullptr;
    QTimer* _debounceTimer = nullptr;

    void populateMaterials(); // repo-ból tölti be a MaterialMaster listát

private slots:
    void onAccept();
};
