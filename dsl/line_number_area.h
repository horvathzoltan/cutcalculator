#pragma once
#include <QWidget>

class DslTextEdit;

class LineNumberArea : public QWidget {
public:
    explicit LineNumberArea(DslTextEdit* editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    DslTextEdit* _editor;
};
