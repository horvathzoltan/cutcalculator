#include "line_number_area.h"
#include "dsl_text_edit.h"
#include <QPainter>
#include "line_number_area.h"
#include "dsl_text_edit.h"
#include <QPainter>

LineNumberArea::LineNumberArea(DslTextEdit* editor)
    : QWidget(editor), _editor(editor)
{
}

QSize LineNumberArea::sizeHint() const {
    return QSize(_editor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    _editor->lineNumberAreaPaintEvent(event);
}
