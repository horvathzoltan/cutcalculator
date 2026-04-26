#include "widget_discovery_helper.h"

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
#include <QDockWidget>
#include <QGroupBox>
#include <QToolButton>

/*
 * Widget relevancia-szűrés
 * -------------------------
 * A cél: csak olyan widgetek kerüljenek a WidgetState-be,
 * amelyeknek ténylegesen van menthető állapotuk.
 *
 * Kizárjuk:
 *   - láthatatlan widgetek
 *   - Qt belső widgetek (qt_ prefix)
 *   - dekorációk (menubar, toolbar, statusbar, dockwidget)
 *   - egyszerű kontrollok (button, label, lineedit, combobox)
 *   - layout nélküli widgetek (kivéve scrollarea/splitter)
 *   - általános container widgetek (QWidget + layout, de nem speciális)
 *
 * Engedélyezzük:
 *   - QAbstractScrollArea
 *   - QSplitter
 *   - QTabWidget
 *   - QHeaderView
 *   - QFrame (ha van layout)
 *   - QGroupBox (ha van layout)
*/
bool WidgetDiscoveryHelper::isWidgetStateRelevant(QWidget* w)
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
    if (qobject_cast<QDockWidget*>(w)) return false;
    if (qobject_cast<QScrollBar*>(w)) return false;
    if (qobject_cast<QTabBar*>(w)) return false;

    // Gombok, címkék, egyszerű kontrollok kizárása
    if (qobject_cast<QAbstractButton*>(w)) return false;
    if (qobject_cast<QToolButton*>(w)) return false;
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
        !qobject_cast<QHeaderView*>(w) &&
        !qobject_cast<QFrame*>(w) &&
        !qobject_cast<QGroupBox*>(w))
        return false;

    // Ha idáig eljutott → layout‑kritikus
    return true;

}

/*
 * Widgetfa bejárás
 * ----------------
 * Rekurzívan összegyűjti a releváns widgeteket.
 *
 * Külön szabály:
 *   - QDockWidget és QToolButton alatt nem megyünk tovább
 *     (ezek nem layout-kritikusak, és felesleges zajt okoznának)
 */

QList<QWidget*> WidgetDiscoveryHelper::collect(QWidget* root)
{
    QList<QWidget*> list;
    if (!root)
        return list;

    if (qobject_cast<QDockWidget*>(root) ||
        qobject_cast<QToolButton*>(root))
        return list;

    // 1) Ha kritikus → felvesszük
    if (isWidgetStateRelevant(root))
        list.append(root);

    const auto children = root->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget* c : children) {
        const QList<QWidget*> sub = collect(c);
        list.append(sub);
    }

    return list;
}