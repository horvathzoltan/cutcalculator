#pragma once
#include <QString>
#include <QDir>
#include <QFileInfo>

class ProjectRootLocator {
public:
    enum Result {
        FoundWithTestdata,
        FoundWithoutTestdata,
        NotFound
    };

    struct Outcome {
        Result result;
        QString rootPath;
        QString testdataPath;
        QStringList markersFound;
        QStringList markersMissing;

        QString diagnosticMessage() const {
            switch (result) {
            case FoundWithTestdata:
                return QString("✅ Projektgyökér megtalálva: %1 → testdata is megvan.")
                    .arg(rootPath);

            case FoundWithoutTestdata: {
                QString msg = QString("⚠️ Projektgyökér megtalálva: %1 → de hiányzik a testdata.")
                                  .arg(rootPath);
                if (!markersMissing.isEmpty()) {
                    msg += QString("\nℹ️ Hiányzó markerek (nem kritikus): %1")
                               .arg(markersMissing.join(", "));
                }
                return msg;
            }

            case NotFound:
                return QString("❌ Nem találtunk projektgyökeret marker alapján. "
                               "Ellenőrizd, hogy van-e .git vagy .pro fájl a gyökérben.");

            default:
                return QString("❌ Ismeretlen diagnosztikai állapot.");
            }
        }
    };

    static Outcome locateFrom(const QString& sourceFilePath);

    static  void setVerbose(bool v){_verbose = v;}
private:
    static bool _verbose;
};
