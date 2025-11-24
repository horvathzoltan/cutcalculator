#pragma once
#include <QVector>
#include <QString>
#include <QList>
#include <functional>
#include <optional>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include "common/utils/filehelper.h"
#include "common/logger/logger.h"
//#include "filecontext_collector.h"
#include "common/csv/filecontext.h"

namespace CsvImporter{

inline QList<QVector<QString>> read(const QString& filepath, QChar separator = QChar()) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString msg = L("❌ Nem sikerült megnyitni a csv fájlt: %1").arg(filepath);
        zWarning(msg);
        return {};
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // 🔍 Automatikus szeparátor detektálás, ha nincs megadva
    if (separator.isNull()) {
     //   zInfo("🔍 Automatikus szeparátor keresés...");
        separator = FileHelper::detectSeparatorSmart(&in);
        if (separator.isNull()) {
            zWarning(L("❌ Nem sikerült szeparátort detektálni a fájlban: %1").arg(filepath));
            return {};
        }
        file.seek(0); // 🔁 Vissza az elejére, újraolvasáshoz
        in.seek(0);
    }

    const auto rows = FileHelper::parseCSV(&in, separator);
    return rows;
}

template<typename T>
static QVector<T> readAndConvert(CsvImporter::FileContext& ctx,
                                 std::function<std::optional<T>(const QVector<QString>&, FileContext&)> converter,
                                 bool skipHeader = true)
{
    const auto rows = read(ctx.filepath());
    QVector<T> result;

    int readlines =0;
    ctx.setTotalLines(skipHeader?rows.size()-1:rows.size());
    for (int i = 0; i < rows.size(); ++i) {
        if (skipHeader && i == 0) continue;

        const auto& row = rows[i];

        ctx.setCurrentLineNumber(i + 1);

        auto maybeObj = converter(row, ctx);
        if (maybeObj.has_value()){
            result.append(std::move(maybeObj.value()));
            readlines++;
            ctx.setReadlines(readlines);
        }
    }

    return result;
}
}
