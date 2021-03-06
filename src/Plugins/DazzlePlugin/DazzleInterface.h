#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Magic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MAGIC_NUMBER( MAGIC_AEZ, 'D', 'Z', 'Z', '1', 1 );
    //////////////////////////////////////////////////////////////////////////
    class UnknownDazzleEffectInterface
        : public UnknownInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceDazzleEffectInterface
        : public UnknownInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
}