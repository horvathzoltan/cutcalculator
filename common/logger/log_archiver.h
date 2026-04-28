#ifndef LOG_ARCHIVER_H
#define LOG_ARCHIVER_H

#include <QString>
#include <QStringList>

/*
 * LogArchiver
 * -----------
 * A log-rotációért felelős komponens.
 *
 * Feladata:
 *   - a log könyvtár beolvasása
 *   - a fájlok bundle-ökbe rendezése timestamp alapján
 *   - a bundle-ök konzisztencia-ellenőrzése
 *   - archiválási döntés meghozatala
 *   - ZIP készítése a régi bundle-ökből
 *   - régi logfájlok törlése
 *
 * A működés determinisztikus, auditálható és hibabiztos.
 */

class IZipWriter;

/*
 * A LogArchiver egy önálló, egyszer használatos objektum,
 * amely egy rotációs ciklust futtat le.
 *
 * A rotáció nem állapotfüggő: minden futás teljesen újraépíti
 * a bundle-struktúrát és meghozza az archiválási döntést.
 */

class LogArchiver
{
public:
    /*
 * Konstruktor
 *
 * logsDir     - a logfájlok könyvtára
 * archiveDir  - a ZIP-ek célkönyvtára
 * zipWriter   - a ZIP készítéséért felelős implementáció
 *
 * A LogArchiver nem birtokolja a zipWriter-t, csak használja.
 */

    LogArchiver(const QString& logsDir,
                const QString& archiveDir,
                IZipWriter* zipWriter);

    /*
 * run()
 * -----
 * A teljes rotációs pipeline végrehajtása.
 *
 * Lépések:
 *   1) fájllista beolvasása
 *   2) bundle-építés
 *   3) bundle-konzisztencia ellenőrzés
 *   4) archiválási döntés
 *   5) ZIP készítése
 *   6) régi logfájlok törlése
 *
 * Siker esetén true, hiba esetén false.
 */

    bool run();

private:
    QString m_logsDir;         //a logfájlok könyvtára
    QString m_archiveDir;       //a ZIP-ek célkönyvtára
    IZipWriter* m_zipWriter;    //a ZIP készítéséhez használt writer

    /*
 * log()
 * -----
 * Egységes naplózási wrapper a LogArchiver számára.
 * Minden üzenet "[LogArchiver]" prefixet kap.
 */

    void log(const QString& msg) const;
};

#endif // LOG_ARCHIVER_H
