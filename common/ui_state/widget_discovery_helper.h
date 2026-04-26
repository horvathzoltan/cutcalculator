#pragma once

#include <QWidget>

/*
 * WidgetDiscoveryHelper
 * ----------------------
 * A WidgetStateManager által használt widgetfa-bejáró.
 *
 * Feladata:
 *   - meghatározni, mely widgetek relevánsak a WidgetState mentéséhez
 *   - rekurzívan összegyűjteni ezeket a widgeteket
 *
 * A relevancia-szűrés célja:
 *   - csak olyan widgetek kerüljenek mentésre, amelyeknek van állapota
 *     (splitter, tabwidget, scrollarea, groupbox, frame, stb.)
 *   - dekorációk, egyszerű kontrollok és Qt belső widgetek kizárása
 *
 * Ez a helper NEM végez állapotmentést — csak felfedezi a widgeteket.
 */

class WidgetDiscoveryHelper
{
public:
    static bool isWidgetStateRelevant(QWidget* w);
    static QList<QWidget *> collect(QWidget *root);

};

