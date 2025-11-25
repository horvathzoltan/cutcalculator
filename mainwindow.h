#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui/adapters/log_view_adapter.h"
#include <QMainWindow>

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
    MaterialTableWidget* _materialsTable = nullptr;
    MaterialTableManager* _materialsManager = nullptr;

    void initEventLogWidget();
    void closeEvent(QCloseEvent *event) override;

    bool event(QEvent *e) override;

    void initMaterialsTab();   // viewer init
    void loadMaterials();      // repository → registry
};
#endif // MAINWINDOW_H
