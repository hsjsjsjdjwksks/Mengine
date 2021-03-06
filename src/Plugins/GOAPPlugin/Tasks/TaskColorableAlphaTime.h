#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Colorable.h"

namespace Mengine
{
    class TaskColorableAlphaTime
        : public GOAP::TaskInterface
    {
    public:
        TaskColorableAlphaTime( const ColorablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time );
        ~TaskColorableAlphaTime() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        ColorablePtr m_colorable;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;

        float m_to;
        float m_time;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskColorableAlphaTime> TaskColorableAlphaTimePtr;
    //////////////////////////////////////////////////////////////////////////
}