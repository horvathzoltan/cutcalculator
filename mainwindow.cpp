#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common/logger/event_logger.h"
#include "ui/adapters/log_view_adapter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initEventLogWidget();
}

MainWindow::~MainWindow()
{
    delete _logAdapter;
    delete ui;
}

void MainWindow::initEventLogWidget() {
    if(_logAdapter) return;

    _logAdapter = new LogViewAdapter(ui->eventLog);

    EventLogger::instance().emitEvent = [this](const QString& line) {
        _logAdapter->appendColoredLineWithTimestamp(line);
    };

    QStringList recent = EventLogger::instance().loadRecentEventsFromLastStart();
    _logAdapter->appendLines(recent);
}
