#pragma once

#include "common/registry/workflow/connection_workflow_policy.h"

// 🧩 BarcodeWorkflowPolicy
// - BarcodeRegistry workflow-ja
// - ConnectionWorkflowPolicy-ból származik
// - insert = registerNew
// - remove = retire
// - nincs update
// - domain logika a registry engine hookjaiban van

struct BarcodeWorkflowPolicy : ConnectionWorkflowPolicy
{
    // REGISTER NEW BARCODE
    template<typename Registry, typename Record>
    static bool registerNew(Registry& reg, const Record& r)
    {
        return run("barcode-register", [&]{
            beforeOperation(reg, "barcode-register");

            // Domain validációk (barcode-specifikus)
            if (!reg.validateBarcode(r))    return false;
            if (!reg.validateCollision(r))  return false;

            // Before hook
            if (!reg.beforeInsert(r)) return false;

            // CRUD (valójában: barcode add)
            reg.add(r);

            // After hook
            reg.afterInsert(r);

            // Log hook
            reg.onRegisterLog(r);

            // Persist
            persist(reg);

            afterOperation(reg, "barcode-register");
            return true;
        });
    }

    // RETIRE BARCODE
    template<typename Registry>
    static bool retire(Registry& reg, const QString& code, const QString& reason)
    {
        return run("barcode-retire", [&]{
            beforeOperation(reg, "barcode-retire");

            using Record = typename Registry::EntityType;

            // Find record by barcode
            const Record* found = reg.findByCode(code);
            if (!found)
                return false;

            Record copy = *found;

            // Before hook
            if (!reg.beforeRemove(copy)) return false;

            // CRUD (valójában: retire)
            if (!reg.retireInternal(code, reason)) return false;

            // After hook
            reg.afterRemove(copy);

            // Log hook
            reg.onRetireLog(copy, reason);

            // Persist
            persist(reg);

            afterOperation(reg, "barcode-retire");
            return true;
        });
    }

    // UPDATE → barcode esetén nincs értelme
    template<typename Registry, typename Record>
    static bool update(Registry&, const Record&)
    {
        static_assert(!std::is_same_v<Registry, Registry>,
                      "BarcodeWorkflowPolicy: update() not supported.");
        return false;
    }
};
