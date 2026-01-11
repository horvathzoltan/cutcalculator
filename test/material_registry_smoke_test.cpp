#include "test_manager.h"
#include "common/logger/logger.h"
#include <QUuid>
#include "materials/model/material_master.h"
#include "common/registry/base/registry_engine_base.h"
#include "common/registry/mixins/crud_mixin.h"

class TestMaterialRegistry
    : public RegistryEngineBase<MaterialMaster>,
      public CrudMixin<TestMaterialRegistry, MaterialMaster>
{
public:
    static TestMaterialRegistry& instance() {
        static TestMaterialRegistry inst;
        return inst;
    }

private:
    TestMaterialRegistry()
        : RegistryEngineBase<MaterialMaster>("TestMaterialRegistry", "MaterialMaster")
    {}
};


bool runMaterialRegistrySmokeTest() {
    zInfo() << "▶️ Running MaterialRegistry smoke test (non-QtTest runner)";

    auto& reg = TestMaterialRegistry::instance();
    reg.setAll({});

    MaterialMaster m;
    m.id = QUuid::createUuid();
    m.name = "TestMaterial";
    m.type = MaterialType(MaterialType::Type::Other);
    m.barcode = "ABC-123";

    if (!reg.insert(m)) {
        zError() << "Insert failed";
        return false;
    }

    const MaterialMaster* found = reg.findById(m.id);
    if (!found) {
        zError() << "Find after insert failed";
        return false;
    }
    if (found->name != QStringLiteral("TestMaterial")) {
        zError() << "Name mismatch after insert";
        return false;
    }

    MaterialMaster updated = *found;
    updated.name = "TestMaterial v2";
    if (!reg.update(updated)) {
        zError() << "Update failed";
        return false;
    }

    const MaterialMaster* found2 = reg.findById(m.id);
    if (!found2) {
        zError() << "Find after update failed";
        return false;
    }
    if (found2->name != QStringLiteral("TestMaterial v2")) {
        zError() << "Name mismatch after update";
        return false;
    }

    if (!reg.remove(m.id)) {
        zError() << "Remove failed";
        return false;
    }

    const MaterialMaster* found3 = reg.findById(m.id);
    if (found3) {
        zError() << "Entity still present after remove";
        return false;
    }

    zInfo() << "✅ MaterialRegistry smoke test PASSED";
    return true;
}
