#pragma once

#include <QWidget>
#include <QVariantMap>

class QSplitter;
class QHeaderView;
class QTabWidget;
class QAbstractScrollArea;

/*
 * StateHandlers
 * --------------
 * A WidgetStateManager által használt extract/restore függvények.
 *
 * Feladatuk:
 *   - widget-specifikus állapot kinyerése (extract)
 *   - widget-specifikus állapot visszaállítása (restore)
 *
 * * Felelősségi kör:
 *   - QSplitter: percent-alapú méretarányok
 *   - QHeaderView: oszlopszélességek + sort state
 *   - QTabWidget: currentIndex
 *   - QAbstractScrollArea: scroll pozíciók
 *
 * A mentett formátum:
 *   - minden handler a saját kulcsait a WidgetStateSettings map-jébe írja
 *   - a kulcsok a WidgetStateManager által generált "key" prefixet használják
 *
 * A logolás célja:
 *   - auditálható extract/restore folyamat
 *   - egységes, modern, két-rétegű UI-state architektúrát tükröző kommunikáció
 *
 * A működés NEM változik — csak a logok és kommentek modernizálása történik.
 *
 * Kapcsolat a WidgetStateManager-rel:
 *   - a WidgetStateManager hívja meg a handler-eket
 *   - a handler-ek a WidgetStateSettings map-jébe írnak
 *
 * Kapcsolat a WindowStateManager-rel:
 *   - nincs közvetlen kapcsolat
 *
 * A StateHandlers a UI-state pipeline 3. rétege.
 */


namespace StateHandlers
{
// QSplitter
void extractSplitter(QWidget* w, QVariantMap& m, const QString& key);
void restoreSplitter(QWidget* w, QVariantMap& m, const QString& key);

// QHeaderView
void extractHeader(QWidget* w, QVariantMap& m, const QString& key);
void restoreHeader(QWidget* w, QVariantMap& m, const QString& key);

// QTabWidget
void extractTabWidget(QWidget* w, QVariantMap& m, const QString& key);
void restoreTabWidget(QWidget* w, QVariantMap& m, const QString& key);

// QAbstractScrollArea
void extractScrollArea(QWidget* w, QVariantMap& m, const QString& key);
void restoreScrollArea(QWidget* w, QVariantMap& m, const QString& key);

}
