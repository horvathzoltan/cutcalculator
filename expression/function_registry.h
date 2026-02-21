#pragma once
#include <QMap>
#include <QString>
#include <functional>
#include "value.h"

using NativeFn = std::function<Value(const QVector<Value>&)>;

class FunctionRegistry {
public:
    static FunctionRegistry& instance() {
        static FunctionRegistry r;
        return r;
    }

    void registerFn(const QString& name, NativeFn fn) {
        fns[name] = fn;
    }

    Value call(const QString& name, const QVector<Value>& args) const {
        auto it = fns.find(name);
        if (it == fns.end())
            throw QString("Undefined function: %1").arg(name);

        if (args.isEmpty()) {
            throw QString("Function %1 called with no arguments").arg(name);
        }

        return it.value()(args);
    }


private:
    QMap<QString, NativeFn> fns;
};


// Qt Creator snippet javaslat: "fn_register_macro"

#define REGISTER_FN(name, fn) \
static bool _reg_##name = [](){ \
        FunctionRegistry::instance().registerFn(#name, fn); \
        return true; \
}()

REGISTER_FN(add, [](auto a){ return Value::numberValue(a[0].number + a[1].number); });
REGISTER_FN(sub, [](auto a){ return Value::numberValue(a[0].number - a[1].number); });
REGISTER_FN(mul, [](auto a){ return Value::numberValue(a[0].number * a[1].number); });
REGISTER_FN(div, [](auto a){
    if (a[1].number == 0.0)
        throw QString("Division by zero");
    return Value::numberValue(a[0].number / a[1].number);
});

REGISTER_FN(gt,  [](auto a){ return Value::boolValue(a[0].number >  a[1].number); });
REGISTER_FN(lt,  [](auto a){ return Value::boolValue(a[0].number <  a[1].number); });
REGISTER_FN(ge,  [](auto a){ return Value::boolValue(a[0].number >= a[1].number); });
REGISTER_FN(le,  [](auto a){ return Value::boolValue(a[0].number <= a[1].number); });
REGISTER_FN(eq,  [](auto a){ return Value::boolValue(a[0].number == a[1].number); });

REGISTER_FN(qty_fixed,    [](auto a){ return Value::numberValue(a[0].number * a[1].number); });
REGISTER_FN(qty_perOrder, [](auto a){ return Value::numberValue(a[0].number); });
REGISTER_FN(qty_perArea,  [](auto a){ return Value::numberValue((a[0].number * a[1].number) / a[2].number); });


// --- Operator overloads (nem használunk makrót, mert a név nem lehet identifier) ---

static bool _reg_op_plus = [](){
    FunctionRegistry::instance().registerFn("+",
                                            [](const QVector<Value>& a){
                                                return Value::numberValue(a[0].number + a[1].number);
                                            });
    return true;
}();

static bool _reg_op_minus = [](){
    FunctionRegistry::instance().registerFn("-",
                                            [](const QVector<Value>& a){
                                                return Value::numberValue(a[0].number - a[1].number);
                                            });
    return true;
}();

static bool _reg_op_mul = [](){
    FunctionRegistry::instance().registerFn("*",
                                            [](const QVector<Value>& a){
                                                return Value::numberValue(a[0].number * a[1].number);
                                            });
    return true;
}();

static bool _reg_op_div = [](){
    FunctionRegistry::instance().registerFn("/",
                                            [](const QVector<Value>& a){
                                                if (a[1].number == 0.0)
                                                    throw QString("Division by zero");
                                                return Value::numberValue(a[0].number / a[1].number);
                                            });
    return true;
}();

// --- Relációs operátorok ---

static bool _reg_op_gt = [](){
    FunctionRegistry::instance().registerFn(">",
                                            [](const QVector<Value>& a){
                                                return Value::boolValue(a[0].number > a[1].number);
                                            });
    return true;
}();

static bool _reg_op_lt = [](){
    FunctionRegistry::instance().registerFn("<",
                                            [](const QVector<Value>& a){
                                                return Value::boolValue(a[0].number < a[1].number);
                                            });
    return true;
}();

static bool _reg_op_ge = [](){
    FunctionRegistry::instance().registerFn(">=",
                                            [](const QVector<Value>& a){
                                                return Value::boolValue(a[0].number >= a[1].number);
                                            });
    return true;
}();

static bool _reg_op_le = [](){
    FunctionRegistry::instance().registerFn("<=",
                                            [](const QVector<Value>& a){
                                                return Value::boolValue(a[0].number <= a[1].number);
                                            });
    return true;
}();

static bool _reg_op_eq = [](){
    FunctionRegistry::instance().registerFn("==",
                                            [](const QVector<Value>& a){
                                                return Value::boolValue(a[0].number == a[1].number);
                                            });
    return true;
}();
