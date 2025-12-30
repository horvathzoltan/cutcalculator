#pragma once
#include "entity.h"
#include "iregistry_entity.h"

#include <QUuid>

struct IdentifiableEntity : public Entity, public IRegistryEntity {
    QUuid id;
    QString name;

    QString displayName() const override {
        return name;
    }

    QString shortId() const override {
        return id.toString(QUuid::WithoutBraces).left(8);
    }

    QString toString() const override {
        return QString("IdentifiableEntity{id=%1, name=%2}")
        .arg(id.toString(), name);
    }
};
