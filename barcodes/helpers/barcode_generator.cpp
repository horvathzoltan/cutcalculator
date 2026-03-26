#include "barcode_generator.h"
#include <QRandomGenerator>
#include <QRegularExpression>

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
    QString norm = normalize(name);
    QStringList parts = norm.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    QStringList stems;                   // <-- ÚJ: ide gyűjtjük a szótöveket
    static QRegularExpression vowels("[AEIOU]");

    for (const QString& p : parts) {

        // Paraméter? (számot tartalmaz)
        if (p.contains(QRegularExpression("[0-9]"))) {
            QString param = p;
            param.remove(QRegularExpression("[^0-9A-Z]"));
            stems << param;              // <-- ide tesszük
            continue;
        }

        // Szó → első két magánhangzóig
        QString word = p;
        word.remove(QRegularExpression("[^A-Z]"));

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

    return stems.join("-");              // <-- ÚJ: szóhatár jelölése
}


/* ============================================================
 * 🧩 Teljes barcode generálás: prefix + token + "-" + slug
 * ============================================================ */
QString BarcodeGenerator::generate(const QString& prefix,
                                   const QString& name,
                                   int tokenLength)
{
    const QString token = makeToken(tokenLength);
    const QString slug  = slugFromName(name);
    return prefix + token + "-" + slug;
}

QString BarcodeGenerator::generateToken(int length)
{
    return makeToken(length);
}
