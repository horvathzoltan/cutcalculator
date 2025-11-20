#include "log_manager.h"

LogManager& LogManager::instance() {
    static LogManager inst;
    return inst;
}

LogManager::LogManager(QObject* parent)
    : QObject(parent)
{
    // nincs globális timer; csatornánként hozunk létre
}

void LogManager::ensureTimer(Channel ch) {
    if (timers.contains(ch) && timers[ch]) return;

    auto t = new QTimer(this);
    t->setInterval(2000);      // alapértelmezett: 2s csend után flush
    t->setSingleShot(true);
    connect(t, &QTimer::timeout, this, [this, ch]() { flush(ch); });
    timers.insert(ch, t);
}

void LogManager::setFile(Channel ch, const QString& path) {
    QMutexLocker locker(&mutex);
    fileNames[ch] = path;
    ensureTimer(ch);
}

void LogManager::setFlushInterval(Channel ch, int msec) {
    QMutexLocker locker(&mutex);
    ensureTimer(ch);
    timers[ch]->setInterval(msec);
}

QString LogManager::currentFile(Channel ch) const {
    QMutexLocker locker(&mutex);
    return fileNames.value(ch);
}

void LogManager::write(Channel ch, const QString& line) {
    QString tsLine = timestamped(line) + "\n";

    if (!useBuffering) {
        // pre‑Qt: azonnali flush, lock nélkül
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
        // post‑Qt: buffer + timer
        QMutexLocker locker(&mutex);
        ensureTimer(ch);
        buffers[ch] += tsLine;
        if (!timers[ch]->isActive())
            timers[ch]->start();
    }
}

void LogManager::enableBuffering(bool on) {
    QMutexLocker locker(&mutex);
    useBuffering = on;
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
    QMutexLocker locker(&mutex);
    for (auto it = buffers.begin(); it != buffers.end(); ++it) {
        Channel ch = it.key();
        const auto& buf = it.value();
        const auto& path = fileNames[ch];
        if (buf.isEmpty() || path.isEmpty()) {
            buffers[ch].clear();
            continue;
        }
        QFile f(path);
        if (f.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&f);
            out << buf;
        }
        buffers[ch].clear();
    }
    // időzítők megállítása (opcionális)
    for (auto tIt = timers.begin(); tIt != timers.end(); ++tIt) {
        if (tIt.value()) tIt.value()->stop();
    }
}

void LogManager::moveFile(Channel ch, const QString& newPath) {
    // először flush
    flush(ch);

    QString oldPath;
    {
        QMutexLocker locker(&mutex);
        oldPath = fileNames[ch];
        fileNames[ch] = newPath;
    }

    if (!oldPath.isEmpty() && oldPath != newPath) {
        QFile::rename(oldPath, newPath);
    }

    {
        QMutexLocker locker(&mutex);
        ensureTimer(ch);
        timers[ch]->stop();
    }
}

QString LogManager::timestamped(const QString& msg) const {
    return QString("[%1] %2")
    .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs))
        .arg(msg);
}
