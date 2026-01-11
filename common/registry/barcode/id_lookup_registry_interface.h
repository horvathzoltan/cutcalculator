#pragma once
#include "common/model/identifiable_entity.h"

class IdLookupRegistryInterface {
public:
    virtual ~IdLookupRegistryInterface() = default;
    virtual const IdentifiableEntity* findEntityById(const QUuid&) const = 0;
};
