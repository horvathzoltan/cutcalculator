#include "barcode_registry_tester.h"

#include "barcodes/registry/barcode_registry.h"
#include "barcodes/repository/barcode_repository.h"
#include "common/logger/logger.h"

#include <QDateTime>
#include <QUuid>

bool BarcodeRegistryTester::run()
{
    zInfo("=== BarcodeRegistry TESTS START ===");

    testRegisterNewUnique();
    testRegisterCollisionDifferentEntity();
    testRegisterSameEntityTwice();
    testRetire();
    testRetireThenRegisterAgain();
    testEntityIdMissingNoCollision();
    testCsvRoundtrip();

    zInfo("=== BarcodeRegistry TESTS END ===");
    return true;
}

/* ============================================================
 * 🧪 1) Új, egyedi barcode regisztrálása
 * ============================================================ */
void BarcodeRegistryTester::testRegisterNewUnique()
{
    zInfo("→ testRegisterNewUnique");

    auto& reg = BarcodeRegistry::instance();
    reg.clearForTest();

    const QUuid id = QUuid::createUuid();
    const QString code = "ABC123";

    bool ok = reg.registerNew(code, "Material", id, "Mat1");
    Q_ASSERT(ok);

    const BarcodeRecord* rec = reg.findByCode(code);
    Q_ASSERT(rec != nullptr);
    Q_ASSERT(rec->code == code);
    Q_ASSERT(rec->entityType == "Material");
    Q_ASSERT(rec->entityId.has_value());
    Q_ASSERT(rec->entityId.value() == id);
    Q_ASSERT(rec->introducedAt.isValid());
    Q_ASSERT(!rec->retiredAt.has_value());
    Q_ASSERT(rec->isActive());

    zInfo("✓ testRegisterNewUnique OK");
}


/* ============================================================
 * 🧪 2) Ütközés – külön entityId → FAIL
 * ============================================================ */
void BarcodeRegistryTester::testRegisterCollisionDifferentEntity()
{
    zInfo("→ testRegisterCollisionDifferentEntity");

    auto& reg = BarcodeRegistry::instance();
    reg.clearForTest();

    QUuid id1 = QUuid::createUuid();
    QUuid id2 = QUuid::createUuid();

    // első regisztráció → siker
    bool ok1 = reg.registerNew("ABC", "Material", id1, "Mat1");
    Q_ASSERT(ok1);

    // második, más entityId → ütközés → false
    bool ok2 = reg.registerNew("ABC", "Material", id2, "Mat2");
    Q_ASSERT(!ok2);

    zInfo("✓ testRegisterCollisionDifferentEntity OK");
}


/* ============================================================
 * 🧪 3) Ugyanaz az entityId → második hívás: OK (no‑op)
 * ============================================================ */
void BarcodeRegistryTester::testRegisterSameEntityTwice()
{
    zInfo("→ testRegisterSameEntityTwice");

    auto& reg = BarcodeRegistry::instance();
    reg.clearForTest();

    QUuid id = QUuid::createUuid();

    bool ok1 = reg.registerNew("ABC", "Material", id, "Mat1");
    Q_ASSERT(ok1);

    // ugyanaz a code + ugyanaz az entityId → no-op, de true
    bool ok2 = reg.registerNew("ABC", "Material", id, "Mat1");
    Q_ASSERT(ok2);

    const BarcodeRecord* rec = reg.findByCode("ABC");
    Q_ASSERT(rec != nullptr);
    Q_ASSERT(rec->entityId.has_value());
    Q_ASSERT(rec->entityId == id);
    Q_ASSERT(rec->isActive());

    zInfo("✓ testRegisterSameEntityTwice OK");
}


/* ============================================================
 * 🧪 4) Retire működés
 * ============================================================ */
void BarcodeRegistryTester::testRetire()
{
    zInfo("→ testRetire");

    auto& reg = BarcodeRegistry::instance();
    reg.clearForTest();

    QUuid id = QUuid::createUuid();

    // 1) új barcode regisztrálása
    bool ok1 = reg.registerNew("ABC123", "Material", id, "MatX");
    Q_ASSERT(ok1);

    const BarcodeRecord* rec1 = reg.findByCode("ABC123");
    Q_ASSERT(rec1 != nullptr);
    Q_ASSERT(rec1->isActive());
    Q_ASSERT(!rec1->retiredAt.has_value());

    // 2) retire művelet
    bool ok2 = reg.retire("ABC123", "test-retire");
    Q_ASSERT(ok2);

    const BarcodeRecord* rec2 = reg.findByCode("ABC123");
    Q_ASSERT(rec2 != nullptr);
    Q_ASSERT(!rec2->isActive());
    Q_ASSERT(rec2->retiredAt.has_value());

    zInfo("✓ testRetire OK");
}


/* ============================================================
 * 🧪 5) Retire után újra regisztrálás → ledger szerint NEM engedett
 * ============================================================ */
void BarcodeRegistryTester::testRetireThenRegisterAgain()
{
    zInfo("→ testRetireThenRegisterAgain");

    auto& reg = BarcodeRegistry::instance();
    reg.clearForTest();

    QUuid id = QUuid::createUuid();

    // 1) első regisztráció → siker
    bool ok1 = reg.registerNew("RET2", "Product", id, "ProdX");
    Q_ASSERT(ok1);

    // 2) retire → siker
    bool ok2 = reg.retire("RET2", "test-retire");
    Q_ASSERT(ok2);

    const BarcodeRecord* rec = reg.findByCode("RET2");
    Q_ASSERT(rec != nullptr);
    Q_ASSERT(!rec->isActive());
    Q_ASSERT(rec->retiredAt.has_value());

    // 3) újra regisztrálni ugyanazt a kódot → ledger szerint TILTOTT
    bool ok3 = reg.registerNew("RET2", "Product", id, "ProdX-again");
    Q_ASSERT(!ok3);

    zInfo("✓ testRetireThenRegisterAgain OK");
}


/* ============================================================
 * 🧪 6) entityId hiányzik → nem számít collision‑nek
 * ============================================================ */
void BarcodeRegistryTester::testEntityIdMissingNoCollision()
{
    zInfo("→ testEntityIdMissingNoCollision");

    auto& reg = BarcodeRegistry::instance();
    reg.clearForTest();

    // 1) CSV-ből jövő rekord szimulálása → entityId = nullopt
    BarcodeRecord csvRec;
    csvRec.id = QUuid::createUuid();
    csvRec.code = "CSVX";
    csvRec.entityType = "Material";
    csvRec.introducedAt = QDateTime::currentDateTime();
    csvRec.entityId = std::nullopt;

    bool okInsert = reg.insertForTest(csvRec);
    Q_ASSERT(okInsert);

    const BarcodeRecord* r1 = reg.findByCode("CSVX");
    Q_ASSERT(r1 != nullptr);
    Q_ASSERT(!r1->entityId.has_value());   // CSV import → nincs entityId

    // 2) Ugyanaz a code, de most már van entityId → NEM collision
    QUuid newId = QUuid::createUuid();
    bool ok2 = reg.registerNew("CSVX", "Material", newId, "MatY");
    Q_ASSERT(ok2);

    const BarcodeRecord* r2 = reg.findByCode("CSVX");
    Q_ASSERT(r2 != nullptr);
    Q_ASSERT(r2->entityId.has_value());
    Q_ASSERT(r2->entityId.value() == newId);

    zInfo("✓ testEntityIdMissingNoCollision OK");
}


/* ============================================================
 * 🧪 7) CSV roundtrip – save → load → összehasonlítás
 * ============================================================ */
void BarcodeRegistryTester::testCsvRoundtrip()
{
    zInfo("→ testCsvRoundtrip");

    auto& reg = BarcodeRegistry::instance();
    reg.clearForTest();

    // 1) Két ledger-rekord létrehozása
    {
        BarcodeRecord r1;
        r1.id = QUuid::createUuid();
        r1.code = "C1";
        r1.entityType = "Material";
        r1.introducedAt = QDateTime::fromString("2024-01-01T10:00:00", Qt::ISODate);
        Q_ASSERT(reg.insertForTest(r1));

        BarcodeRecord r2;
        r2.id = QUuid::createUuid();
        r2.code = "C2";
        r2.entityType = "Product";
        r2.introducedAt = QDateTime::fromString("2024-01-02T10:00:00", Qt::ISODate);
        r2.retiredAt = QDateTime::fromString("2024-02-01T10:00:00", Qt::ISODate);
        Q_ASSERT(reg.insertForTest(r2));
    }

    // 2) Export → CSV
    reg.persist();

    // 3) Import → QVector<BarcodeRecord>
    QVector<BarcodeRecord> loaded;
    bool ok = BarcodeRepository::load(loaded);
    Q_ASSERT(ok);
    Q_ASSERT(loaded.size() == 2);

    // 4) Ellenőrzés: mindkét rekord megvan
    auto it1 = std::find_if(loaded.begin(), loaded.end(),
                            [](const BarcodeRecord& r){ return r.code == "C1"; });
    auto it2 = std::find_if(loaded.begin(), loaded.end(),
                            [](const BarcodeRecord& r){ return r.code == "C2"; });

    Q_ASSERT(it1 != loaded.end());
    Q_ASSERT(it2 != loaded.end());

    // 5) Mezők ellenőrzése
    Q_ASSERT(it1->entityType == "Material");
    Q_ASSERT(!it1->retiredAt.has_value());

    Q_ASSERT(it2->entityType == "Product");
    Q_ASSERT(it2->retiredAt.has_value());
    Q_ASSERT(it2->retiredAt->toString(Qt::ISODate) == "2024-02-01T10:00:00");

    zInfo("✓ testCsvRoundtrip OK");
}
