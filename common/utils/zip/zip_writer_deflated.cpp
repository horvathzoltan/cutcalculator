#include "zip_writer_deflated.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>

#include <zlib.h>

static quint32 computeCrc32(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return 0;

    quint32 crc = crc32(0L, Z_NULL, 0);
    QByteArray buf;
    buf.resize(64 * 1024);

    while (true) {
        qint64 n = f.read(buf.data(), buf.size());
        if (n <= 0)
            break;
        crc = crc32(crc, reinterpret_cast<const Bytef*>(buf.constData()), n);
    }

    return crc;
}

// raw DEFLATE (ZIP-nek ez kell, nem zlib-headeres stream)
static bool deflateRaw(const QByteArray& input, QByteArray& output, uLongf& outSize)
{
    z_stream strm;
    memset(&strm, 0, sizeof(strm));

    int res = deflateInit2(&strm,
                           Z_BEST_COMPRESSION,
                           Z_DEFLATED,
                           -MAX_WBITS,      // <--- raw DEFLATE
                           8,
                           Z_DEFAULT_STRATEGY);
    if (res != Z_OK)
        return false;

    output.resize(compressBound(input.size()));
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.constData()));
    strm.avail_in = input.size();
    strm.next_out = reinterpret_cast<Bytef*>(output.data());
    strm.avail_out = output.size();

    res = deflate(&strm, Z_FINISH);
    if (res != Z_STREAM_END) {
        deflateEnd(&strm);
        return false;
    }

    outSize = strm.total_out;
    deflateEnd(&strm);

    output.resize(outSize);
    return true;
}

bool ZipWriterDeflated::createZip(const QString& zipPath,
                                  const QStringList& files)
{
    QFile out(zipPath);
    if (!out.open(QIODevice::WriteOnly))
        return false;

    QList<FileEntry> entries;
    entries.reserve(files.size());

    quint32 offset = 0;

    for (const QString& path : files) {
        QFileInfo info(path);
        if (!info.exists())
            continue;

        QFile in(path);
        if (!in.open(QIODevice::ReadOnly))
            return false;

        QByteArray input = in.readAll();

        FileEntry entry;
        entry.fileName = info.fileName();
        entry.uncompressedSize = input.size();
        entry.crc32 = computeCrc32(path);
        entry.localHeaderOffset = offset;
        entry.compressedSize = 0; // majd töltjük

        // --- Local File Header helye ---
        const qint64 headerPos = out.pos();

        // ideiglenes header (méretek 0-val, később felülírjuk)
        if (!writeLocalFileHeader(out, entry))
            return false;

        // --- tömörítés raw DEFLATE-tel ---
        QByteArray compressed;
        uLongf destLen = 0;
        if (!deflateRaw(input, compressed, destLen))
            return false;

        entry.compressedSize = static_cast<quint32>(destLen);

        if (out.write(compressed) != static_cast<qint64>(destLen))
            return false;

        // --- Local Header frissítése valós CRC/size értékekkel ---
        {
            QDataStream s(&out);
            s.setByteOrder(QDataStream::LittleEndian);

            // signature(4) + version(2) + flags(2) + method(2) + time(2) + date(2) = 14 byte
            const qint64 sizesPos = headerPos + 14;
            const qint64 curPos = out.pos();

            if (!out.seek(sizesPos))
                return false;

            s << quint32(entry.crc32);
            s << quint32(entry.compressedSize);
            s << quint32(entry.uncompressedSize);

            if (!out.seek(curPos))
                return false;
        }

        // offset a következő fájl Local Headeréhez
        offset = static_cast<quint32>(out.pos());

        entries.append(entry);
    }

    const quint32 cdStart = offset;

    if (!writeCentralDirectory(out, entries, cdStart))
        return false;

    const quint32 cdEnd = static_cast<quint32>(out.pos());
    const quint32 cdSize = cdEnd - cdStart;

    if (!writeEndOfCentralDirectory(out, cdStart, cdSize, static_cast<quint16>(entries.size())))
        return false;

    return true;
}

bool ZipWriterDeflated::writeLocalFileHeader(QIODevice& out, const FileEntry& entry)
{
    QByteArray name = entry.fileName.toUtf8();
    QDataStream s(&out);
    s.setByteOrder(QDataStream::LittleEndian);

    s << quint32(0x04034b50);   // local file header signature
    s << quint16(20);           // version needed
    s << quint16(0);            // flags
    s << quint16(8);            // compression = DEFLATE
    s << quint16(0);            // mod time
    s << quint16(0);            // mod date
    s << quint32(0);            // crc32 (később felülírjuk)
    s << quint32(0);            // compressed size (később felülírjuk)
    s << quint32(0);            // uncompressed size (később felülírjuk)
    s << quint16(name.size());
    s << quint16(0);            // extra field length

    if (out.write(name) != name.size())
        return false;

    return true;
}

bool ZipWriterDeflated::writeCentralDirectory(QIODevice& out,
                                              const QList<FileEntry>& entries,
                                              quint32 cdStartOffset)
{
    Q_UNUSED(cdStartOffset);

    QDataStream s(&out);
    s.setByteOrder(QDataStream::LittleEndian);

    for (const FileEntry& e : entries) {
        QByteArray name = e.fileName.toUtf8();

        s << quint32(0x02014b50);      // central file header signature
        s << quint16(20);              // version made by
        s << quint16(20);              // version needed
        s << quint16(0);               // flags
        s << quint16(8);               // compression = DEFLATE
        s << quint16(0);               // mod time
        s << quint16(0);               // mod date
        s << quint32(e.crc32);
        s << quint32(e.compressedSize);
        s << quint32(e.uncompressedSize);
        s << quint16(name.size());
        s << quint16(0);               // extra field length
        s << quint16(0);               // file comment length
        s << quint16(0);               // disk number start
        s << quint16(0);               // internal attrs
        s << quint32(0);               // external attrs
        s << quint32(e.localHeaderOffset);

        if (out.write(name) != name.size())
            return false;
    }

    return true;
}

bool ZipWriterDeflated::writeEndOfCentralDirectory(QIODevice& out,
                                                   quint32 cdStartOffset,
                                                   quint32 cdSize,
                                                   quint16 fileCount)
{
    QDataStream s(&out);
    s.setByteOrder(QDataStream::LittleEndian);

    s << quint32(0x06054b50);      // EOCD signature
    s << quint16(0);               // disk number
    s << quint16(0);               // disk with CD
    s << quint16(fileCount);       // entries on this disk
    s << quint16(fileCount);       // total entries
    s << quint32(cdSize);          // size of CD
    s << quint32(cdStartOffset);   // offset of CD
    s << quint16(0);               // comment length

    return true;
}
