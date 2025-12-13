#include "materials/repository/material_repository.h"
#include <QFile>
#include <QTextStream>
#include <QUuid>

#include "materials/repository/material_repository.h"
#include "materials/model/material_master.h"
#include "materials/model/material_type.h"
#include "materials/model/crosssectionshape.h"
#include "materials/registry/material_registry.h"
#include <QFile>
#include <QTextStream>
#include <QUuid>
#include "common/utils/filename_helper.h"
#include "common/csv/csvimporter.h"
#include "colors/model/named_color.h"
#include "materials/model/cutting_mode.h"
#include "materials/model/painting_mode.h"
#include "common/registry/barcode_table.h"
#include "common/registry/registry_manager.h"
#include "common/registry/barcode_collision_helper.h"

// --- Stage 1: Convert ---
std::optional<CsvImporter::AuditedRow<MaterialRepository::MaterialRow>>
MaterialRepository::convertRowToMaterialRow(const QVector<QString>& parts,
                                            CsvImporter::FileContext& ctx) {
    if (parts.size() < 11) {
        ctx.addError(ctx.currentLineNumber(), "⚠️ Kevés mező (11 szükséges)");
        return std::nullopt;
    } else if (parts.size() > 11) {
        ctx.addError(ctx.currentLineNumber(), "⚠️ Túl sok mező (11 szükséges)");
        return std::nullopt;
    }

    MaterialRow row;
    row.name        = parts[0].trimmed();
    row.barcode     = parts[1].trimmed();
    row.stockLength = parts[2].trimmed().toDouble();
    row.dim1        = parts[3].trimmed();
    row.dim2        = parts[4].trimmed();
    row.shapeStr    = parts[5].trimmed();
    row.machineId   = parts[6].trimmed();
    row.typeStr     = parts[7].trimmed();
    row.colorStr    = parts[8].trimmed();
    row.cuttingMode = parts[9].trimmed();
    row.paintingMode= parts[10].trimmed();

    return CsvImporter::AuditedRow<MaterialRow>{
        ctx.currentLineNumber(),
        row
    };
}


CsvImporter::RowError MaterialRepository::makeError(int lineNumber,
                                const QString& message,
                                const MaterialRepository::MaterialRow& row) {
    return { lineNumber, message, row.barcode, row.name };
}

// QString MaterialRepository::toDisplay(const MaterialRow& row) {
//     return row.name.isEmpty() ? "[" + row.barcode + "]"
//                               : row.name + " [" + row.barcode + "]";
// }

// --- Stage 2.5: Validate ---
QVector<CsvImporter::RowError>
MaterialRepository::validateMaterialRow(const MaterialRow& row, int lineNumber) {
    QVector<CsvImporter::RowError> errors;

    // if(row.barcode == "ROL-P"){
    //     zInfo("brekk");
    // }
    if (row.barcode.isEmpty()){
        errors.append(makeError(lineNumber, "⚠️ Hiányzó barcode", row));
    } else {
        if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
                  "Material",
                  BarcodeCollisionHelper::RowInfo{ row.barcode, row.name },
                  lineNumber))
        {
            errors.append(*err);
        }
    }

    if (row.cuttingMode.compare("Length", Qt::CaseInsensitive) == 0) {
        if (row.stockLength <= 0)
            errors.append(makeError(lineNumber, "⚠️ Érvénytelen hossz", row));

        if (row.shapeStr.compare("Rectangular", Qt::CaseInsensitive) == 0) {
            if (row.dim1.isEmpty() || row.dim2.isEmpty())
                errors.append(makeError(lineNumber, "⚠️ Hiányzó szélesség/magasság", row));
        } else if (row.shapeStr.compare("Round", Qt::CaseInsensitive) == 0) {
            if (row.dim1.isEmpty())
                errors.append(makeError(lineNumber, "⚠️ Hiányzó átmérő", row));
        }
    }
    else if (row.cuttingMode.compare("Piece", Qt::CaseInsensitive) == 0) {
        // Darabos alkatrészeknél nem kötelező hossz/méret
        // stockLength, dim1, dim2 lehet üres vagy 0
    }

    if (!row.colorStr.isEmpty()) {
        NamedColor nc(row.colorStr);
        if (!nc.isValid())
            errors.append(
                makeError(lineNumber, QString("⚠️ Ismeretlen színformátum: %1").arg(row.colorStr), row));
    }

    return errors;
}

// --- Stage 2: Build ---
std::optional<MaterialMaster>
MaterialRepository::buildMaterialFromRow(const MaterialRow& row,
                                         CsvImporter::FileContext& ctx) {

    // Tartalmi validáció hibák gyűjtése
    auto rowErrors = validateMaterialRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);

    MaterialMaster m;
    m.id = QUuid::createUuid();
    m.name = row.name;
    m.barcode = row.barcode;
    m.stockLength_mm = row.stockLength;
    m.defaultMachineId = row.machineId;
    m.shape = CrossSectionShape::fromString(row.shapeStr);
    m.type = MaterialType::fromString(row.typeStr);
    m.cuttingMode = CuttingModeUtils::parse(row.cuttingMode);
    m.paintingMode = PaintingModeUtils::parse(row.paintingMode);

    // Runtime validáció: dimenziók
    if (m.cuttingMode == CuttingMode::Length) {
        if (m.shape == CrossSectionShape(CrossSectionShape::Shape::Rectangular)) {
            if (!row.dim1.isEmpty() && !row.dim2.isEmpty()) {
                bool okW = false, okH = false;
                double w = row.dim1.toDouble(&okW);
                double h = row.dim2.toDouble(&okH);
                if (!okW || !okH || w <= 0 || h <= 0) {
                    ctx.addError(ctx.currentLineNumber(), "⚠️ Érvénytelen szélesség/magasság");
                }
                m.size_mm = QSizeF(w, h);
            }
        } else if (m.shape == CrossSectionShape(CrossSectionShape::Shape::Round)) {
             if (!row.dim1.isEmpty()) {
                bool okD = false;
                double d = row.dim1.toDouble(&okD);
                if (!okD || d <= 0) {
                    ctx.addError(ctx.currentLineNumber(), "⚠️ Érvénytelen átmérő");
                }
                m.diameter_mm = d;
          }
        }
    }
    else if (m.cuttingMode == CuttingMode::Piece) {
        // Darabos alkatrészeknél nem kötelező hossz/méret
        m.stockLength_mm = 0;
        m.diameter_mm = 0;
        m.size_mm = QSizeF(0,0);
        // Nem dobunk hibát, ha dim1/dim2 üres
    }
    // Szín
    if (!row.colorStr.isEmpty()) {
        m.color = NamedColor(row.colorStr);
        if (!m.color.isValid()) {
            ctx.addError(ctx.currentLineNumber(), QString("⚠️ Ismeretlen színformátum: %1").arg(row.colorStr));
        }
    }

    return m;
}

// --- Stage 3: Load & Assemble ---
QVector<CsvImporter::AuditedRow<MaterialRepository::MaterialRow>>
MaterialRepository::loadMaterialRows(CsvImporter::FileContext& ctx) {
    return CsvImporter::readAndConvert<CsvImporter::AuditedRow<MaterialRepository::MaterialRow>>(ctx, convertRowToMaterialRow);
}

// --- Entry Point ---
bool MaterialRepository::loadFromCSV(MaterialRegistry& registry) {
    auto& helper = FileNameHelper::instance();
    if (!helper.isInitialized()) {
        zWarning("❌ A FileNameHelper nincs inicializálva.");
        return false;
    }

    const QString fn = helper.getMaterialCsvFile();
    CsvImporter::FileContext ctx("Material import", fn);

    const QVector<CsvImporter::AuditedRow<MaterialRepository::MaterialRow>> rows = loadMaterialRows(ctx);

    const QVector<MaterialMaster> defs =
        CsvImporter::buildAll<MaterialRow, MaterialMaster>(
            rows,
            buildMaterialFromRow,
            ctx
        );

    if (ctx.hasErrors()) {
        zWarning(QString("⚠️ Hibák az anyag import során (%1)").arg(ctx.errorsSize()));
    }

    for (const auto& m : defs) {
        registry.registerData(m); // ez már bejegyzi a BarcodeTable-be
    }

    zInfo(QString("📊 MaterialRepository: %1 anyag betöltve").arg(defs.size()));
    return !defs.isEmpty();
}

