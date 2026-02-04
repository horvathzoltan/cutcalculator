#pragma once

//#include "../core/registry_core.h"
#include "common/registry/mixins/crud_workflow_mixin.h"
#include "contract_checks.h"
#include <QString>
//#include "common/logger/logger.h"

template<typename Host, typename Entity>
struct CollectorMixin : protected CrudWorkflowMixin<Host, Entity> {
    CollectorContractChecks<Host, Entity> _contract_check;

public:
    using CrudWorkflowMixin<Host, Entity>::insertWithWorkflow;
    using CrudWorkflowMixin<Host, Entity>::updateWithWorkflow;
    // removeWithWorkflow NEM kerül kicsatornázásra → tiltva marad
};
