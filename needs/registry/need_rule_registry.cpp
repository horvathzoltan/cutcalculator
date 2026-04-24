#include "needs/registry/need_rule_registry.h"
#include "calculation/model/need_calculation_detail.h"
#include "needs/repository/need_rule_repository.h"

#include "calcmodes/registry/need_calculation_registry.h"

#include "calculation/registry/need_calculation_detail_registry.h"

#include <calculation/service/matrix_generator.h>

// --- Lookup API ---

NeedRuleRegistry::NeedRuleRegistry()
    : RegistryEngineBase("NeedRuleRegistry", "NeedRule")
{
    // A betöltést a StartupManager végzi, nem a konstruktor.
}



QVector<NeedRule>
NeedRuleRegistry::findByLeft(const QUuid& leftId) const
{
    return findAll([&](const NeedRule& r){
        return r.leftId == leftId;
    });
}

// --- Domain hookok ---

bool NeedRuleRegistry::validateConnection(const NeedRule& r) const
{
    // A ConnectionEntity már tud bal/jobb lookupot
    const bool leftOk  = r.left().has_value();
    const bool rightOk = r.right().has_value();
    return leftOk && rightOk;
}

bool NeedRuleRegistry::validateDuplicate(const NeedRule& r) const
{
    return !existsBy([&](const NeedRule& x){
        return x.leftId  == r.leftId
               && x.rightId == r.rightId;
    });
}


void NeedRuleRegistry::afterInsert(const NeedRule& r)
{
    const auto modes =
        NeedCalculationRegistry::instance().findAll([&](const NeedCalculation& nc){
            return nc.productId == r.leftId;
        });

    for (const auto& m : modes) {
        // NeedCalculationDetail d;
        // d.id = QUuid::createUuid();
        // d.needCalculationId = m.id;
        // d.materialId = r.rightId;
        // d.formula = "";
        // d.kind = NeedCalculationDetail::DetailKind::Cutting;

        // NeedCalculationDetailRegistry::instance().insert(d);
        MatrixGenerator::createDetail(m.id, r.rightId);
    }
}

void NeedRuleRegistry::afterRemove(const NeedRule& r)
{
    // Töröljük az összes detail-t, ami:
    // - ehhez a materialhoz tartozik
    // - és olyan módhoz, amelynek productId == r.leftId
    auto details = NeedCalculationDetailRegistry::instance().findAll(
        [&](const NeedCalculationDetail& d){
            const auto* calc =
                NeedCalculationRegistry::instance().findById(d.needCalculationId);
            if (!calc)
                return false;
            return d.materialId == r.rightId &&
                   calc->productId == r.leftId;
        });

    for (const auto& d : details) {
        // 5.1 Audit log: törlés oka
        zInfo(QString("🧹 NeedRule törlés miatt detail törlés: detailId=%1, material=%2, product=%3")
                  .arg(d.id.toString(), r.rightId.toString(), r.leftId.toString()));

        // 5.2 Sikertelen törlés logolása
        if (!NeedCalculationDetailRegistry::instance().remove(d.id)) {
            zWarning(QString("⚠️ Sikertelen detail törlés NeedRule törlés miatt: detailId=%1")
                         .arg(d.id.toString()));
        }
    }
}


// --- Log hookok ---

void NeedRuleRegistry::onInsertLog(const NeedRule& r)
{
    zInfo(QString("➕ NeedRule INSERT: left=%1 right=%2")
              .arg(r.leftId.toString(), r.rightId.toString()));
}

void NeedRuleRegistry::onRemoveLog(const NeedRule& r)
{
    zInfo(QString("🗑️ NeedRule REMOVE: left=%1 right=%2")
              .arg(r.leftId.toString(), r.rightId.toString()));
}

// --- Persist ---

void NeedRuleRegistry::persist() const
{
    NeedRuleRepository::save();
}


