#pragma once

#include <QString>
#include <QDateTime>
#include <QUuid>
#include <QVector>
#include <QMutex>
#include <optional>
#include "common/logger/event_logger.h"

/**
 * BarcodeTable – globális, auditbarát gyűjtőtábla minden valaha regisztrált barcode-hoz.
 *
 * Hunglish magyarázat:
 * - Ez a "könyvelés" a barcode-oknak. Amikor egy entitás ténylegesen bekerül a registry-be,
 *   itt regisztráljuk a kódját (introduced). Amikor töröljük, itt nyugdíjazzuk (retired).
 * - Soha nem adunk ki újra egy már használt barcode-ot (globális uniqueness a teljes történeti készletre).
 */
class BarcodeTable {
public:
    enum class Status { Active, Retired };

    struct Record {
        QString code;
        QString entityType;   // pl. "Material" vagy "Product" – emberbarát, audit logban szép
        QUuid entityId;
        QDateTime introducedAt;
        std::optional<QDateTime> retiredAt;
        Status status = Status::Active;
    };

    static BarcodeTable& instance();

    // Globális összefoglaló – debug/audit helper
    int count() const;
    int activeCount() const;
    int retiredCount() const;

    // Uniqueness ellenőrzés: a teljes halmazra (active + retired)
    bool exists(const QString& code) const;
    bool isActive(const QString& code) const;

    /**
     * checkUnique – visszaadja, hogy a code kiadható-e még valaha.
     * Ha a code már szerepel (active vagy retired), NEM unique.
     * entityType és id csak audit információ (nem befolyásolja a uniqueness-et).
     */
    bool checkUnique(const QString& code,
                     const QString& entityType,
                     const QUuid& id) const;

    /**
     * registerNew – ténylegesen beírja a kódot Active státusszal.
     * Feltétele: előtte checkUnique == true. Ha mégsem, false visszatér.
     */
    bool registerNew(const QString& code,
                     const QString& entityType,
                     const QUuid& id);

    /**
     * retire – egy létező (Active) kódot Retired státuszba tesz. Ha nem létezik vagy már retired, false.
     * reason: emberbarát ok (pl. "Material deleted")
     */
    bool retire(const QString& code, const QString& reason);

    // Debug/audit: kiírható összefoglaló
    void dumpSummary() const;


    std::optional<BarcodeTable::Record> find(const QString& code) const;

private:
    BarcodeTable() = default;
    BarcodeTable(const BarcodeTable&) = delete;
    BarcodeTable& operator=(const BarcodeTable&) = delete;

    std::optional<int> indexOf(const QString& code) const;

    // Thread-safety: egy egyszerű QMutex elég itt, mivel ritkán írjuk és egyszerű konténer.
    mutable QRecursiveMutex _mtx;
    //mutable QMutex _mtx;
    QVector<Record> _records;
};
