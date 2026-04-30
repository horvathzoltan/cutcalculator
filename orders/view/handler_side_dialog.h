#pragma once
#include <QDialog>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class HandlerSideDialog : public QDialog {
    Q_OBJECT
public:
    HandlerSideDialog(int qty, int jInit, int bInit, QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Handler side");
        setModal(true);

        auto* layout = new QVBoxLayout(this);

        auto* row = new QHBoxLayout();
        _j = new QSpinBox(this);
        _b = new QSpinBox(this);

        _j->setRange(0, qty);
        _b->setRange(0, qty);

        _j->setValue(jInit);
        _b->setValue(bInit);

        row->addWidget(new QLabel("J:", this));
        row->addWidget(_j);
        row->addWidget(new QLabel("B:", this));
        row->addWidget(_b);

        layout->addLayout(row);

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        layout->addWidget(buttons);

        connect(buttons, &QDialogButtonBox::accepted, this, [this, qty]() {
            if (_j->value() + _b->value() == qty)
                accept();
        });
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

    int j() const { return _j->value(); }
    int b() const { return _b->value(); }

private:
    QSpinBox* _j = nullptr;
    QSpinBox* _b = nullptr;
};
