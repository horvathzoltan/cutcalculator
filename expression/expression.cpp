#include "expression.h"
#include "function.h"
#include "keyword.h"

Expression::Expression()
{
    _keyword=KeyWord::Undefined;
    _isValid=false;
}

Expression::Expression(
    KeyWord::Word keyword,
    const QList<QVariant> &params,
    const QString& original_text
    )
{
    _keyword = keyword;
    _params = params;
    _original_text = original_text;
    _isValid=true;
}
//0123456789
//avg ( 52.5,57.7)
auto Expression::Parse(const QString &txt) -> Expression
{
    int ix = txt.indexOf('(');
    if(ix==-1) return {};
    auto key = txt.left(ix).trimmed();
    auto keyword = KeyWord::Parse(key);
    QList<QVariant> params;
    if(keyword!=KeyWord::Undefined){
        int ix2 = txt.indexOf(')', ix+1);
        if(ix2==-1) return {};
        auto params_txt = txt.mid(ix+1, ix2-ix-1);
        if(params_txt.isEmpty()) return {};
        auto paramtxts = params_txt.split(',');
        for(auto&paramtxt:paramtxts){
            params.append(paramtxt.trimmed());
        }
    }
    return {keyword, params, txt};
}

auto Expression::ToString() const -> QString
{
    if(_keyword==KeyWord::Undefined) return _original_text;
    QStringList e;
    for(auto&p:_params) e.append(p.toString());
    return KeyWord::ToString(_keyword)+'('+e.join(',')+')';
}

auto Expression::Calculate() -> QVariant
{
    switch(_keyword){
    case KeyWord::avg: return(Function::avg(_params));
    case KeyWord::sum: return(Function::sum(_params));
    case KeyWord::sub: return(Function::sub(_params));
    case KeyWord::add: return(Function::add(_params));
    case KeyWord::mul: return(Function::mul(_params));
    case KeyWord::div: return(Function::div(_params));
    case KeyWord::overdrill: return(Function::overdrill(_params));
    default: return {};
    }
    return "?";
}
