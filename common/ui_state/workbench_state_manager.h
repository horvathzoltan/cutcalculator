#pragma once
#include <QObject>
#include <QMap>
#include <QString>

class QWidget;

class WorkbenchStateManager : public QObject {
    Q_OBJECT
public:
    static WorkbenchStateManager& instance();

    void registerWorkbench(const QString& id, QWidget* wb);
    void onTabActivated(QWidget* wb);
    void onTabDeactivated(QWidget* wb, bool immediate = false);

    void beginClosing();

    bool isShown(QWidget *wb);
private:
    bool _closing = false;

    struct WBInfo {
        QWidget* widget = nullptr;
        bool wasShown = false;
    };

    QMap<QString, WBInfo> _workbenches;

    explicit WorkbenchStateManager(QObject* parent = nullptr);
    QString findIdByWidget(QWidget* wb) const;
};
