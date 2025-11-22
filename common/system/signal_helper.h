#pragma once

#include <csignal>
#include <functional>
#include <QString>
#include <QCoreApplication>
#include "common/logger/logger.h"

class SignalHelper
{
public:
    static constexpr int SIGINT_  = SIGINT;
    static constexpr int SIGTERM_ = SIGTERM;

    /// Beállítja a kívánt leállító jelet (SIGINT, SIGTERM)
    static void setShutDownSignal(int signalId)
    {
#ifdef __linux__
        struct sigaction sa;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = handleShutDownSignal;
        if (sigaction(signalId, &sa, nullptr) == -1)
        {
            perror("setting up termination signal");
            ::exit(1);
        }
#elif defined(_WIN32)
        signal(signalId, handleShutDownSignal);
#else
        Q_UNUSED(signalId);
#endif
    }

    /// Opcionális cleanup callback beállítása (signalId‑val)
    static void setCleanupHandler(std::function<void(int)> fn)
    {
        cleanupHandler() = std::move(fn);
    }

    static QString signalName(int sig) {
        switch (sig) {
        case SIGHUP:   return "SIGHUP";   // terminál bezárás / controlling tty megszűnése
        case SIGINT:   return "SIGINT";   // Ctrl+C
        case SIGQUIT:  return "SIGQUIT";  // Ctrl+\ (core dump)
        case SIGILL:   return "SIGILL";   // illegális utasítás
        case SIGTRAP:  return "SIGTRAP";  // debugger break
        case SIGABRT:  return "SIGABRT";  // abort()
        case SIGBUS:   return "SIGBUS";   // bus error
        case SIGFPE:   return "SIGFPE";   // lebegőpontos hiba
        case SIGKILL:  return "SIGKILL";  // nem elfogható, nem ignorálható kill
        case SIGUSR1:  return "SIGUSR1";  // user-defined jel 1
        case SIGSEGV:  return "SIGSEGV";  // szegmens hiba
        case SIGUSR2:  return "SIGUSR2";  // user-defined jel 2
        case SIGPIPE:  return "SIGPIPE";  // írás lezárt pipe-ba
        case SIGALRM:  return "SIGALRM";  // alarm() timeout
        case SIGTERM:  return "SIGTERM";  // killall, normál terminálás
        case SIGCHLD:  return "SIGCHLD";  // child process kilépett
        case SIGCONT:  return "SIGCONT";  // folytatás (fg/bg)
        case SIGSTOP:  return "SIGSTOP";  // nem ignorálható stop
        case SIGTSTP:  return "SIGTSTP";  // Ctrl+Z
        case SIGTTIN:  return "SIGTTIN";  // háttérben futó process olvasna tty-ról
        case SIGTTOU:  return "SIGTTOU";  // háttérben futó process írna tty-ra
        default:       return QString("signal %1").arg(sig);
        }
    }


private:
    static void handleShutDownSignal(int signalId)
    {
        zInfo(QStringLiteral("EXIT: %1").arg(signalId));

        // ha van cleanup callback, futtatjuk
        if (cleanupHandler()) {
            cleanupHandler()(signalId);
        }

        QCoreApplication::exit(0);
    }

    // statikus tároló a callbacknek
    static std::function<void(int)>& cleanupHandler()
    {
        static std::function<void(int)> fn;
        return fn;
    }
};
