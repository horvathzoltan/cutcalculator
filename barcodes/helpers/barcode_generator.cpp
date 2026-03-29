#include "barcode_generator.h"
#include <QRandomGenerator>
#include <QRegularExpression>
#include <barcodes/registry/barcode_registry.h>

/* ============================================================
 * 🧩 Normalizálás – ékezetek eltávolítása, nagybetű, ×→X
 * ============================================================ */
QString BarcodeGenerator::normalize(const QString& s)
{
    QString out = s.normalized(QString::NormalizationForm_D);
    out.remove(QRegularExpression("\\p{Mn}"));   // minden nonspacing mark (ékezet) le
    out = out.toUpper();

    out.replace(QRegularExpression("[×xX*/]"), "X");
    return out;
}

QString BarcodeGenerator::normalizeForSlug(const QString& s)
{
    // 1) NFD normalizáció (ékezetek szétválasztása)
    QString out = s.normalized(QString::NormalizationForm_D);

    // 2) Combining mark-ok eltávolítása (ékezetek törlése)
    out.remove(QRegularExpression("\\p{Mn}"));

    // 3) Unicode whitespace → sima space
    out.replace(QRegularExpression("\\s+"), " ");

    // 4) Speciális karakterek → kötőjel
    out.replace(QRegularExpression("[^A-Za-z0-9 ]"), "-");

    // 5) Lowercase (slug mindig kisbetűs)
    out = out.toLower();

    // 6) NFC visszaalakítás (kompakt forma)
    out = out.normalized(QString::NormalizationForm_C);

    return out;
}

/* ============================================================
 * 🧩 Base36 token generálás
 * ============================================================ */
QString BarcodeGenerator::makeToken(int length)
{
    static const QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString t;
    t.reserve(length);

    for (int i = 0; i < length; ++i) {
        int idx = QRandomGenerator::global()->bounded(alphabet.size());
        t.append(alphabet[idx]);
    }
    return t;
}

/* ============================================================
 * 🧩 Slug – második magánhangzó előtt vágunk
 * ============================================================ */
QString BarcodeGenerator::slugFromName(const QString& name)
{
    // 1) Unicode‑tudatos normalizáció
    const QString norm = normalizeForSlug(name);

    // 2) Szavakra bontás
    const QStringList parts = norm.split(' ', Qt::SkipEmptyParts);

    QStringList stems;
    stems.reserve(parts.size());

    // Magánhangzók több nyelvhez (latin alapú)
    static const QRegularExpression vowels("[aeiouy]");

    for (const QString& p : parts) {

        // 3) Paraméter (számot tartalmaz)
        if (p.contains(QRegularExpression("[0-9]"))) {
            QString param = p;
            param.remove(QRegularExpression("[^0-9a-z]"));
            stems << param;
            continue;
        }

        // 4) Rövidítések (pl. MDF, PVC, ABS)
        if (p.length() <= 4 && p.contains(QRegularExpression("^[a-z]+$"))) {
            stems << p;
            continue;
        }

        // 5) Szótő képzése: első két magánhangzóig
        QString word = p;
        word.remove(QRegularExpression("[^a-z]"));

        int first = word.indexOf(vowels);
        if (first < 0) {
            stems << word;
            continue;
        }

        int second = word.indexOf(vowels, first + 1);
        if (second < 0) {
            stems << word.left(first + 1);
        } else {
            stems << word.left(second);
        }
    }

    // 6) Kötőjelek összevonása
    QString slug = stems.join("-");
    slug.replace(QRegularExpression("-+"), "-");

    // 7) Trim
    if (slug.startsWith('-')) slug.remove(0, 1);
    if (slug.endsWith('-')) slug.chop(1);

    return slug;
}



/* ============================================================
 * 🧩 Teljes barcode generálás: prefix + token + "-" + slug
 * ============================================================ */
QString BarcodeGenerator::generate(const QString& prefix,
                                   const QString& name,
                                   int tokenLength)
{
    // 1) Prefix normalizálása
    QString fixedPrefix = prefix;
    if (!fixedPrefix.endsWith(QLatin1Char('-')))
        fixedPrefix.append(QLatin1Char('-'));

    // 2) Slug generálása
    const QString slug = slugFromName(name);

    // 3) Token generálása
    const QString token = makeToken(tokenLength);

    // 4) Alap jelölt kód
    const QString base = fixedPrefix + token + QLatin1Char('-') + slug;

    // 5) Prefix alatti kódok lekérése
    const QStringList existing =
        BarcodeRegistry::instance().barcodesWithPrefix(fixedPrefix);

    // 6) Ha nincs ütközés → kész
    if (!existing.contains(base))
        return base;

    // 7) Numerikus postfix fallback
    int maxPostfix = 0;
    const int baseLen = base.length();

    for (const QString& code : existing) {

        // Pontos egyezés → postfix = 0
        if (code == base) {
            maxPostfix = std::max(maxPostfix, 0);
            continue;
        }

        // Ha nem base + '-' kezdettel indul → nem releváns
        if (!code.startsWith(base + QLatin1Char('-')))
            continue;

        // A postfix rész kinyerése
        const QStringView tail = QStringView{code}.mid(baseLen + 1);

        bool ok = false;
        const int n = tail.toInt(&ok);
        if (ok)
            maxPostfix = std::max(maxPostfix, n);
    }

    // 8) Következő postfix
    return base + QLatin1Char('-') + QString::number(maxPostfix + 1);
}



QString BarcodeGenerator::generateToken(int length)
{
    return makeToken(length);
}

QString BarcodeGenerator::ensureUnique(const QString& base)
{
    // 1) Ha nincs ilyen barcode → ez lesz az új
    if (BarcodeRegistry::instance().isBarcodeUnique(base))
        return base;

    // 2) Keresünk postfixeket
    int maxPostfix = 0;
    const QStringList existing =
        BarcodeRegistry::instance().barcodesWithPrefix(base);

    for (const QString& code : existing) {
        if (!code.startsWith(base + "-"))
            continue;

        bool ok = false;
        int n = code.mid(base.length() + 1).toInt(&ok);
        if (ok)
            maxPostfix = std::max(maxPostfix, n);
    }

    // 3) Következő postfix
    return base + "-" + QString::number(maxPostfix + 1);
}
