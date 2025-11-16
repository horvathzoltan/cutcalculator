#include "project_root_locator.h"
#include <QDebug>

bool ProjectRootLocator::_verbose = false;

ProjectRootLocator::Outcome ProjectRootLocator::locateFrom(const QString& sourceFilePath) {
    Outcome outcome;
    QDir dir = QFileInfo(sourceFilePath).absoluteDir();

    while (dir.cdUp()) {
        QString root = dir.absolutePath();
        QStringList markers = { ".git", ".qtcreator", "main.cpp" };
        QStringList found, missing;

        for (const QString& marker : markers) {
            if (QFileInfo::exists(dir.filePath(marker))) {
                found << marker;
            } else {
                missing << marker;
            }
        }

        // Külön kezeljük a *.pro fájlt
        bool hasProFile = !dir.entryList(QStringList("*.pro"), QDir::Files).isEmpty();
        if (hasProFile) {
            found << "*.pro";
        } else {
            missing << "*.pro";
        }

        if (found.size() >= 2) { // legalább két marker megvan
            outcome.rootPath = root;
            outcome.testdataPath = dir.filePath("testdata");
            outcome.markersFound = found;
            outcome.markersMissing = missing;

            if (QFileInfo::exists(outcome.testdataPath)) {
                outcome.result = FoundWithTestdata;
                if(_verbose){
                    qDebug() << "✅ Projektgyökér megtalálva[locateFrom]:" << root << "→ testdata is megvan.";
                }
            } else {
                outcome.result = FoundWithoutTestdata;
                if(_verbose){
                qWarning() << "⚠️ Projektgyökér megtalálva[locateFrom]:" << root << "→ de nincs testdata.";
                }
            }
            return outcome;
        }
    }

    outcome.result = NotFound;
    if(_verbose){
        qWarning() << "❌ Nem találtunk projektgyökeret marker alapján.";
    }
    return outcome;
}
