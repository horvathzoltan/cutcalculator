#pragma once
#include <QWidget>

class MaterialTableWidget;
class MaterialTableManager;

class MaterialsWorkbench : public QWidget {
    Q_OBJECT
public:
    explicit MaterialsWorkbench(QWidget* parent = nullptr);

//protected:
    //void hideEvent(QHideEvent* e) override;
  //  bool event(QEvent* e) override;

private:
    MaterialTableWidget* _table = nullptr;
    MaterialTableManager* _manager = nullptr;
};
