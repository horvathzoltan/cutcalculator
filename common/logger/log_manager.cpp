#include "log_manager.h"
#include "common/logger/logger.h"

LogManager& LogManager::instance() {
    static LogManager inst;
    return inst;
}

LogManager::LogManager(QObject* parent)
    : QObject(parent)
{}

void LogManager::initChannels(const QString& folderPath, const QSet<Channel>& channels) {
    QMutexLocker locker(&mutex);
    folder = folderPath;
    activeChannels = channels;

    for (Channel ch : channels) {
        fileNames[ch] = folder + "/" + generateFileName(ch);
    }
}

void LogManager::moveToFolder(const QString& newFolder) {
    flushAll();

    QMutexLocker locker(&mutex);
    folder = newFolder;
    for (Channel ch : activeChannels) {
        const auto fname = generateFileName(ch);
        QString oldFile = fileNames[ch];
        QString newFile = folder + "/" + fname;
        fileNames[ch] = newFile;
        if (!oldFile.isEmpty() && oldFile != newFile) {
            QFile::rename(oldFile, newFile);
        }
        ensureTimer(ch);
        timers[ch]->stop();
    }
}

void LogManager::enableBuffering(bool on) {
    QMutexLocker locker(&mutex);
    useBuffering = on;
}

void LogManager::setFlushInterval(Channel ch, int msec) {
    QMutexLocker locker(&mutex);
    ensureTimer(ch);
    timers[ch]->setInterval(msec);
}

void LogManager::write(Channel ch, const QString& line) {
    if (!activeChannels.contains(ch)) return;

    const QString tsLine = timestamped(line) + "\n";

    if (!useBuffering) {
        QString path;
        {
            QMutexLocker locker(&mutex);
            path = fileNames[ch];
        }
        if (!path.isEmpty()) {
            QFile f(path);
            if (f.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&f);
                out << tsLine;
            }
        }
    } else {
        QMutexLocker locker(&mutex);
        ensureTimer(ch);
        buffers[ch] += tsLine;
        timers[ch]->start();
    }
}

void LogManager::writeAll(const QString& line) {
    for (Channel ch : activeChannels) {
        write(ch, line);
    }
}

void LogManager::flush(Channel ch) {
    QString buf, path;
    {
        QMutexLocker locker(&mutex);
        buf = buffers[ch];
        path = fileNames[ch];
        buffers[ch].clear();
    }
    if (buf.isEmpty() || path.isEmpty()) return;

    QFile f(path);
    if (f.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&f);
        out << buf;
    }
}

void LogManager::flushAll() {
    QMap<Channel, QString> toWrite;
    QMap<Channel, QString> paths;
    {
        QMutexLocker locker(&mutex);
        for (Channel ch : activeChannels) {
            const auto& buf = buffers[ch];
            const auto& path = fileNames[ch];
            if (!buf.isEmpty() && !path.isEmpty()) {
                toWrite.insert(ch, buf);
                paths.insert(ch, path);
            }
            buffers[ch].clear();
        }
        for (auto tIt = timers.begin(); tIt != timers.end(); ++tIt) {
            if (tIt.value()) tIt.value()->stop();
        }
    }
    for (auto it = toWrite.begin(); it != toWrite.end(); ++it) {
        QFile f(paths[it.key()]);
        if (f.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&f);
            out << it.value();
        }
    }
}

QString LogManager::currentFile(Channel ch) const {
    QMutexLocker locker(&mutex);
    return fileNames.value(ch);
}

void LogManager::ensureTimer(Channel ch) {
    if (timers.contains(ch) && timers[ch]) return;
    auto t = new QTimer(this);
    t->setInterval(2000);
    t->setSingleShot(true);
    connect(t, &QTimer::timeout, this, [this, ch]() { flush(ch); });
    timers.insert(ch, t);
}

QString LogManager::timestamped(const QString& msg) const {
    return QString("[%1] %2")
    .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs))
        .arg(msg);
}

QString LogManager::generateFileName(Channel ch) const {
    const auto prefix = channelPrefix(ch);
    const auto ts = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    return QString("%1_%2.txt").arg(prefix, ts);
}

QString LogManager::channelPrefix(Channel ch) {
    switch (ch) {
    case Channel::Events:      return "events";
    case Channel::Errors:      return "log";
    case Channel::Audit:       return "audit";
    case Channel::Performance: return "perf";
    }
    return "log"; // fallback
}

#include <QTextStream>

QStringList LogManager::readChannel(Channel ch, int maxLines) {
    flush(ch);

    QString path = currentFile(ch);
    QStringList lines;
    if (path.isEmpty()) return lines;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        zWarning() << "Nem sikerült megnyitni a log fájlt:" << path;
        return lines;
    }

    QTextStream in(&f);
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    f.close();

    if (maxLines > 0 && lines.size() > maxLines) {
        return lines.mid(lines.size() - maxLines);
    }
    return lines;
}

QStringList LogManager::readChannelSinceLastStart(Channel ch, int maxLines) {
    QStringList all = readChannel(ch, 0);
    std::reverse(all.begin(), all.end());

    QStringList recent;
    for (const QString& line : all) {
        //if (line.contains("🟢 START")) break;
        recent << line;
        if (maxLines > 0 && recent.size() >= maxLines) break;
    }
    std::reverse(recent.begin(), recent.end());
    return recent;
}
