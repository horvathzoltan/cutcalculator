#include "calculation/repository/need_calculation_detail_repository.h"
#include "common/csv/csvimporter.h"
#include "common/csv/filecontext.h"
#include "common/csv/filecontext_collector.h"
#include "common/logger/event_logger.h"
#include <QFile>
#include <QTextStream>

static QString ncd_path() {
    return "testdata/needcalc_details.csv";
}

bool NeedCalculationDetailRepository::load(QVector<NeedCalculationDetail>& out) {
    CsvImporter::FileContext ctx("NeedCalculationDetail import", ncd_path());
    QVector<NeedCalculationDetail> tmp;

    const auto rows = CsvImporter::read(ncd_path(), ';');
    if (rows.isEmpty()) {
        ctx.setFileError("❌ NeedCalculationDetail CSV üres vagy nem olvasható");
        FileContextCollector::instance().onContextDestroyed(ctx);
        return false;
    }

    for (int i = 1; i < rows.size(); ++i) {
        const auto& r = rows[i];
        const auto& f = r.fields;
        if (f.size() < 3) {
            ctx.addError(r.rawLineNumber, "⚠️ NeedCalculationDetail: kevés mező (3 kell)");
            continue;
        }
        NeedCalculationDetail d;
        d.id = QUuid::createUuid();
        d.needCalculationId = QUuid(f[0].trimmed());
        d.materialId = QUuid(f[1].trimmed());
        d.formula = f[2].trimmed();
        tmp.append(d);
    }

    out = std::move(tmp);
    FileContextCollector::instance().onContextDestroyed(ctx);
    zInfo(QString("✅ NeedCalculationDetail loaded: %1 sor").arg(out.size()));
    return true;
}

bool NeedCalculationDetailRepository::save(const QVector<NeedCalculationDetail>& data) {
    QFile file(ncd_path());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        zWarning("⚠️ NeedCalculationDetail save: nem nyitható meg írásra");
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "needCalculationId;materialId;formula\n";
    for (const auto& d : data) {
        out << d.needCalculationId.toString(QUuid::WithoutBraces) << ";"
            << d.materialId.toString(QUuid::WithoutBraces) << ";"
            << d.formula << "\n";
    }
    zInfo(QString("💾 NeedCalculationDetail saved: %1 sor").arg(data.size()));
    return true;
}
