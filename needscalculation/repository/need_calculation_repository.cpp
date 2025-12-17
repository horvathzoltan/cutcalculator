#include "needscalculation/repository/need_calculation_repository.h"
#include "common/csv/csvimporter.h"
#include "common/csv/filecontext.h"
#include "common/csv/filecontext_collector.h"
#include "common/logger/event_logger.h"
#include <QFile>
#include <QTextStream>

static QString nc_path() {
    // Hunglish: később SettingsManager.datapath + traits
    return "testdata/needcalcs.csv";
}

bool NeedCalculationRepository::load(QVector<NeedCalculation>& out) {
    CsvImporter::FileContext ctx("NeedCalculation import", nc_path());
    QVector<NeedCalculation> tmp;

    const auto rows = CsvImporter::read(nc_path(), ';');
    if (rows.isEmpty()) {
        ctx.setFileError("❌ NeedCalculation CSV üres vagy nem olvasható");
        FileContextCollector::instance().onContextDestroyed(ctx);
        return false;
    }

    // Header skip
    for (int i = 1; i < rows.size(); ++i) {
        const auto& r = rows[i];
        const auto& f = r.fields;
        if (f.size() < 2) {
            ctx.addError(r.rawLineNumber, "⚠️ NeedCalculation: túl kevés mező (2 kell)");
            continue;
        }
        NeedCalculation c;
        c.id = QUuid::createUuid();
        c.productDefinitionId = QUuid(f[0].trimmed());
        c.modeName = f[1].trimmed();
        tmp.append(c);
    }

    out = std::move(tmp);
    FileContextCollector::instance().onContextDestroyed(ctx);
    zEventINFO(QString("✅ NeedCalculation loaded: %1 sor").arg(out.size()));
    return true;
}

bool NeedCalculationRepository::save(const QVector<NeedCalculation>& data) {
    QFile file(nc_path());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        zEventERROR("❌ NeedCalculation save: nem nyitható meg írásra");
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "productDefinitionId;modeName\n";
    for (const auto& c : data) {
        out << c.productDefinitionId.toString(QUuid::WithoutBraces) << ";"
            << c.modeName << "\n";
    }
    zEventINFO(QString("💾 NeedCalculation saved: %1 sor").arg(data.size()));
    return true;
}
