#pragma once
#include <QUuid>

struct TestDataIds {
    QUuid P1;
    QUuid M1;
    QUuid M2;
};

class TestDataBuilder {
public:
    static TestDataIds prepareStandard();
};
