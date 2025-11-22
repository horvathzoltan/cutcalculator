/**
 * @file log_manager.h
 * @brief Többcsatornás, audit-barát log menedzser Qt alapokon.
 *
 * A LogManager singleton osztály különböző log csatornákat (Events, Errors, Audit, Performance)
 * kezel. Csatornánként külön fájlba ír, de globálisan egyetlen mappát állíthatsz be.
 * Csak azokat a csatornákat indítja el, amelyeket az initChannels() hívásban megadsz.
 *
 * Két üzemmódot támogat:
 * - Pre-Qt (QCoreApplication előtt): minden write() azonnal flushol.
 * - Post-Qt (QApplication után): bufferelt írás, csatornánkénti időzítővel.
 *
 * Használat:
 * @code
 * LogManager::instance().initChannels(binDir, {LogManager::Channel::Events, LogManager::Channel::Errors});
 * LogManager::instance().writeAll("🟢 START application");
 * ...
 * QApplication app(argc, argv);
 * LogManager::instance().enableBuffering(true);
 * @endcode
 */

#pragma once

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QMap>
#include <QSet>
#include <QFileInfo>

class LogManager : public QObject {
    Q_OBJECT
public:
    /// Log csatornák
    enum class Channel {
        Events,
        Errors,
        Audit,
        Performance
    };

    /// Singleton példány
    static LogManager& instance();

    /**
     * @brief Inicializálja az aktív csatornákat és beállítja a globális mappát.
     * @param folder A mappa, ahova a log fájlok kerülnek.
     * @param channels Az aktív csatornák halmaza.
     */
    void initChannels(const QString& folder, const QSet<Channel>& channels);

    /**
     * @brief Áthelyezi az összes aktív csatorna log fájlját új mappába.
     * @param newFolder Az új mappa.
     */
    void moveToFolder(const QString& newFolder);

    /**
     * @brief Bufferelés engedélyezése (QApplication után).
     * @param on true esetén bufferelt írás, false esetén azonnali flush.
     */
    void enableBuffering(bool on);

    /**
     * @brief Csatornánkénti flush intervallum beállítása.
     * @param ch Csatorna.
     * @param msec Időzítés milliszekundumban.
     */
    void setFlushInterval(Channel ch, int msec);

    /**
     * @brief Egy sor írása egy adott csatornába.
     * @param ch Csatorna.
     * @param line A log sor.
     */
    void write(Channel ch, const QString& line);

    /**
     * @brief Egy sor írása minden aktív csatornába.
     * @param line A log sor.
     */
    void writeAll(const QString& line);

    /**
     * @brief Buffer flusholása egy adott csatornában.
     * @param ch Csatorna.
     */
    void flush(Channel ch);

    /// Minden buffer flusholása.
    void flushAll();

    /// Aktuális fájl elérési útja.
    QString currentFile(Channel ch) const;

    /**
 * @brief Csatorna flush és teljes beolvasás.
 * @param ch Csatorna.
 * @param maxLines Visszaadott sorok maximuma (0 = korlátlan).
 */
    QStringList readChannel(Channel ch, int maxLines = 0);

    /**
 * @brief Csatorna flush és beolvasás csak az utolsó START óta.
 * @param ch Csatorna.
 * @param maxLines Visszaadott sorok maximuma (0 = korlátlan).
 */
    QStringList readChannelSinceLastStart(Channel ch, int maxLines = 0);

private:
    explicit LogManager(QObject* parent = nullptr);

    QSet<Channel> activeChannels;
    QMap<Channel, QString> fileNames;
    QMap<Channel, QString> buffers;
    QMap<Channel, QTimer*> timers;

    QString folder;
    bool useBuffering = false;
    mutable QMutex mutex;

    void ensureTimer(Channel ch);
    QString timestamped(const QString& msg) const;
    QString generateFileName(Channel ch) const;
    static QString channelPrefix(Channel ch);
};
