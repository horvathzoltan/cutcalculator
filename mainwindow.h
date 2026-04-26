#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "products/view/product_tree_manager.h"
#include "ui/adapters/log_view_adapter.h"
#include <QMainWindow>
#include <QSplitter>
#include <QTreeView>

class MaterialTableWidget;
class MaterialTableManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LogViewAdapter* _logAdapter = nullptr;   // pointer
    QSplitter* _productTypesSplitter = nullptr;

    void initEventLogWidget();
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;
    void moveEvent(QMoveEvent* e) override;
    void changeEvent(QEvent* e) override;

/*material*/
    MaterialTableWidget* _materialsTable = nullptr;
    MaterialTableManager* _materialsManager = nullptr;

    void initMaterialsTab();   // material viewer init
    void initBOMWorkbenchTab(); // bom workbench init
    void initOrderWorkbenchTab();

/*product*/
    QTreeView* _productTreeView = nullptr;
    ProductTreeManager* _productTreeManager = nullptr;

    bool _windowRestoredOnce = false;
    bool _restoreInProgress = false;
    QString _initialMonitorProfile;   // induló monitorprofil (early restore-hoz)
    QString _lastSeenProfile;         // final placement stabilizációhoz

    //void BOMWorkbenchSaveState();
signals:
    void finalPlacementReached();

private slots:
    void onWindowStable();

};
#endif // MAINWINDOW_H
