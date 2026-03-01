#include "calculation/registry/need_calculation_detail_registry.h"
#include "common/registry/manager/registry_manager.h"
#include "calculation/repository/need_calculation_detail_repository.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "dsl/formula_analysis.h"
#include "dsl/formula_contract.h"
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"


// --- ÚJ: opt: validáció ---
bool NeedCalculationDetailRegistry::validateOpt(const QString& f)
{
    const QString trimmed = f.trimmed();
    if (trimmed.isEmpty())
        return true;

    // Tokenizálás: minden + jel mentén
    QStringList tokens = trimmed.split('+', Qt::SkipEmptyParts);

    for (QString tok : tokens) {
        tok = tok.trimmed();

        // Csak az opt: tokeneket vizsgáljuk
        if (!tok.startsWith("opt:"))
            continue;

        // opt:<flag>:+<value>
        // Példa: opt:paint:+40

        int first = tok.indexOf(':');             // "opt:"
        int second = tok.indexOf(':', first + 1); // flag után

        if (second < 0)
            return false;

        QString flag = tok.mid(first + 1, second - first - 1).trimmed();
        if (flag.isEmpty())
            return false;

        QString valStr = tok.mid(second + 1).trimmed(); // +40 vagy -5

        bool ok = false;
        valStr.toInt(&ok);
        if (!ok)
            return false;
    }

    return true;
}

// --- ÚJ: choose: validáció ---
bool NeedCalculationDetailRegistry::validateChoose(const QString& f)
{
    QString t = f.trimmed();
    if (!t.startsWith("choose:"))
        return false;

    // Levágjuk a prefixet
    t = t.mid(QStringLiteral("choose:").size()).trimmed();

    // Kell benne lennie '?' és ':' jeleknek
    int q = t.indexOf('?');
    int c = t.indexOf(':', q + 1);

    if (q < 0 || c < 0)
        return false;

    // Három rész: feltétel, true ág, false ág
    QString cond = t.left(q).trimmed();
    QString trueBranch = t.mid(q + 1, c - q - 1).trimmed();
    QString falseBranch = t.mid(c + 1).trimmed();

    // Egyik sem lehet üres
    if (cond.isEmpty()) return false;
    if (trueBranch.isEmpty()) return false;
    if (falseBranch.isEmpty()) return false;

    // Feltételben legyen valamilyen összehasonlító operátor
    if (!(cond.contains(">=") ||
          cond.contains("<=") ||
          cond.contains("==") ||
          cond.contains(">")  ||
          cond.contains("<")))
        return false;

    return true;
}


// --- Kényelmi API ---

bool NeedCalculationDetailRegistry::updateFormula(const QUuid& id,
                                                  const QString& newFormula)
{
    const auto* existing = findById(id);
    if (!existing)
        return false;

    auto updated = *existing;
    updated.formula = newFormula;

    return updateWithWorkflow(updated);
}


// --- Lookup API ---

QVector<NeedCalculationDetail>
NeedCalculationDetailRegistry::findByCalculation(const QUuid& calcId) const
{
    return findAll([&](const NeedCalculationDetail& d){
        return d.needCalculationId == calcId;
    });
}

bool NeedCalculationDetailRegistry::insert(const NeedCalculationDetail &d) {
    return insertWithWorkflow(d);
}

bool NeedCalculationDetailRegistry::update(const NeedCalculationDetail &d) {
    return updateWithWorkflow(d);
}

bool NeedCalculationDetailRegistry::remove(const QUuid &id) {
    return removeWithWorkflow(id);
}

// --- Domain hookok ---

// v2: domain validation uses updated formula rules (isFormulaValid)
bool NeedCalculationDetailRegistry::validateDomain(const NeedCalculationDetail& d) const
{
    // Formula üres → valid
    if (d.formula.trimmed().isEmpty())
        return true;

    // Formula nem lehet "unknown"
    if (d.formula.trimmed() == "unknown")
        return false;

    // Kind ellenőrzés
    if (d.kind != NeedCalculationDetail::DetailKind::Cutting &&
        d.kind != NeedCalculationDetail::DetailKind::Kitting)
        return false;

    // Material létezzen
    if (!materialExists(d.materialId))
        return false;

    // Calculation létezzen
    const NeedCalculation* nc =
        NeedCalculationRegistry::instance().findById(d.needCalculationId);
    if (!nc)
        return false;

    // Product létezzen
    auto p = ProductRegistry::instance().findById(nc->productId);
    if (!p)
        return false;

    return true;
}


bool NeedCalculationDetailRegistry::validateDuplicate(const NeedCalculationDetail& d) const
{
    return !existsBy([&](const NeedCalculationDetail& x){
        return x.needCalculationId == d.needCalculationId
               && x.materialId == d.materialId
               && x.id != d.id;
    });
}

bool NeedCalculationDetailRegistry::beforeInsert(NeedCalculationDetail &d) { return validateMaterial(d); }

bool NeedCalculationDetailRegistry::beforeUpdate(NeedCalculationDetail &d) { return validateMaterial(d); }

/*
// --- Shadow-mode előkészítés ---
// A log hookok teljesen mellékhatás-mentesek (csak zInfo).
// A persist() tiszta, nem módosít memóriát és nem hív CRUD-ot.
// A registry készen áll a log→persist sorrend későbbi átállítására (WF-1/WF-2).
*/

// v2: log reflects formula state (empty=valid, "unknown"=invalid)
void NeedCalculationDetailRegistry::onInsertLog(const NeedCalculationDetail& d)
{
    zInfo(QString("➕ NeedCalculationDetail INSERT: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

// v2: log reflects updated formula state
void NeedCalculationDetailRegistry::onUpdateLog(const NeedCalculationDetail& d)
{
    zInfo(QString("✏️ NeedCalculationDetail UPDATE: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

// v2: log reflects formula state at removal
void NeedCalculationDetailRegistry::onRemoveLog(const NeedCalculationDetail& d)
{
    zInfo(QString("🗑️ NeedCalculationDetail REMOVE: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

/*
// --- AfterHook-ok ---
// Mellékhatás-mentes, opcionális UI/notification lépések.
// Workflow sorrend: persist → log → afterHook.
void afterInsert(const NeedCalculationDetail&);
void afterUpdate(const NeedCalculationDetail&);
void afterRemove(const NeedCalculationDetail&);
*/
void NeedCalculationDetailRegistry::afterInsert(const NeedCalculationDetail& d)
{
    // Mellékhatás-mentes: opcionális UI/diagnosztikai jelzés
    zInfo(QString("ℹ️ NeedCalculationDetail AFTER INSERT: id=%1")
              .arg(d.id.toString()));
}

void NeedCalculationDetailRegistry::afterUpdate(const NeedCalculationDetail& d)
{
    zInfo(QString("ℹ️ NeedCalculationDetail AFTER UPDATE: id=%1")
              .arg(d.id.toString()));
}

void NeedCalculationDetailRegistry::afterRemove(const NeedCalculationDetail& d)
{
    zInfo(QString("ℹ️ NeedCalculationDetail AFTER REMOVE: id=%1")
              .arg(d.id.toString()));
}

// --- Persist ---

void NeedCalculationDetailRegistry::persist() const
{
    NeedCalculationDetailRepository::save(readAll());
}

// v2: registry load log reflects formula model v2 state
void NeedCalculationDetailRegistry::onLoadLog()
{
    zInfo(QString("📊 NeedCalculationDetailRegistry: %1 sor betöltve").arg(size()));
}

bool NeedCalculationDetailRegistry::validateMaterial(const NeedCalculationDetail& d)
{
    return materialExists(d.materialId);
}

bool NeedCalculationDetailRegistry::materialExists(const QUuid& materialId) {
    const auto* ent =
        RegistryManager::instance().findEntity("MaterialMaster", materialId);
    return ent != nullptr;
}

