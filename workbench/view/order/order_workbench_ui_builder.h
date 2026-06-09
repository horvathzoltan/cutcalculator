#pragma once
#include <QWidget>
#include <QToolBar>

#include "workbench/view/order/order_workbench.h"

//class OrderWorkbench;

class OrderWorkbenchUIBuilder
{
public:

    // UIModel = logikai UI elemek, amelyeket a Workbench vagy a Presenter használ.
    // Nem tartalmaz pusztán vizuális widgeteket (toolbar, layout, splitter).

    static OrderWorkbenchUIModel build(OrderWorkbench* wb);
};


/*
  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  State              listPanel   listPlaceholder   Add   Delete   Rename   Refresh   headerPanel   headerPlaceholder   Modify   Save   Cancel   Delete(H)   itemTable   itemPlaceholder   itemAdd   itemDelete
  ------------------ ----------- ----------------- ----- -------- -------- --------- ------------- ------------------- -------- ------ -------- ----------- ----------- ----------------- --------- ------------
  InitialEmpty       H           V                 E     D        D        E         H             V                   D        D      D        D           H           V                 D         D
  NewHeaderEditing   H           V                 D     D        D        E         V             H                   D        D      D        D           H           V                 D         D
  NewHeaderEdited    H           V                 D     D        D        E         V             H                   D        E      E        D           H           V                 D         D
  ListNoSelection    V           H                 E     D        D        E         H             V                   D        D      D        D           H           V                 D         D
  HeaderSelected     V           H                 E     E        E        E         V             H                   E        D      D        E           V           H                 E         E
  HeaderEditing      V           H                 D     D        D        E         V             H                   D        E      E        D           V           H                 E         E
  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- * */