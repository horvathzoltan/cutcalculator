#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <functional>
#include <QUuid>

#include "common/utils/attention_blinker.h"
#include "common/utils/cursor_aware_placement.h"

class ProductNameDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProductNameDialog(QWidget* parent,
                               const QString& initialName,
                               const QString& initialBarcode,
                               const QUuid& parentId,
                               std::function<bool(const QString&, QString&)> nameCheck,
                               std::function<bool(const QString&, QString&)> barcodeCheck);

    QString value() const;
    QString barcodeValue() const;

private slots:
    void onNameChanged(const QString& text);
    void onBarcodeChanged(const QString& text);
    void updateOkState();

private:
    QLineEdit* _edit = nullptr;
    QLineEdit* _barcodeEdit = nullptr;
    QLabel* _label = nullptr;
    QLabel* _globalError = nullptr;
    QDialogButtonBox* _buttons = nullptr;
    QPushButton* _ok = nullptr;

    QUuid _parentId;

    std::function<bool(const QString&, QString&)> _nameCheck;
    std::function<bool(const QString&, QString&)> _barcodeCheck;

    AttentionBlinker* _blinker = nullptr;
    bool _attentionRemoved = false;

    void removeAttentionStyling();
    void onUserFoundDialog();

protected:
    bool event(QEvent* e) override;
    void mousePressEvent(QMouseEvent* event) override;
};
