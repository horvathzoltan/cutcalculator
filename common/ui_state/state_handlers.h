#pragma once

#include <QWidget>
#include <QVariantMap>

class QSplitter;
class QHeaderView;
class QTabWidget;
class QAbstractScrollArea;

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
