#pragma once
#include <QCompleter>
#include <QStringListModel>

class DslCompleter : public QCompleter {
    Q_OBJECT
public:
    explicit DslCompleter(const QStringList& words, QObject* parent = nullptr)
        : QCompleter(parent)
    {
        auto* model = new QStringListModel(words, this);
        setModel(model);
        setCaseSensitivity(Qt::CaseInsensitive);
        setCompletionMode(QCompleter::PopupCompletion);
        setFilterMode(Qt::MatchContains);
    }
};
