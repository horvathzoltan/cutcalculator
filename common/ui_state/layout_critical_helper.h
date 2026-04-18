#pragma once

#include <QWidget>

class LayoutCriticalHelper
{
public:
    static bool isLayoutCritical(QWidget* w);
    static QList<QWidget *> collect(QWidget *root);

};

