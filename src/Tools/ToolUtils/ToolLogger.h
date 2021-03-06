#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class ToolLogger
        : public LoggerBase
    {
    public:
        ToolLogger();
        ~ToolLogger() override;

    public:
        void log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size ) override;
    };
}