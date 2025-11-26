// products/repository/product_repository.h
#pragma once
#include "products/model/product_definition.h"
#include <QVector>
#include <QString>

/**
 * ProductRepository
 *
 * Feladata: a globális terméktípus definíciók betöltése CSV-ből.
 * Elvárt oszlopok: id, parentId, name, barcode
 */
class ProductRepository {
public:
    /**
     * Betölti a products.csv fájlt és visszaadja a terméktípusokat.
     * A hibákat auditálhatóan logolja, de nem dob kivételt.
     */
    static QVector<ProductDefinition> loadFromCSV(const QString& filePath);
};
