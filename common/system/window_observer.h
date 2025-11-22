// window_observer.h
#pragma once
#include <QObject>
#include <QMainWindow>
#include <QPointer>

class WindowObserver : public QObject {
    Q_OBJECT
public:
    explicit WindowObserver(QObject* parent = nullptr);
    void setWindow(QMainWindow* w);
protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
private:
    QPointer<QMainWindow> window_;
    bool openedOnce_ = false;
};
