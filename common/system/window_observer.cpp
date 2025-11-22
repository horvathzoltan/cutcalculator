// window_observer.cpp
#include "window_observer.h"
#include "lifecycle_manager.h"
#include <QEvent>

WindowObserver::WindowObserver(QObject* parent) : QObject(parent) {}

void WindowObserver::setWindow(QMainWindow* w) {
    if (window_) window_->removeEventFilter(this);
    window_ = w;
    if (window_) window_->installEventFilter(this);
}

bool WindowObserver::eventFilter(QObject* obj, QEvent* ev) {
    if (!window_ || obj != window_) return QObject::eventFilter(obj, ev);

    if (ev->type() == QEvent::Show && !openedOnce_) {
        openedOnce_ = true;
        LifecycleManager::instance().setPhase_4_1();//markPhase(LifecycleManager::Phase::MainWindowOpen);
    }
    if (ev->type() == QEvent::Close) {
        LifecycleManager::instance().setPhase_4_2();//markPhase(LifecycleManager::Phase::MainWindowClose);
    }
    return QObject::eventFilter(obj, ev);
}
