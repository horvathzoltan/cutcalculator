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
#include "common/color/namedcolor.h"
#include "materials/model/cutting_mode.h"
#include "materials/model/painting_mode.h"

bool MaterialRepository::loadFromCSV(MaterialRegistry& registry) {
    auto& helper = FileNameHelper::instance();
    if (!helper.isInitialized()) {
        zWarning("❌ A FileNameHelper nincs inicializálva.");
        return false;
    }

    const QString fn = helper.getMaterialCsvFile();
    CsvImporter::FileContext ctx(L("Material import"), fn);
    const QVector<MaterialMaster> loaded = loadFromCSV_private(ctx);

    if (loaded.isEmpty()) {
        ctx.setFileError("❌ Sikertelen beolvasás: a fájl nem tartalmazott feldolgozható adatot.");
        return false;
    }

    registry.setData(loaded); // 🔧 Anyagregisztráció
    return true;
}


QVector<MaterialMaster>
MaterialRepository::loadFromCSV_private(CsvImporter::FileContext& ctx) {
    return CsvImporter::readAndConvert<MaterialMaster>(ctx, convertRowToMaterial, true);
}

std::optional<MaterialMaster>
MaterialRepository::convertRowToMaterial(const QVector<QString>& parts, CsvImporter::FileContext& ctx) {
    const auto rowOpt = convertRowToMaterialRow(parts, ctx);
    if (!rowOpt.has_value()) return std::nullopt;

    return buildMaterialFromRow(rowOpt.value(), ctx);
}

std::optional<MaterialRepository::MaterialRow>
MaterialRepository::convertRowToMaterialRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx) {
    if (parts.size() < 11) {
        QString msg = L("⚠️ Kevés mező (legalább 9)");
        ctx.addError(ctx.currentLineNumber(), msg);
        return std::nullopt;
    }

    MaterialRow row;
    row.name       = parts[0].trimmed();
    row.barcode    = parts[1].trimmed();
    row.dim1       = parts[3].trimmed();
    row.dim2       = parts[4].trimmed();
    row.shapeStr   = parts[5].trimmed();
    row.machineId  = parts[6].trimmed();
    row.typeStr    = parts[7].trimmed();
    row.colorStr = parts[8].trimmed();
    row.cuttingMode = parts[9].trimmed();
    row.paintingMode = parts[10].trimmed();

    const QString lengthStr = parts[2].trimmed();
    bool okLength = false;
    row.stockLength = lengthStr.toDouble(&okLength);

    if (row.barcode.isEmpty() || !okLength || row.stockLength <= 0) {
        QString msg = L("⚠️ Érvénytelen barcode vagy hossz");
        ctx.addError(ctx.currentLineNumber(), msg);
        return std::nullopt;
    }

    return row;
}

std::optional<MaterialMaster>
MaterialRepository::buildMaterialFromRow(const MaterialRow& row, CsvImporter::FileContext& ctx) {
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

    if (m.shape == CrossSectionShape(CrossSectionShape::Shape::Rectangular)) {
        if (row.dim1.isEmpty() || row.dim2.isEmpty()) {
            ctx.addError(ctx.currentLineNumber(), L("⚠️ Hiányzó szélesség/magasság adat"));
        } else {
            bool okW = false, okH = false;
            double w = row.dim1.toDouble(&okW);
            double h = row.dim2.toDouble(&okH);
            if (!okW || !okH || w <= 0 || h <= 0) {
                QString msg = L("⚠️ Érvénytelen szélesség/magasság");
                ctx.addError(ctx.currentLineNumber(), msg);
            }
            m.size_mm = QSizeF(w, h);
        }
    }
    else if (m.shape == CrossSectionShape(CrossSectionShape::Shape::Round)) {
        if (row.dim1.isEmpty()) {
            ctx.addError(ctx.currentLineNumber(), L("⚠️ Hiányzó átmérő adat"));
        } else {
            bool okD = false;
            double d = row.dim1.toDouble(&okD);
            if (!okD || d <= 0) {
                QString msg = L("⚠️ Érvénytelen átmérő");
                ctx.addError(ctx.currentLineNumber(), msg);
            }
            m.diameter_mm = d;
        }
    }

    // 🎨 Szín hozzárendelés – RAL, HEX vagy üres
    if (!row.colorStr.isEmpty()) {
        m.color = NamedColor(row.colorStr);
        if (!m.color.isValid()) {
            QString msg = L("⚠️ Ismeretlen színformátum: %1").arg( row.colorStr);
            ctx.addError(ctx.currentLineNumber(), msg);
        }
    } else {
        m.color = NamedColor(); // nincs festve
    }

    return m;
}


