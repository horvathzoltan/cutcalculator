#include "matrix_generator.h"
#include "matrix_validator.h"
#include "calculation/registry/need_calculation_detail_registry.h"

#include <QDateTime>

#include <materials/registry/material_registry.h>
//#include "common/logger/event_logger.h"

bool MatrixGenerator::generate(const QVector<MissingDetail>& list)
{
    auto start = QDateTime::currentDateTime();

    zInfo("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    zInfo("🛠 MatrixGenerator session started");
    zInfo("Strategy: on-demand auto-heal (mode activation)");

    zInfo(QString("MissingDetail count: %1").arg(list.size()));


    QSet<QUuid> products, modes, materials;
    for (const auto& md : list) {
        products.insert(md.productId);
        modes.insert(md.modeId);
        materials.insert(md.materialId);
    }

    zInfo(QString("Products affected: %1").arg(products.size()));
    zInfo(QString("Modes affected: %1").arg(modes.size()));
    zInfo(QString("Materials affected: %1").arg(materials.size()));
    zInfo("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");

    auto sorted = list;
    std::sort(sorted.begin(), sorted.end(), [](const MissingDetail& a, const MissingDetail& b) {
        if (a.productId != b.productId) return a.productId < b.productId;
        if (a.modeId != b.modeId) return a.modeId < b.modeId;
        return a.materialId < b.materialId;
    });
    for (const auto& m : sorted) {
        // NeedCalculationDetail d;
        // d.id = QUuid::createUuid();
        // d.needCalculationId = m.modeId;
        // d.materialId = m.materialId;
        // d.formula = ""; // v2: default formula = empty (unknown)
        // d.kind = NeedCalculationDetail::DetailKind::Cutting;

        // NeedCalculationDetailRegistry::instance().insert(d);
            createDetail(m.modeId, m.materialId);

        zInfo(QString("  ➕ Generated detail | product=%1 | mode=%2 | material=%3")
                  .arg(m.productId.toString())
                  .arg(m.modeId.toString())
                  .arg(m.materialId.toString()));

    }

    auto end = QDateTime::currentDateTime();
    auto ms = start.msecsTo(end);

    zInfo(QString("🛠 MatrixGenerator session finished in %1 ms").arg(ms));
    zInfo("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");

    return true;
}

void MatrixGenerator::createDetail(const QUuid& modeId, const QUuid& materialId)
{
    NeedCalculationDetail d;
    d.id = QUuid::createUuid();
    d.needCalculationId = modeId;
    d.materialId = materialId;
    d.formula = ""; // default formula

    auto* mat = MaterialRegistry::instance().findById(materialId);

    if (mat && mat->cuttingMode == CuttingMode::Piece)
        d.kind = NeedCalculationDetail::DetailKind::Kitting;
    else
        d.kind = NeedCalculationDetail::DetailKind::Cutting;

    NeedCalculationDetailRegistry::instance().insert(d);
}



// v2: generate missing details for a product in deterministic order
bool MatrixGenerator::generateForProduct(const QUuid& productId)
{
    auto missing = MatrixValidator::validateProduct(productId);
    return generate(missing);
}

// v2: generate missing details for a mode in deterministic order
bool MatrixGenerator::generateForMode(const QUuid& modeId)
{
    auto missing = MatrixValidator::validateMode(modeId);
    return generate(missing);
}
