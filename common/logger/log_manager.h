#pragma once

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QMap>

class LogManager : public QObject {
    Q_OBJECT
public:
    enum class Channel {
        Events,
        Errors,
        Audit,
        Performance
    };

    static LogManager& instance();

    // Konfiguráció
    void setFile(Channel ch, const QString& path);
    void setFlushInterval(Channel ch, int msec); // csatornánkénti időzítés
    QString currentFile(Channel ch) const;

    // Írás és karbantartás
    void write(Channel ch, const QString& line);
    void flush(Channel ch);
    void flushAll();
    void moveFile(Channel ch, const QString& newPath);

    void enableBuffering(bool on);

private:
    explicit LogManager(QObject* parent = nullptr);

    bool useBuffering = false; // alapból unbuffered
    // Állapot csatornánként
    QMap<Channel, QString> fileNames;
    QMap<Channel, QString> buffers;
    QMap<Channel, QTimer*> timers;

    mutable QMutex mutex;

    // Helper
    QString timestamped(const QString& msg) const;
    void ensureTimer(Channel ch);
};
