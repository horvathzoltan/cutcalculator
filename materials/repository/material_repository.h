#pragma once

#include <QString>
#include <QVector>
#include "common/csv/csvimporter.h"
#include "materials/model/material_master.h"
#include "materials/registry/material_registry.h"

/**
 * MaterialRepository
 *
 * Háromfázisú CSV import minta:
 *  - Stage 1: Convert → CSV sor → MaterialRow
 *  - Stage 2.5: Validate → MaterialRow → hibák
 *  - Stage 2: Build → MaterialRow → MaterialMaster
 *  - Stage 3: Assemble → MaterialMaster → MaterialRegistry
 */
class MaterialRepository {
public:
    static bool loadFromCSV(MaterialRegistry& registry);
private:
    // 📥 CSV betöltés → visszaadja az anyagok listáját
   // static QVector<MaterialMaster> loadFromCSV_private(CsvImporter::FileContext& filePath);

    struct MaterialRow {
        QString name;
        QString barcode;
        double stockLength;
        QString dim1;
        QString dim2;
        QString shapeStr;
        QString machineId;
        QString typeStr;
        QString colorStr; // 🎨 Opcionális színmező (RAL, HEX vagy üres)
        QString cuttingMode;
        QString paintingMode;
    };

    // --- Stage 1: Convert ---
    static std::optional<MaterialRow> convertRowToMaterialRow(const QVector<QString>& parts,
                                                              CsvImporter::FileContext& ctx);

    // Stage 2.5: Validate
    static QVector<CsvImporter::RowError> validateMaterialRow(const MaterialRow& row,
                                                              int lineNumber);

    // --- Stage 2: Build ---
    static std::optional<MaterialMaster> buildMaterialFromRow(const MaterialRow& row,
                                                              CsvImporter::FileContext& ctx);

    // --- Stage 3: Load & Assemble ---
    static QVector<MaterialRow> loadMaterialRows(CsvImporter::FileContext& ctx);
};
