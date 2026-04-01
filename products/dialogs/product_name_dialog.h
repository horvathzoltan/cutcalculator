// products/dialogs/product_name_dialog.h
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

#include "common/utils/attention_blinker.h"
#include "common/utils/cursor_aware_placement.h"

/**
 * @brief ProductNameDialog – terméktípus nevének bekérése.
 *
 * UX:
 *  - Egér alá pozicionálás
 *  - AttentionBlinker
 *  - Inline validáció (üres / duplikált név)
 */
class ProductNameDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProductNameDialog(QWidget* parent,
                               const QString& initialValue,
                               std::function<bool(const QString&)> duplicateCheck);

    QString value() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QLineEdit* _edit = nullptr;
    QLabel* _label = nullptr;
    QDialogButtonBox* _buttons = nullptr;
    QPushButton* _ok = nullptr;

    std::function<bool(const QString&)> _duplicateCheck;
    AttentionBlinker* _blinker = nullptr;
    bool _attentionRemoved = false;

    void removeAttentionStyling();
    void onUserFoundDialog();

protected:
    bool event(QEvent* e) override;
    void mousePressEvent(QMouseEvent* event) override;
};
