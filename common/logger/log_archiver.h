#ifndef LOG_ARCHIVER_H
#define LOG_ARCHIVER_H

#include <QString>
#include <QStringList>

class IZipWriter;

class LogArchiver
{
public:
    LogArchiver(const QString& logsDir,
                const QString& archiveDir,
                IZipWriter* zipWriter);

    bool run();

private:
    QString m_logsDir;
    QString m_archiveDir;
    IZipWriter* m_zipWriter;

    void log(const QString& msg) const;
};

#endif // LOG_ARCHIVER_H
