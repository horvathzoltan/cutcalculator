#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <functional>

// Egységes rövidítések
//#define zEvent(msg)  EventLogger::instance().zEvent_((msg))
#define zEventINFO(msg)  EventLogger::instance().zEvent_(EventLogger::Info,  (msg))
#define zEventWARN(msg)  EventLogger::instance().zEvent_(EventLogger::Warning, (msg))
#define zEventERROR(msg) EventLogger::instance().zEvent_(EventLogger::Error, (msg))

#define zEvent(...)   EventLogger::instance().zEvent_(__VA_ARGS__ __VA_OPT__ ())

class EventLogger {
public:
    enum Level{ Info,Warning,Error };
    static EventLogger& instance();

    static class EventStream zEvent_();


    void zEvent_(const QString& msg);
    void zEvent_(const QStringList& msg);
    void zEvent_(Level level, const QString& msg);

    // UI callback: pl. QListWidget vagy QPlainTextEdit frissítéséhez
    std::function<void(const QString&)> emitEvent = [](const QString&) {};

    //QStringList loadRecentEvents(int maxLines = 50);


    QStringList readSinceLastStart(int maxLines = 50);
private:
    EventLogger() = default;
    //QFile file;
    QString fileName;
    //bool writeToFile(const QString &line);
    QString timestamped(const QString &msg);
    //bool _isVerbose = false;

    static QString toString(Level level);   // <-- új helper
};

class EventStream {
    QString buffer;
public:
    EventStream(){}

    template<typename T>
    EventStream& operator<<(const T& value) {
        buffer += QVariant(value).toString();
        return *this;
    }

    // 🔧 noquote() csak a szintaxis miatt van, semmit nem csinál
    EventStream& noquote() { return *this; }

    ~EventStream() {
       EventLogger::instance().zEvent_(buffer);
    }
};
