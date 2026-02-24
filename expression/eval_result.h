#pragma once
#include "expression/ast.h"
#include "expression/value.h"
#include <QString>

struct TraceEntry {
    QString nodeType;
    QString nodeValue;
    QString result;
};

struct EvalResult {
    QString code;   // <-- új mező: a kiértékelt kód

    bool ok = false;
    QString error;

    QString astDump;

    QStringList tokensDump;
    QStringList rpnDump;

    QSet<QString> readVars;
    QSet<QString> writtenVars;

    QVector<TraceEntry> trace;

    QMap<QString, Value> variableSnapshot;   // <-- új mező: változók értékei

    static EvalResult success(const QString& code = QString()) {
        EvalResult r;
        r.ok = true;
        r.code = code;
        return r;
    }

    static EvalResult failure(const QString& code, const QString& msg) {
        EvalResult r;
        r.ok = false;
        r.code = code;
        r.error = msg;
        return r;
    }

    void debugDump() const;   // <-- új tagfüggvény deklaráció
};

