// products/repository/product_repository.cpp
#include "products/repository/product_repository.h"
#include "common/csv/filecontext.h"
#include "common/csv/rowerror.h"
#include "common/logger/logger.h"

/**
 * CSV formátum:
 * id,parentId,name,barcode
 * {UUID},,{RootName},CODE
 * {UUID},{ParentUUID},{ChildName},CODE
 */
QVector<ProductDefinition> ProductRepository::loadFromCSV(const QString& filePath) {
    QVector<ProductDefinition> out;
    CsvImporter::FileContext ctx("Product import", filePath);

    if (!ctx.openForRead()) {
        zError(QString("❌ ProductRepository: nem sikerült megnyitni a CSV-t: %1").arg(filePath));
        return out;
    }

    int rowIx = 0;
    while (ctx.readNextRow()) {
        ++rowIx;
        const auto row = ctx.currentRow();

        // Kötelező mezők ellenőrzése
        const QString idStr = row.value("id");
        const QString nameStr = row.value("name");

        if (idStr.isEmpty() || nameStr.isEmpty()) {
            ctx.addRowError(RowError(rowIx, "Hiányzó kötelező mező: id vagy name"));
            zWarning(QString("⚠️ ProductRepository: sor %1 - hiányzó id vagy name").arg(rowIx));
            continue;
        }

        ProductDefinition def;
        def.id = QUuid(idStr);
        const QString parentStr = row.value("parentId");
        def.parentId = parentStr.isEmpty() ? QUuid() : QUuid(parentStr);
        def.name = nameStr;
        def.barcode = row.value("barcode");

        out.append(def);
    }

    zInfo(QString("📊 ProductRepository: %1 terméktípus beolvasva (%2)")
              .arg(out.size())
              .arg(filePath));
    return out;
}
