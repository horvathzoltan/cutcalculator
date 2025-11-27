#pragma once

#include <QString>
#include <QVector>
#include "common/csv/csvimporter.h"
#include "materials/model/material_master.h"
#include "materials/registry/material_registry.h"

/**
 * MaterialRepository
 *
 * Three Phase Import minta alapján tölti be az anyagokat CSV-ből.
 * Fázisok:
 *  - Convert: CSV sor → MaterialRow
 *  - Build: MaterialRow → MaterialMaster
 *  - Assemble: MaterialMaster → MaterialRegistry
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

    // static std::optional<MaterialMaster> convertRowToMaterial(const QVector<QString>& parts, CsvImporter::FileContext& ctx);
    // static std::optional<MaterialRow>convertRowToMaterialRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx);
    // static std::optional<MaterialMaster> buildMaterialFromRow(const MaterialRow &row, CsvImporter::FileContext& ctx);

    // --- Stage 1: Convert ---
    static std::optional<MaterialRow> convertRowToMaterialRow(const QVector<QString>& parts,
                                                              CsvImporter::FileContext& ctx);

    // --- Stage 2: Build ---
    static std::optional<MaterialMaster> buildMaterialFromRow(const MaterialRow& row,
                                                              CsvImporter::FileContext& ctx);

    // --- Stage 3: Load & Assemble ---
    static QVector<MaterialRow> loadMaterialRows(CsvImporter::FileContext& ctx);
};
