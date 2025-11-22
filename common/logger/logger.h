#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QVariant>

#define L(msg) QStringLiteral(msg)
//#if defined(Q_OS_LINUX)
//#define getLocInfo LocInfo(static_cast<const char*>(__PRETTY_FUNCTION__),__FILE__,__LINE__)
//#elif defined(Q_OS_WIN)
#if defined(__GNUC__)
#define getLocInfo Logger::LocInfo(static_cast<const char*>(__PRETTY_FUNCTION__) ,__FILE__,__LINE__)
#elif defined(__MINGW32__)
#define getLocInfo LocInfo(__PRETTY_FUNCTION__ ,__FILE__,__LINE__)
#elif defined (_MSC_VER )
#define getLocInfo LocInfo(__FUNCSIG__ ,__FILE__,__LINE__)
#endif
//#endif

#define zError(...)   Logger::error2(__VA_ARGS__ __VA_OPT__(,) getLocInfo)
#define zWarning(...) Logger::warning2(__VA_ARGS__ __VA_OPT__(,) getLocInfo)
#define zInfo(...)    Logger::info2(__VA_ARGS__ __VA_OPT__(,) getLocInfo)

#define zDebug() Logger::debug2(getLocInfo)
#define zTrace() Logger::trace2(getLocInfo)

#define zMessage(msg) Logger::message((msg))

class Logger
{
public:
    class LocInfo
    {
    public:
        QString func;
        QString file;
        int line;

        LocInfo(const char *func, const char *file, int line);
        QString ToString() const;
    };

    enum ErrLevel:int {
        INFO,
        WARNING,
        ERROR_,
    };

    enum DbgLevel:int {
        NONE=0,
        TRACE,
        DEBUG,
    };

    // static void Init(ErrLevel level, DbgLevel dbglevel,
    //                  bool isBreak, bool isVerbose);
    static void setVerbose(bool v){ _isVerbose = v; }
    static void setBreakOnError(bool b){ _isBreakOnError = b; }

private:
    static ErrLevel _errlevel;
    static DbgLevel _dbglevel;
    static bool _isBreakOnError;
    static bool _isVerbose;
    //static bool _isInited;
    static std::function<void(const QString& str)> _func;

    static QString ToString(ErrLevel, const QString&, const QString&, const QString&);
    static QString ToString(DbgLevel level, const QString &msg, const QString &loci, const QString &st);
    static QString ToString(const ErrLevel &l);
    static QString ToString(const DbgLevel &l);
    static void err_message(ErrLevel level, const QString &msg);
    static void dbg_message(DbgLevel level, const QString& msg);
    static QString zStackTrace();

    static inline const QString DEFMSG = QStringLiteral("⚠️ [Logger] has not been initialized. Message skipped:");

public:
    static void error2(const QString& msg, const LocInfo& l);
    static void warning2(const QString& msg, const LocInfo& l);
    static void debug2(const LocInfo& l);
    static void trace2(const LocInfo& l);
    static void info2(const QString& msg, const LocInfo& l);
    static void info2(const QStringList& msg, const LocInfo& l);
    static void message(const QString& msg);

    static void SetFunction(std::function<void(const QString& str)> f){ _func = f;};
    static class LogStream warning2(const LocInfo &locinfo);
    static class LogStream error2(const LocInfo &locinfo);
    static class LogStream info2(const LocInfo &locinfo);
};

class LogStream {
    Logger::ErrLevel level;
    Logger::LocInfo loc;
    QString buffer;
public:
    LogStream(Logger::ErrLevel lvl, const Logger::LocInfo& l)
        : level(lvl), loc(l) {}

    template<typename T>
    LogStream& operator<<(const T& value) {
        buffer += QVariant(value).toString();
        return *this;
    }

    // 🔧 noquote() csak a szintaxis miatt van, semmit nem csinál
    LogStream& noquote() { return *this; }

    ~LogStream() {
        switch(level) {
        case Logger::ERROR_:
            Logger::error2(buffer, loc);
            break;
        case Logger::WARNING:
            Logger::warning2(buffer, loc);
            break;
        case Logger::INFO:
            Logger::info2(buffer, loc);
            break;
        }
    }
};


#endif // LOGGER_H
