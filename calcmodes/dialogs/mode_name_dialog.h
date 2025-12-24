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

/**
 * @brief ModeNameDialog – számítási mód nevének bekérése.
 *
 * UX célok:
 *  - Egér alá pozicionálás (attention focus)
 *  - Accent keret (vizuális kiemelés)
 *  - Inline validáció (üres / whitespace / duplikált név)
 *  - Shake animáció hibánál
 *  - SelectAll + fókusz
 *
 * A duplikáció ellenőrzéséhez egy callback függvényt kap:
 *   std::function<bool(const QString&)> isDuplicate;
 */
class ModeNameDialog : public QDialog {
    Q_OBJECT

public:
    explicit ModeNameDialog(QWidget* parent,
                            const QString& initialValue,
                            std::function<bool(const QString&)> duplicateCheck);

    QString value() const;

private slots:
    void onTextChanged(const QString& text);

private:
    void applyAccentFrame();
    void positionNearCursor();
    void shake();

private:
    QLineEdit* _edit = nullptr;
    QLabel* _label = nullptr;
    QDialogButtonBox* _buttons = nullptr;
    QPushButton* _ok = nullptr;

    std::function<bool(const QString&)> _duplicateCheck;
};
