#pragma once
#include <QString>
#include <QUuid>
#include <QVector>

/**
 * @class NeedCalculator
 * @brief Megrendelésből (OrderLine) vágási és kitting listát készít a NeedRule + NeedCalculationDetail alapján.
 *
 * *
 * - Input: productId, w, h, qty
 * - a modeName kiválasztása UI-ból vagy szabályból; itt paraméter
 */
struct OrderLine {
    QUuid productId;
    int w_mm = 0;
    int h_mm = 0;
    int qty = 1;
    QString color; // optional
};

struct CutItem {
    QUuid materialId;
    int length_mm;
    int pieces;
};

struct KitItem {
    QUuid materialId;
    int pieces;
};

class NeedCalculator {
public:
    static QVector<CutItem> makeCutList(const OrderLine& ol, const QString& modeName);
    static QVector<KitItem> makeKitList(const OrderLine& ol, const QString& modeName);

private:
    static int evalFormula(const QString& f, int w, int h);
};
