#include "needs/repository/need_rule_repository.h"

bool NeedRuleRepository::load(QVector<NeedRule>& out)
{
    const QString path = FileNameHelper::instance().getNeedRuleCsvFile();
    CsvImporter::FileContext ctx("NeedRule import", path);

    // 1) CSV beolvasás
    const auto rows = CsvImporter::read(path, ',');
    if (rows.isEmpty()) {
        zWarning("⚠️ NeedRuleRepository: üres CSV");
        return false;
    }

    QVector<RawRow> raw;
    raw.reserve(rows.size());

    // 2) Header kihagyása
    for (int i = 1; i < rows.size(); ++i) {
        const auto& r = rows[i];
        const auto& f = r.fields;

        if (f.size() < 2) {
            ctx.addError(r.rawLineNumber, "⚠️ NeedRule: kevés mező (2 kell)");
            continue;
        }

        RawRow rr;
        rr.productCode  = f[0].trimmed();
        rr.materialCode = f[1].trimmed();
        rr.lineNumber   = r.rawLineNumber;
        raw.append(rr);
    }

    // 3) RawRow → NeedRule (lookup ProductRegistry + MaterialRegistry)
    QVector<NeedRule> rules;
    rules.reserve(raw.size());

    for (const auto& rr : raw) {
        // Product lookup
        const auto* prod = ProductRegistry::instance().findIf([&](const ProductMaster& p){
            return p.barcode.compare(rr.productCode, Qt::CaseInsensitive) == 0;
        });

        if (!prod) {
            ctx.addError(rr.lineNumber,
                         QString("⚠️ NeedRule: ismeretlen productId: %1")
                             .arg(rr.productCode));
            continue;
        }

        // Material lookup
        const auto* mat = MaterialRegistry::instance().findIf([&](const MaterialMaster& m){
            return m.barcode.compare(rr.materialCode, Qt::CaseInsensitive) == 0;
        });

        if (!mat) {
            ctx.addError(rr.lineNumber,
                         QString("⚠️ NeedRule: ismeretlen materialId: %1")
                             .arg(rr.materialCode));
            continue;
        }

        NeedRule rule;
        rule.leftId  = prod->id;
        rule.rightId = mat->id;

        rules.append(rule);
    }

    out = rules;

    zInfo(QString("📦 NeedRuleRepository: %1 kapcsolat beolvasva")
              .arg(rules.size()));

    return !rules.isEmpty();
}

bool NeedRuleRepository::save()
{
    const QString path = FileNameHelper::instance().getNeedRuleCsvFile();
    QFile f(path);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        FileHelper::logFileError(f, "NeedRule SAVE", QIODevice::WriteOnly);
        return false;
    }

    QTextStream ts(&f);
    ts.setEncoding(QStringConverter::Utf8);

    ts << "productId;materialId\n";

    const auto& all = NeedRuleRegistry::instance().readAll();
    for (const auto& r : all) {
        const auto* prod = ProductRegistry::instance().findById(r.leftId);
        const auto* mat  = MaterialRegistry::instance().findById(r.rightId);

        ts << (prod ? prod->barcode : "")
           << ";"
           << (mat ? mat->barcode : "")
           << "\n";
    }

    zInfo(QString("💾 NeedRuleRepository: CSV export kész → %1").arg(path));
    return true;
}
