#ifndef IZIP_WRITER_H
#define IZIP_WRITER_H

#include <QString>
#include <QStringList>

class IZipWriter
{
public:
    virtual ~IZipWriter() = default;

    virtual bool createZip(const QString& zipPath,
                           const QStringList& files) = 0;
};

#endif // IZIP_WRITER_H
