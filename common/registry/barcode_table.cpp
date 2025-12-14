#include "barcode_table.h"
#include "common/logger/logger.h"
#include <algorithm>
#include "common/system/verbose_manager.h"

BarcodeTable& BarcodeTable::instance() {
    static BarcodeTable inst;
    return inst;
}

int BarcodeTable::count() const {
    QMutexLocker lock(&_mtx);
    return _records.size();
}

int BarcodeTable::activeCount() const {
    //QMutexLocker lock(&_mtx);
    return std::count_if(_records.begin(), _records.end(),
                         [](const Record& r){ return r.status == Status::Active; });
}

int BarcodeTable::retiredCount() const {
    //QMutexLocker lock(&_mtx);
    return std::count_if(_records.begin(), _records.end(),
                         [](const Record& r){ return r.status == Status::Retired; });
}

std::optional<int> BarcodeTable::indexOf(const QString& code) const {
    for (int i = 0; i < _records.size(); ++i) {
        if (_records[i].code == code)
            return i;
    }
    return std::nullopt;
}

bool BarcodeTable::exists(const QString& code) const {
    QMutexLocker lock(&_mtx);
    return indexOf(code).has_value();
}

bool BarcodeTable::isActive(const QString& code) const {
    QMutexLocker lock(&_mtx);
    auto idx = indexOf(code);
    if (!idx.has_value()) return false;
    return _records[idx.value()].status == Status::Active;
}

bool BarcodeTable::checkUnique(const QString& code,
                               const QString& entityType,
                               const QUuid& id) const
{
    Q_UNUSED(entityType)
    Q_UNUSED(id)
    // Hunglish: a uniq check itt brutál egyértelmű — ha létezik bármilyen formában, nem unique
    return !exists(code);
}

bool BarcodeTable::registerNew(const QString& code,
                               const QString& entityType,
                               const QUuid& id)
{
    QMutexLocker lock(&_mtx);

    if (exists(code)) {
        zInfo(QString("Barcode collision on registerNew: %1").arg(code));
        return false;
    }

    Record r;
    r.code = code;
    r.entityType = entityType;
    r.entityId = id;
    r.introducedAt = QDateTime::currentDateTime();
    r.status = Status::Active;
    _records.append(r);

    if(IS_VERBOSE_THIS())
    {
        zInfo(QString("Barcode introduced: %1 (%2, id=%3)")
                   .arg(code, entityType, id.toString(QUuid::WithoutBraces)));
    }
    return true;
}

bool BarcodeTable::retire(const QString& code, const QString& reason) {
    QMutexLocker lock(&_mtx);

    auto idx = indexOf(code);
    if (!idx.has_value()) {
        zWarning(QString("Retire requested for unknown code: %1").arg(code));
        return false;
    }

    auto& r = _records[idx.value()];
    if (r.status == Status::Retired) {
        zWarning(QString("Retire requested for already retired code: %1").arg(code));
        return false;
    }

    r.status = Status::Retired;
    r.retiredAt = QDateTime::currentDateTime();

    zWarning(QString("Barcode retired: %1 (reason=%2)").arg(code, reason));
    return true;
}

void BarcodeTable::dumpSummary() const {
    QMutexLocker lock(&_mtx);
    zInfo(QString("BarcodeTable summary: total=%1, active=%2, retired=%3")
                   .arg(QString::number(_records.size()),
                        QString::number(activeCount()),
                        QString::number(retiredCount())));
}

std::optional<BarcodeTable::Record> BarcodeTable::find(const QString& code) const {
    QMutexLocker lock(&_mtx);
    auto idx = indexOf(code);
    if (!idx.has_value()) return std::nullopt;
    return _records[idx.value()];
}
