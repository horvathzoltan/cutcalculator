#pragma once

#include <QUuid>
#include <QString>

struct Piece
{
    QUuid materialId;
    QString materialBarcode;
    int requiredLength;
    QString handlerSide;

    QString externalRef;
    QString ownerName;
    QString colorName;

    int fullWidth;
    int fullHeight;
};
