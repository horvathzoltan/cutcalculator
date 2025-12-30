#pragma once
#include <QString>

class IRegistryEntity {
public:
    virtual ~IRegistryEntity() = default;

    virtual QString displayName() const = 0;
    virtual QString shortId() const = 0;
    virtual QString toString() const = 0;
};
