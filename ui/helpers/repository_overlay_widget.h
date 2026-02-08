#pragma once

#include "ui/widgets/overlay_icon_widget.h"
#include "overlay_status_helper.h"
#include <QToolBar>

class QWidget;

template<typename T>
class RepositoryOverlayWidget : public OverlayIconWidget
{

public:
    RepositoryOverlayWidget(QToolBar* tb, const QString& objectName)
    {
        setBaseEmoji("📄"); // v2: unified document base emoji
        setObjectName(objectName);
        if(tb) tb->addWidget(this);
    }

    void refresh(int visible){
        int repo = T::instance().size();
        auto state = OverlayStatusHelper::computeState(repo, visible);
        OverlayStatusHelper::apply(this, state);
    }

public:
    void setState(OverlayStatusHelper::State s)
    {
        OverlayStatusHelper::apply(this, s);
    }

};
