#pragma once

#include <QObject>
#include <QPointer>
 #include <QTimer>
// #include <QRect>
// #include <QList>
// #include <QRegion>
// #include <QHash>
#include <QWidget>

// class QWidget;
// class QTabWidget;
// class QSplitter;

class WindowStabilityMonitor : public QObject
{
    Q_OBJECT


    struct WidgetSnapshot {
        QRect geometry;
        QSize size;
        QSize minSize;
    };

public:
    static WindowStabilityMonitor& instance();

    // A fő ablakot adjuk át neki (MainWindow*)
    void attachTo(QWidget* window);

signals:
    void windowStable();   // egyszer, amikor tényleg stabil
    void uiReadyForRestore(); // UIStateCollector restore trigger

private:
    explicit WindowStabilityMonitor(QObject* parent = nullptr);

    QPointer<QWidget> _window;
    QTimer _timer;
    int _stableCount = 0;
    QString _lastProfile;
    double _lastDpi = 0.0;
    QRect _lastGeometry;
    QRect _lastTabsGeometry;
    QList<int> _lastSplitterSizes;
    QHash<QWidget*, WidgetSnapshot> _lastWidgetState;
    int _recentInstabilityCount = 0;
    int _tabsStableCount = 0;
    int _splitterStableCount = 0;
    int _childrenStableCount = 0;

    void poll();
    bool isStableOnce() const;
    bool areWidgetsStable(const QList<QWidget*>& widgets);

    int computeAdaptiveThreshold(int widgetCount, bool dpiChanged, int recentInstability);
};
