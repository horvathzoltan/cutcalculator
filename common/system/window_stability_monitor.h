#pragma once

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QRect>
#include <QList>

class QWidget;
class QTabWidget;
class QSplitter;

class WindowStabilityMonitor : public QObject
{
    Q_OBJECT
public:
    static WindowStabilityMonitor& instance();

    // A fő ablakot adjuk át neki (MainWindow*)
    void attachTo(QWidget* window);

signals:
    void windowStable();   // egyszer, amikor tényleg stabil

private:
    explicit WindowStabilityMonitor(QObject* parent = nullptr);

    QPointer<QWidget> _window;
    QTimer _timer;

    int _stableCount = 0;

    QString _lastProfile;
    double _lastDpi = 0.0;
    QRect _lastGeometry;
    QRect _lastTabsGeometry;
    bool _tabsStable = false;
    bool _splitterStable = false;
    QList<int> _lastSplitterSizes;

    void poll();
    bool isStableOnce() const;
};
