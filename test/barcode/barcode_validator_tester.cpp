#include "barcode_validator_tester.h"

#include "barcodes/validator/barcode_validator.h"
#include "barcodes/helpers/barcode_generator.h"
#include "common/logger/logger.h"

bool BarcodeValidatorTester::run()
{
    zInfo("=== BarcodeValidator TESTS START ===");

    testEmptyCodeRejected();
    testInvalidCharactersRejected();
    testDuplicateCodeRejected();
    testDifferentEntityAllowed();
    testRetiredCodeRejected();
    testEntityIdRequired();
    testPrefixRules();
    testGeneratorIntegration();
    testFallbackLogic();

    zInfo("=== BarcodeValidator TESTS END ===");
    return true;
}

/* ============================================================
 * A tényleges tesztmetódusok a következő patchekben jönnek:
 * PATCH 7 #3 … #11
 * ============================================================ */
void BarcodeValidatorTester::testEmptyCodeRejected()
{
    zInfo("→ testEmptyCodeRejected");

    // A Validator CSV-import kontextust vár (operationName, filepath)
    CsvImporter::FileContext ctx("test-op", "test.csv");

    QString code = "";
    QString entityType = "Material";
    QUuid id = QUuid::createUuid();
    QString name = "TestMaterial";

    // A Validator singleton registryt használ, nekünk nem kell példányosítani
    bool ok = BarcodeValidator::checkAndRegister_CSV(code, entityType, id, name, ctx);

    // 1) A validáció sikertelen kell legyen
    Q_ASSERT(ok == false);

    // 2) Audit hiba keletkezik → legalább 1 error
    Q_ASSERT(ctx.errorsSize() >= 1);

    zInfo("✓ testEmptyCodeRejected OK");
}

void BarcodeValidatorTester::testInvalidCharactersRejected()
{
    zInfo("→ testInvalidCharactersRejected");

    // A Validator CSV-import kontextust vár
    CsvImporter::FileContext ctx("test-op", "test.csv");

    // Tiltott karaktereket tartalmazó kódok
    QStringList invalidCodes = {
        "ABC 123",   // szóköz
        "ABC#123",   // speciális karakter
        "ÁBC123",    // ékezet
        "ABC/123",   // slash
        "ABC\\123"   // backslash
    };

    QString entityType = "Material";
    QUuid id = QUuid::createUuid();
    QString name = "TestMaterial";

    for (const QString& code : invalidCodes) {
        CsvImporter::FileContext localCtx("test-op", "test.csv");

        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     entityType,
                                                     id,
                                                     name,
                                                     localCtx);

        // 1) A validáció sikertelen kell legyen
        Q_ASSERT(ok == false);

        // 2) Audit hiba keletkezik → legalább 1 error
        Q_ASSERT(localCtx.errorsSize() >= 1);
    }

    zInfo("✓ testInvalidCharactersRejected OK");
}

void BarcodeValidatorTester::testDuplicateCodeRejected()
{
    zInfo("→ testDuplicateCodeRejected");

    QString entityType = "Material";
    QString name = "TestMaterial";
    QString code = "DUPL001";

    QUuid id1 = QUuid::createUuid();
    QUuid id2 = QUuid::createUuid();

    // 1) Első regisztráció → sikeres kell legyen
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(code, entityType, id1, name, ctx);

        Q_ASSERT(ok == true);
        Q_ASSERT(ctx.errorsSize() == 0);
    }

    // 2) Második regisztráció ugyanazzal a kóddal → el kell utasítani
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(code, entityType, id2, name, ctx);

        Q_ASSERT(ok == false);
        Q_ASSERT(ctx.errorsSize() >= 1);   // collision error expected
    }

    zInfo("✓ testDuplicateCodeRejected OK");
}

void BarcodeValidatorTester::testDifferentEntityAllowed()
{
    zInfo("→ testDifferentEntityAllowed");

    QString code = "SAME001";
    QString name = "TestName";

    QUuid id1 = QUuid::createUuid();
    QUuid id2 = QUuid::createUuid();

    // 1) Első regisztráció: Material
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     "Material",
                                                     id1,
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == true);
        Q_ASSERT(ctx.errorsSize() == 0);
    }

    // 2) Második regisztráció: Product (ugyanaz a code, más entityType)
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     "Product",
                                                     id2,
                                                     name,
                                                     ctx);

        // Külön entityType → NEM engedélyezett (globális barcode)
        Q_ASSERT(ok == false);
        Q_ASSERT(ctx.errorsSize() >= 1);
    }

    zInfo("✓ testDifferentEntityAllowed OK");
}


void BarcodeValidatorTester::testRetiredCodeRejected()
{
    zInfo("→ testRetiredCodeRejected");

    QString code = "RET001";
    QString entityType = "Material";
    QString name = "TestMaterial";

    QUuid id = QUuid::createUuid();

    // 1) Első regisztráció → sikeres
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     entityType,
                                                     id,
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == true);
        Q_ASSERT(ctx.errorsSize() == 0);
    }

    // 2) Nyugdíjazás
    {
        BarcodeRegistry& r = BarcodeRegistry::instance();
        r.retire(code, "test-retire");
        // retire() nem ad vissza hibát, csak auditál
    }

    // 3) Újra regisztrálás → el kell utasítani
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     entityType,
                                                     QUuid::createUuid(),
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == false);
        Q_ASSERT(ctx.errorsSize() >= 1);   // retired code → collision error expected
    }

    zInfo("✓ testRetiredCodeRejected OK");
}


void BarcodeValidatorTester::testEntityIdRequired()
{
    zInfo("→ testEntityIdRequired");

    QString code = "NOID001";
    QString entityType = "Material";
    QString name = "TestMaterial";

    // Null UUID → kötelező mező hiánya
    QUuid nullId;  // QUuid() → 00000000-0000-0000-0000-000000000000

    CsvImporter::FileContext ctx("test-op", "test.csv");

    bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                 entityType,
                                                 nullId,
                                                 name,
                                                 ctx);

    // 1) A validáció sikertelen kell legyen
    // Null UUID → továbbra is engedélyezett (entityId opcionális)
    Q_ASSERT(ok == true);
    Q_ASSERT(ctx.errorsSize() == 0);

    zInfo("✓ testEntityIdRequired OK");
}

void BarcodeValidatorTester::testPrefixRules()
{
    zInfo("→ testPrefixRules");

    QString entityType = "Material";
    QString name = "TestMaterial";
    QUuid id = QUuid::createUuid();

    // 1) Whitespace prefix → trim után OK → engedélyezett
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        QString code = "   OK123";

        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     entityType,
                                                     id,
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == true);
        Q_ASSERT(ctx.errorsSize() == 0);
    }

    // 2) Speciális karakter prefix → tiltott
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        QString code = "@@ABC123";

        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     entityType,
                                                     QUuid::createUuid(),
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == false);
        Q_ASSERT(ctx.errorsSize() >= 1);
    }

    // 3) Túl hosszú prefix → tiltott (domain szabály)
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        QString code = "PREFIX_TOO_LONG_1234567890_ABC";

        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     entityType,
                                                     QUuid::createUuid(),
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == false);
        Q_ASSERT(ctx.errorsSize() >= 1);
    }

    // 4) Ékezetes prefix → tiltott
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        QString code = "ÁBC123";

        bool ok = BarcodeValidator::checkAndRegister_CSV(code,
                                                     entityType,
                                                     QUuid::createUuid(),
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == false);
        Q_ASSERT(ctx.errorsSize() >= 1);
    }

    zInfo("✓ testPrefixRules OK");
}

void BarcodeValidatorTester::testGeneratorIntegration()
{
    zInfo("→ testGeneratorIntegration");

    QString entityType = "Material";
    QString name = "GeneratedMaterial";

    // 1) Generálunk egy kódot
    QString generated = BarcodeGenerator::generate("MAT", name, 8);
    Q_ASSERT(!generated.isEmpty());

    // 2) Első regisztráció → sikeres kell legyen
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(generated,
                                                     entityType,
                                                     QUuid::createUuid(),
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == true);
        Q_ASSERT(ctx.errorsSize() == 0);
    }

    // 3) Második regisztráció ugyanazzal a kóddal → ütközés → elutasítás
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(generated,
                                                     entityType,
                                                     QUuid::createUuid(),
                                                     name,
                                                     ctx);

        Q_ASSERT(ok == false);
        Q_ASSERT(ctx.errorsSize() >= 1);   // collision expected
    }

    zInfo("✓ testGeneratorIntegration OK");
}


void BarcodeValidatorTester::testFallbackLogic()
{
    zInfo("→ testFallbackLogic");

    //
    // A fallback logika lényege:
    //
    // - A prefix NEM változik (domain jelentésű → pl. X-, MAT-, PRD-)
    // - Ha a prefix alatt minden egyszerű kód foglalt,
    //   a Generator NEM prefixet vált, hanem SLUG fallbacket használ.
    //
    // Példa fallback:
    //   X-0 … X-9  → mind foglalt
    //   → X-E-FALLB   (új slug)
    //
    // A teszt ezt ellenőrzi.
    //

    QString prefix = "X-";
    QString entityType = "Material";
    QString name = "FallbackTest";

    // 1) Feltöltjük a prefix egyszerű kódjait: X-0 … X-9
    for (int i = 0; i < 10; ++i) {
        QString code = prefix + QString::number(i);

        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(
            code,
            entityType,
            QUuid::createUuid(),
            name,
            ctx
            );

        Q_ASSERT(ok == true);
        Q_ASSERT(ctx.errorsSize() == 0);
    }

    //
    // 2) A Generator most már nem tud X-0 … X-9 formátumú kódot adni,
    //    ezért SLUG fallbacket fog használni.
    //
    QString generated = BarcodeGenerator::generate(prefix, name, 1);

    Q_ASSERT(!generated.isEmpty());
    Q_ASSERT(generated.startsWith(prefix));     // prefix marad → helyes
    Q_ASSERT(generated != prefix + "0");        // nem egyszerű token
    Q_ASSERT(generated != prefix + "1");
    Q_ASSERT(generated != prefix + "2");
    Q_ASSERT(generated != prefix + "3");
    Q_ASSERT(generated != prefix + "4");
    Q_ASSERT(generated != prefix + "5");
    Q_ASSERT(generated != prefix + "6");
    Q_ASSERT(generated != prefix + "7");
    Q_ASSERT(generated != prefix + "8");
    Q_ASSERT(generated != prefix + "9");

    // A fallback slug jellemzően tartalmaz "FALL" mintát
    Q_ASSERT(generated.contains("FALL"));       // slug fallback → helyes

    //
    // 3) A fallback kódot a Validatornak el kell fogadnia
    //
    {
        CsvImporter::FileContext ctx("test-op", "test.csv");
        bool ok = BarcodeValidator::checkAndRegister_CSV(
            generated,
            entityType,
            QUuid::createUuid(),
            name,
            ctx
            );

        Q_ASSERT(ok == true);
        Q_ASSERT(ctx.errorsSize() == 0);
    }

    zInfo("✓ testFallbackLogic OK");
}
