#include "filecontext.h"
#include "filecontext_collector.h"

CsvImporter::FileContext::FileContext(const QString &operationName, const QString &filepath)
    : _filepath(filepath), _operationName(operationName) {}

CsvImporter::FileContext::~FileContext() {
    // Observer értesítés: automatikus gyűjtés
    FileContextCollector::instance().onContextDestroyed(*this);
}
