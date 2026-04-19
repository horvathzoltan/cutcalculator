#include "layout_critical_helper.h"

#include <QWidget>
#include <QScreen>
#include <QTabWidget>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAbstractButton>
#include <QLabel>
#include <QComboBox>
#include <QScrollBar>
#include <QLineEdit>
#include <QAbstractScrollArea>
#include <QHeaderView>

bool LayoutCriticalHelper::isLayoutCritical(QWidget* w)
{
    if (!w)
        return false;

    // Láthatatlan widget nem számít
    if (!w->isVisible())
        return false;

    // Qt belső widgetek kizárása
    if (w->objectName().startsWith("qt_"))
        return false;

    // Névtelen QHeaderView kizárása (Qt belső header)
    if (qobject_cast<QHeaderView*>(w) && w->objectName().isEmpty())
        return false;


    // Dekorációk kizárása
    if (qobject_cast<QMenuBar*>(w)) return false;
    if (qobject_cast<QToolBar*>(w)) return false;
    if (qobject_cast<QStatusBar*>(w)) return false;
    if (qobject_cast<QScrollBar*>(w)) return false;
    if (qobject_cast<QTabBar*>(w)) return false;

    // Gombok, címkék, egyszerű kontrollok kizárása
    if (qobject_cast<QAbstractButton*>(w)) return false;
    if (qobject_cast<QLabel*>(w)) return false;
    if (qobject_cast<QLineEdit*>(w)) return false;
    if (qobject_cast<QComboBox*>(w)) return false;

    // Layout nélküli widgetek kizárása
    if (!w->layout() &&
        !qobject_cast<QAbstractScrollArea*>(w) &&
        !qobject_cast<QSplitter*>(w))
        return false;

    // Általános container kizárása: QWidget + van layoutja + nem speciális widget
    if (w->layout() &&
        !qobject_cast<QAbstractScrollArea*>(w) &&
        !qobject_cast<QSplitter*>(w) &&
        !qobject_cast<QTabWidget*>(w) &&
        !qobject_cast<QHeaderView*>(w))
        return false;

    // Ha idáig eljutott → layout‑kritikus
    return true;

}


QList<QWidget*> LayoutCriticalHelper::collect(QWidget* root)
{
    QList<QWidget*> list;
    if (!root)
        return list;

    // 1) Ha kritikus → felvesszük
    if (isLayoutCritical(root))
        list << root;

    // 2) Rekurzió → mindig
    const auto children = root->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget* c : children) {
        list << collect(c);
    }

    return list;
}