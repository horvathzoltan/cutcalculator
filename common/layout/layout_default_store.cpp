#include "common/layout/layout_default_store.h"

LayoutDefaultStore& LayoutDefaultStore::instance()
{
    static LayoutDefaultStore inst;
    return inst;
}

QString LayoutDefaultStore::windowGeometryPercent() const
{
    return SettingsManager::instance().windowGeometryPercent();
}

void LayoutDefaultStore::setWindowGeometryPercent(const QString& s)
{
    SettingsManager::instance().setWindowGeometryPercent(s);
}

QString LayoutDefaultStore::screenSizeString() const
{
    return SettingsManager::instance().screenSizeString();
}

void LayoutDefaultStore::setScreenSizeString(const QString& s)
{
    SettingsManager::instance().setScreenSizeString(s);
}

QString LayoutDefaultStore::mainSplitterPercent() const
{
    return SettingsManager::instance().mainSplitterPercent();
}

void LayoutDefaultStore::setMainSplitterPercent(const QString& s)
{
    SettingsManager::instance().setMainSplitterPercent(s);
}

QString LayoutDefaultStore::productTypesSplitterPercent() const
{
    return SettingsManager::instance().productTypesSplitterPercent();
}

void LayoutDefaultStore::setProductTypesSplitterPercent(const QString& s)
{
    SettingsManager::instance().setProductTypesSplitterPercent(s);
}

QString LayoutDefaultStore::leftVerticalSplitterPercent() const
{
    return SettingsManager::instance().leftVerticalSplitterPercent();
}

void LayoutDefaultStore::setLeftVerticalSplitterPercent(const QString& s)
{
    SettingsManager::instance().setLeftVerticalSplitterPercent(s);
}

QString LayoutDefaultStore::rightVerticalSplitterPercent() const
{
    return SettingsManager::instance().rightVerticalSplitterPercent();
}

void LayoutDefaultStore::setRightVerticalSplitterPercent(const QString& s)
{
    SettingsManager::instance().setRightVerticalSplitterPercent(s);
}

QString LayoutDefaultStore::productTreeHeaderPercent() const
{
    return SettingsManager::instance().productTreeHeaderPercent();
}

void LayoutDefaultStore::setProductTreeHeaderPercent(const QString& s)
{
    SettingsManager::instance().setProductTreeHeaderPercent(s);
}

QString LayoutDefaultStore::formulaEditorGeometryPercent() const
{
    return SettingsManager::instance().formulaEditorGeometryPercent();
}

void LayoutDefaultStore::setFormulaEditorGeometryPercent(const QString& s)
{
    SettingsManager::instance().setFormulaEditorGeometryPercent(s);
}

void LayoutDefaultStore::flush()
{
    SettingsManager::instance().save();
}
