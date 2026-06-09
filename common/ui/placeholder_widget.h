#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class PlaceholderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaceholderWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        auto* layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(20, 20, 20, 20);

        _label = new QLabel(this);
        _label->setAlignment(Qt::AlignCenter);
        _label->setStyleSheet("font-size: 16px; color: #666;");
        _label->setWordWrap(true);

        layout->addWidget(_label);
    }

public slots:
    void setText(const QString& text)
    {
        _label->setText(text);
    }

private:
    QLabel* _label = nullptr;
};
