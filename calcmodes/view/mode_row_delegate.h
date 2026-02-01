#pragma once
#include <QStyledItemDelegate>
#include "colors/model/colorconstants.h"

class ModeRowDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* p, const QStyleOptionViewItem& opt,
               const QModelIndex& idx) const override
    {
        QStyleOptionViewItem o(opt);
        initStyleOption(&o, idx);

        // Ha a cellának van saját háttérszíne (pl. sárga hiányos mód)
        const QVariant bg = idx.data(Qt::BackgroundRole);
        if (bg.isValid()) {
            o.backgroundBrush = QBrush(bg.value<QColor>());
        }

        // Hover overlay
        if (opt.state.testFlag(QStyle::State_MouseOver)) {
            o.backgroundBrush = QColor(220, 235, 255); // halvány kék
        }

        // Selection overlay
        if (opt.state.testFlag(QStyle::State_Selected)) {
            o.backgroundBrush = QColor(180, 210, 255); // erősebb kék
        }

        QStyledItemDelegate::paint(p, o, idx);
    }
};
