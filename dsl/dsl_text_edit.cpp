#include "dsl_text_edit.h"
#include "line_number_area.h"
#include <QAbstractItemView>
#include <QScrollBar>
#include <QKeyEvent>
#include <QPainter>
#include <QTextBlock>
#include <QToolTip>

DslTextEdit::DslTextEdit(QWidget* parent)
    : QPlainTextEdit(parent)
{
    _lineNumberArea = new LineNumberArea(this);

    connect(this, &DslTextEdit::blockCountChanged, this, [this](int){ updateLineNumberAreaWidth(); });
    connect(this, &DslTextEdit::updateRequest, this, [this](const QRect& r, int dy){
        if (dy)
            _lineNumberArea->scroll(0, dy);
        else
            _lineNumberArea->update(0, r.y(), _lineNumberArea->width(), r.height());
    });
    connect(this, &DslTextEdit::cursorPositionChanged, this, [this](){ _lineNumberArea->update(); });

    updateLineNumberAreaWidth();
}


void DslTextEdit::setCompleter(QCompleter* c)
{
    if (_completer)
        QObject::disconnect(_completer, nullptr, this, nullptr);

    _completer = c;

    if (!_completer)
        return;

    _completer->setWidget(this);
    _completer->setCompletionMode(QCompleter::PopupCompletion);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);

    connect(_completer, SIGNAL(activated(QString)),
            this, SLOT(insertCompletion(QString)));
}

void DslTextEdit::insertCompletion(const QString& completion)
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    tc.insertText(completion);
    setTextCursor(tc);
}

void DslTextEdit::keyPressEvent(QKeyEvent* e)
{
    if (_completer && _completer->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    QPlainTextEdit::keyPressEvent(e);

    if (!_completer)
        return;

    QString prefix = textCursor().selectedText();
    if (prefix.isEmpty())
        prefix = textCursor().block().text();

    if (prefix.length() < 1) {
        _completer->popup()->hide();
        return;
    }

    _completer->setCompletionPrefix(prefix);
    QRect cr = cursorRect();
    cr.setWidth(_completer->popup()->sizeHintForColumn(0)
                + _completer->popup()->verticalScrollBar()->sizeHint().width());
    _completer->complete(cr);
}

int DslTextEdit::lineNumberAreaWidth() const {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void DslTextEdit::lineNumberAreaPaintEvent(QPaintEvent* event) {
    QPainter painter(_lineNumberArea);
    painter.fillRect(event->rect(), QColor("#f0f0f0"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {

        if (block.isVisible() && bottom >= event->rect().top()) {

            // sor-szám
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::gray);
            painter.drawText(0, top, _lineNumberArea->width() - 4,
                             fontMetrics().height(),
                             Qt::AlignRight, number);

            // hibapötty
            if (_errorLines.contains(blockNumber)) {
                painter.setBrush(Qt::red);
                painter.setPen(Qt::red);
                int radius = 4;
                int cx = 4;

                // Pixelpontos baseline-igazítás
                int cy = top + fontMetrics().ascent() - radius + 1;

                painter.drawEllipse(QPoint(cx, cy), radius, radius);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}


void DslTextEdit::resizeEvent(QResizeEvent* event) {
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    _lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                       lineNumberAreaWidth(), cr.height()));
}

void DslTextEdit::updateLineNumberAreaWidth() {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void DslTextEdit::mouseMoveEvent(QMouseEvent* e)
{
    QTextCursor c = cursorForPosition(e->pos());
    c.select(QTextCursor::WordUnderCursor);
    QString tok = c.selectedText();

    if (_errorMap.contains(tok)) {
        QToolTip::showText(e->globalPosition().toPoint(), _errorMap[tok], this);
    } else {
        QToolTip::hideText();
    }

    QPlainTextEdit::mouseMoveEvent(e);
}

void DslTextEdit::updateInlineErrorSelections()
{
    QList<QTextEdit::ExtraSelection> extras;

    if (!_inlineErrors.isEmpty()) {
        QTextCursor c(document());
        c.movePosition(QTextCursor::EndOfLine);

        QTextEdit::ExtraSelection sel;
        sel.cursor = c;

        QTextCharFormat fmt;
        fmt.setForeground(QColor("#cc0000"));
        fmt.setFontItalic(true);

        // A trükk: FullWidthSelection + szöveg overlay
        fmt.setProperty(QTextFormat::FullWidthSelection, true);

        sel.format = fmt;

        extras.append(sel);

        // A tényleges overlay szöveg NEM a dokumentumba kerül,
        // hanem a paintEvent-ben rajzoljuk rá.
    }

    setExtraSelections(extras);
}


void DslTextEdit::paintEvent(QPaintEvent* e)
{
    // 1) Üres mező → csak placeholder, semmi más
    if (toPlainText().trimmed().isEmpty()) {
        setErrorLines({});
        QPlainTextEdit::paintEvent(e);
        return;
    }

    // 2) Alap rajzolás
    QPlainTextEdit::paintEvent(e);

    // 3) Ha nincs inline hiba → nincs overlay
    if (_inlineErrors.isEmpty())
        return;

    // 4) Overlay pozíció meghatározása
    QPainter p(viewport());
    p.setPen(QColor("#cc0000"));
    p.setFont(QFont(font().family(), font().pointSize(), QFont::StyleItalic));

    QString msg = "// " + _inlineErrors.join("; ");

    // Sor pozíció (ugyanaz a blokk, mint a bogyónál)
    QTextCursor c(document());
    c.movePosition(QTextCursor::EndOfLine);
    QTextBlock block = c.block();

    int blockNumber = block.blockNumber();
    QTextBlock first = firstVisibleBlock();

    // A blokk top koordinátája
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();

    // Baseline (ugyanaz, mint a bogyónál)
    int baseline = top + fontMetrics().ascent();

    // Jobbra igazítás
    int right = viewport()->width() - 6;
    QFontMetrics fm(p.font());
    int msgWidth = fm.horizontalAdvance(msg);
    int x = right - msgWidth;

    // Rajzolás
    p.drawText(x, baseline, msg);

}

