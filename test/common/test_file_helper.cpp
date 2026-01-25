#include "test_file_helper.h"
#include <QFile>

QStringList readAllLines(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QStringList lines;
    while (!f.atEnd())
        lines << QString::fromUtf8(f.readLine()).trimmed();

    return lines;
}
