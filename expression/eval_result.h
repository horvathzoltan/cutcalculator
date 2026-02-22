#pragma once
#include "expression/ast.h"
#include <QString>

struct TraceEntry {
    QString nodeType;
    QString nodeValue;
    QString result;
};

struct EvalResult {
    bool ok = false;
    QString error;
    //AstNode* ast = nullptr;

    QString astDump;

    QStringList tokensDump;
    QStringList rpnDump;

    QSet<QString> readVars;
    QSet<QString> writtenVars;

    QVector<TraceEntry> trace;

    static EvalResult success() {
        EvalResult r;
        r.ok = true;
        return r;
    }

    static EvalResult failure(const QString& msg) {
        EvalResult r;
        r.ok = false;
        r.error = msg;
        return r;
    }
};
