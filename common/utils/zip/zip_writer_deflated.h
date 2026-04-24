#ifndef ZIP_WRITER_DEFLATED_H
#define ZIP_WRITER_DEFLATED_H

#include <QIODevice>
#include <QString>
#include <QStringList>

#include "common/utils/zip/izip_writer.h"

class ZipWriterDeflated : public IZipWriter
{
public:
    ZipWriterDeflated() = default;
    ~ZipWriterDeflated() override = default;

    bool createZip(const QString& zipPath,
                   const QStringList& files) override;

private:
    struct FileEntry {
        QString fileName;
        quint32 crc32 = 0;
        quint32 compressedSize = 0;
        quint32 uncompressedSize = 0;
        quint32 localHeaderOffset = 0;
    };

    bool writeLocalFileHeader(QIODevice& out, const FileEntry& entry);
    bool writeCentralDirectory(QIODevice& out, const QList<FileEntry>& entries, quint32 cdStartOffset);
    bool writeEndOfCentralDirectory(QIODevice& out, quint32 cdStartOffset, quint32 cdSize, quint16 fileCount);
};

#endif // ZIP_WRITER_DEFLATED_H
