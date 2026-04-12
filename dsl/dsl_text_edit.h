#pragma once
#include <QPlainTextEdit>
#include <QCompleter>

class DslTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit DslTextEdit(QWidget* parent = nullptr);

    void setCompleter(QCompleter* c);
    QCompleter* completer() const { return _completer; }

protected:
    void keyPressEvent(QKeyEvent* e) override;

private:
    QCompleter* _completer = nullptr;

public:
    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    void updateLineNumberAreaWidth();


protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QWidget* _lineNumberArea = nullptr;
    QSet<int> _errorLines;

public:
    void setErrorLines(const QSet<int>& lines) { _errorLines = lines; _lineNumberArea->update(); }

public:
    const QSet<int>& errorLines() const { return _errorLines; }

protected:
    void mouseMoveEvent(QMouseEvent* e) override;

protected:
    void paintEvent(QPaintEvent* e) override;

public:
    void setErrorMap(const QHash<QString, QString>& map) { _errorMap = map; }

private:
    QHash<QString, QString> _errorMap;

private:
    QStringList _inlineErrors;

public:
    void setInlineErrors(const QStringList& errs) {
        if (toPlainText().trimmed().isEmpty()) {
            _inlineErrors.clear();
            setExtraSelections({});
            return;
        }

        _inlineErrors = errs;
        updateInlineErrorSelections();
    }

private:
    void updateInlineErrorSelections();

private slots:
    void insertCompletion(const QString& completion);
};
