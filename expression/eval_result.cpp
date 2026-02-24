#include "eval_result.h"
#include "common/logger/logger.h"
#include "value.h"

void EvalResult::debugDump() const
{
    zInfo() << "\n================ DEBUG PIPELINE ================";
    zInfo() << "Code:" << code;

    // ERROR
    if (!error.isEmpty()) {
        zInfo() << "\n=== ERROR ===";
        zInfo() << error;
    }

    // TOKENS
    zInfo() << "\n=== Tokens ===";
    if (!tokensDump.isEmpty()) {
        for (const QString& s : tokensDump)
            zInfo() << " " << s;
    } else {
        zInfo() << " (no token dump)";
    }

    // RPN
    zInfo() << "\n=== RPN ===";
    if (!rpnDump.isEmpty()) {
        for (const QString& s : rpnDump)
            zInfo() << " " << s;
    } else {
        zInfo() << " (no RPN dump)";
    }

    // AST
    if (!astDump.isEmpty()) {
        zInfo() << "\n=== AST DUMP ===\n" << astDump;
    }

    // READ/WRITTEN VARS
    auto setToString = [](const QSet<QString>& s) {
        QStringList list = QStringList(s.begin(), s.end());
        list.sort();
        return "{" + list.join(", ") + "}";
    };

    zInfo() << "Read variables:" << setToString(readVars);
    zInfo() << "Written variables:" << setToString(writtenVars);

    // VARIABLES
    if (!variableSnapshot.isEmpty()) {
        zInfo() << "\n=== Variables ===";
        for (auto it = variableSnapshot.begin(); it != variableSnapshot.end(); ++it) {
            const Value& v = it.value();
            zInfo() << QString("[%3] %1 = %2")
                           .arg(it.key())
                           .arg(v.toString())
                           .arg(v.typeName());
        }
    }

    // TRACE
    if (!trace.isEmpty()) {
        zInfo() << "\n=== Execution Trace ===";
        for (const auto& te : trace)
            zInfo() << " " << te.nodeType << "(" << te.nodeValue << ") => " << te.result;
    }

    zInfo() << "================================================\n";
}
