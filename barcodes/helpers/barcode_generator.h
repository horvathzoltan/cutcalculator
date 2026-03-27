#pragma once

#include <QString>

/* ============================================================
 * 🧩 BarcodeGenerator – prefix + token + slug
 * ============================================================ */
class BarcodeGenerator {
public:
    // prefix: pl. "PROD-"
    // name: entitás neve (slug alapja)
    // tokenLength: Base36 token hossza (pl. 6)
    static QString generate(const QString& prefix,
                            const QString& name,
                            int tokenLength = 6);

    // slug: kvázi-szótagolás – második magánhangzó előtt vágunk
    static QString slugFromName(const QString& name);
    static QString generateToken(int length = 6);

private:
    static QString normalize(const QString& s);
    static QString normalizeForSlug(const QString& s);
    static QString makeToken(int length = 6);
};
