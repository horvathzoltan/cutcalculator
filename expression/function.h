#pragma once

#include "variable.h"

#include <QList>
#include <QVariant>
#include <QtMath>

class Function{
public:
    static QVariant avg(const QList<QVariant>& params){
        if(params.isEmpty()) return {};
        double a=0,b=0;
        bool isok;
        for(auto&p:params){
            auto d = p.toDouble(&isok);
            if(isok){ a+=d; b++;}
        }
        return (b!=0)?a/b:QVariant();
    }

    static QVariant sum(const QList<QVariant>& params){
        if(params.isEmpty()) return {};
        double a=0;
        auto ds= ToDoubles(params);

        for(auto&d:ds) if(!qIsNaN(d))a+=d;
        return a;
    }

    static QList<double> ToDoubles(const QList<QVariant>& params){
        QList<double> l;

        for (auto& p : params) {
            QString s0 = p.toString();
            double d0;
            bool ok = false;

            if (s0.startsWith('$')) {
                QString token = s0.mid(1);
                d0 = VariableRepository::instance().get(token).toDouble(&ok);
            } else {
                d0 = p.toDouble(&ok);
            }

            if (ok)
                l.append(d0);
            else
                l.append(qQNaN());
        }

        return l;
    }


    static QVariant add(const QList<QVariant>& params){
        if(params.isEmpty()) return {};
        if(params.length()<2) return {};

        auto d= ToDoubles(params);
        if(qIsNaN(d[0])||qIsNaN(d[1])) return {};
        return d[0]+d[1];
    }

    static QVariant sub(const QList<QVariant>& params){
        if(params.isEmpty()) return {};
        if(params.length()<2) return {};
        auto d= ToDoubles(params);
        if(qIsNaN(d[0])||qIsNaN(d[1])) return {};
        return d[0]-d[1];
    }

    static QVariant mul(const QList<QVariant>& params){
        if(params.isEmpty()) return {};
        if(params.length()<2) return {};
        auto d= ToDoubles(params);
        if(qIsNaN(d[0])||qIsNaN(d[1])) return {};
        return d[0]*d[1];
    }

    static QVariant div(const QList<QVariant>& params){
        if(params.isEmpty()) return {};
        if(params.length()<2) return {};
        auto d= ToDoubles(params);
        if(qIsNaN(d[0])||qIsNaN(d[1])) return {};
        return d[0]/d[1];
    }


    static QVariant opt(const QList<QVariant>& params){
        if(params.length() < 2) return {};
        bool okFlag = false;
        double flag = params[0].toDouble(&okFlag);
        if(!okFlag) return {};
        if(flag != 0.0)
            return params[1];
        return 0.0;
    }


    static QVariant qtyFixed(const QList<QVariant>& params) {
        if (params.size() < 2)
            return {};

        bool okQty = false;
        bool okN   = false;

        double qty = params[0].toDouble(&okQty);
        double n   = params[1].toDouble(&okN);

        if (!okQty || !okN)
            return {};

        return qty * n;
    }

    static QVariant qtyPerOrder(const QList<QVariant>& params) {
        if (params.size() < 1)
            return {};

        bool okN = false;
        double n = params[0].toDouble(&okN);
        if (!okN)
            return {};

        return n;
    }

    static QVariant qtyPerArea(const QList<QVariant>& params) {
        if (params.size() < 3)
            return {};

        bool okW = false, okH = false, okA = false;
        double w = params[0].toDouble(&okW);
        double h = params[1].toDouble(&okH);
        double a = params[2].toDouble(&okA);

        if (!okW || !okH || !okA || a == 0.0)
            return {};

        return (w * h) / a;
    }
};

