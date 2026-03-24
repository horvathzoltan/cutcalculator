#include "barcode_generator_tester.h"

#include "barcodes/helpers/barcode_generator.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

#include <QSet>
#include <QString>

bool BarcodeGeneratorTester::run()
{
    zInfo("=== BarcodeGenerator TESTS START ===");

    testSlugBasic();
    testSlugUnicode();
    testTokenLength();
    testGeneratePrefixAndLength();
    testGenerateStability();
    testGenerateUniqueness();

    zInfo("=== BarcodeGenerator TESTS END ===");
    return true;
}

/* ============================================================
 * 🧪 1) slug basic – egyszerű ASCII név → slug
 * ============================================================ */
void BarcodeGeneratorTester::testSlugBasic()
{
    zInfo("→ testSlugBasic");

    QString slug = BarcodeGenerator::slugFromName("Simple Name");
    Q_ASSERT(slug == "simple-name");

    slug = BarcodeGenerator::slugFromName("Hello  World");
    Q_ASSERT(slug == "hello-world");

    slug = BarcodeGenerator::slugFromName("A");
    Q_ASSERT(slug == "a");

    slug = BarcodeGenerator::slugFromName("  Leading  and  trailing  ");
    Q_ASSERT(slug == "leading-and-trailing");

    zInfo("✓ testSlugBasic OK");
}


/* ============================================================
 * 🧪 2) slug unicode – ékezetek eltávolítása
 * ============================================================ */
void BarcodeGeneratorTester::testSlugUnicode()
{
    zInfo("→ testSlugUnicode");

    QString slug = BarcodeGenerator::slugFromName("Árvíztűrő tükörfúrógép");
    Q_ASSERT(slug == "arvizturo-tukorfurogep");

    slug = BarcodeGenerator::slugFromName("ŐűÚő");
    Q_ASSERT(slug == "ouuo");

    slug = BarcodeGenerator::slugFromName("ÉÉÉ   ÓÓ");
    Q_ASSERT(slug == "eee-oo");

    slug = BarcodeGenerator::slugFromName("  Üveg   Űrhajó  ");
    Q_ASSERT(slug == "uveg-urhajo");

    zInfo("✓ testSlugUnicode OK");
}


/* ============================================================
 * 🧪 3) token hossza – mindig 6 karakter
 * ============================================================ */
void BarcodeGeneratorTester::testTokenLength()
{
    zInfo("→ testTokenLength");

    for (int i = 0; i < 100; ++i) {
        QString t = BarcodeGenerator::generateToken();

        // hossz
        Q_ASSERT(t.length() == 6);

        // karakterkészlet: A–Z vagy 0–9
        for (QChar c : t) {
            bool ok = (c.isDigit() || (c >= 'A' && c <= 'Z'));
            Q_ASSERT(ok);
        }
    }

    zInfo("✓ testTokenLength OK");
}


/* ============================================================
 * 🧪 4) generate() – prefix + token + slug
 * ============================================================ */
void BarcodeGeneratorTester::testGeneratePrefixAndLength()
{
    zInfo("→ testGeneratePrefixAndLength");

    QString code = BarcodeGenerator::generate("MAT-", "Steel Bolt", 6);
    // formátum: MAT-XXXXXX-steelbolt
    Q_ASSERT(code.startsWith("MAT-"));

    // prefix után jön a token
    QString afterPrefix = code.mid(4);
    int dashPos = afterPrefix.indexOf('-');
    Q_ASSERT(dashPos > 0);

    QString token = afterPrefix.left(dashPos);
    QString slug  = afterPrefix.mid(dashPos + 1);

    Q_ASSERT(token.length() == 6);

    // slugFromName() eredménye
    QString expectedSlug = BarcodeGenerator::slugFromName("Steel Bolt");
    Q_ASSERT(slug == expectedSlug);

    zInfo("✓ testGeneratePrefixAndLength OK");
}


/* ============================================================
 * 🧪 5) generate() stabilitás – ugyanaz a név → ugyanaz a slug
 * ============================================================ */
void BarcodeGeneratorTester::testGenerateStability()
{
    zInfo("→ testGenerateStability");

    QString s1 = BarcodeGenerator::slugFromName("Stainless Steel");
    QString s2 = BarcodeGenerator::slugFromName("Stainless Steel");
    QString s3 = BarcodeGenerator::slugFromName("  Stainless   Steel  ");

    Q_ASSERT(s1 == s2);
    Q_ASSERT(s1 == s3);

    // slug stabil, token nem — ezért a generate() csak a slug részét ellenőrizzük
    QString code = BarcodeGenerator::generate("MAT-", "Stainless Steel");
    QString afterPrefix = code.mid(4);
    int dashPos = afterPrefix.indexOf('-');
    Q_ASSERT(dashPos > 0);

    QString slug = afterPrefix.mid(dashPos + 1);
    Q_ASSERT(slug == s1);

    zInfo("✓ testGenerateStability OK");
}

/* ============================================================
 * 🧪 6) generate() egyediség – 1000 token → mind különböző
 * ============================================================ */
void BarcodeGeneratorTester::testGenerateUniqueness()
{
    zInfo("→ testGenerateUniqueness");

    QSet<QString> tokens;
    tokens.reserve(2000);

    for (int i = 0; i < 2000; ++i) {
        QString t = BarcodeGenerator::generateToken(6);
        Q_ASSERT(!tokens.contains(t));
        tokens.insert(t);
    }

    // biztos ami biztos: tényleg 2000 egyedi elem
    Q_ASSERT(tokens.size() == 2000);

    zInfo("✓ testGenerateUniqueness OK");
}

