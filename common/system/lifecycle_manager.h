/**
 * @file lifecycle_manager.h
 * @brief Automatikus Qt-alkalmazás életciklus követés és audit-barát logolás.
 *
 * A LifecycleManager a LogManagerre épülő magas szintű réteg, ami automatikusan
 * naplózza a fő fázisokat:
 *  - Startup (main eleje; explicit init)
 *  - Application (QApplication létrejött; attach után)
 *  - MainWindowOpen (első top-level QMainWindow tényleges megjelenése)
 *  - MainWindowClose (az ablak bezárása)
 *  - Shutdown (aboutToQuit)
 *  - Abort (signal alapú megszakítás)
 *
 * Használat:
 * @code
 * // main elején (Startup)
 * LifecycleManager::instance().initStartup(); // opcionális, de ajánlott
 *
 * QApplication a(argc, argv);
 * // Automatikus fáziskövetés bekötése
 * LifecycleManager::instance().attach(&a);
 *
 * // Főablak létrejött; bekapcsoljuk az automatikus open/close figyelést
 * MainWindow w;
 * LifecycleManager::instance().watchMainWindow(&w);
 * w.show();
 *
 * // Abort jelek bekötése (SIGINT/SIGTERM)
 * SignalHelper::setCleanupHandler([](int sig){
 *     LifecycleManager::instance().onAbort(sig);
 * });
 * @endcode
 */

#pragma once

#include <QObject>
#include <QApplication>
#include <QMainWindow>
#include <QPointer>

//#include "common/logger/log_manager.h"
//class WindowObserver;  // előre deklaráció
#include "window_observer.h"

class LifecycleManager : public QObject {
    Q_OBJECT
public:
    enum class Phase {
        Startup,
        Application,
        MainWindowOpen,
        MainWindowClose,
        Shutdown,
        Abort
    };

    static LifecycleManager& instance();

public:

    // Jel alapú megszakítás (Abort)
    void onAbort(int signal);

    /// Indulás - bin mappával
    void setPhase_1(const char* argv0, const QString& appName);
    // áttérés dataPath-ra - log mozgatása
    void setPhase_2(const QString& a);
    // QApplication bekötése - bufferelés
    void setPhase_3(QCoreApplication* a);
    // Főablak figyelése (automatikus open/close követés)
    void setPhase_4(QMainWindow* window);
    void setPhase_4_1();
    void setPhase_4_2();
    /// Befejezés
    void setPhase_5(int exitCode);
private:
    explicit LifecycleManager(QObject* parent = nullptr);

    // Belső event filter a MainWindow open/close érzékeléséhez

    // Startup jelzés (main elején, még QApplication előtt)
    void initStartup(const QString& appName);

    // QApplication bekötése (automatikus Application és Shutdown követés)
    void attach(QCoreApplication* app);

    void markPhase(Phase phase);

    QPointer<QCoreApplication> app_;
    WindowObserver* observer_ = nullptr;

    QString _appName;
    int _exitCode = 0;
    int _signalCode = 0;
    bool startupMarked_ = false;
    bool applicationMarked_ = false;
    bool shutdownMarked_ = false;
};
