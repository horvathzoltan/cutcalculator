#pragma once
#include "test/common/test_module_base.h"
#include "products/model/product_master.h"
#include "test/common/test_data_builder.h"

class ProductRegistryTester : public TestModuleBase {
public:
    ProductRegistryTester()
        : TestModuleBase("ProductRegistry")
    {}

    bool run() override;

private:
    TestDataIds ids;

    void prepare();
    void clearCsv(const QString& path);

    void testInsert();
    void testUpdate();
    void testDelete();
};
