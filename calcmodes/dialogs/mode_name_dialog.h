#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>

/**
 * @class ModeNameDialog
 * @brief Számítási mód név bekérő dialógus (ModeName).
 *
 * Feladat:
 *  - Új NeedCalculation felvitelekor bekéri a modeName értéket.
 *  - Üres név esetén a dialógus nem engedélyezi az OK gombot.
 *
 * Integráció:
 *  - CalculationModesManager-ben használjuk a request_add_mode jelre reagálva.
 */
class ModeNameDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ModeNameDialog(QWidget* parent = nullptr);

    /// Visszaadja a felhasználó által megadott számítási mód nevét (trimmed).
    QString modeName() const;

private:
    QLineEdit* _edit = nullptr;
    QLabel* _label = nullptr;
    QDialogButtonBox* _buttons = nullptr;

    void setupUi();
    void connectSignals();
};
