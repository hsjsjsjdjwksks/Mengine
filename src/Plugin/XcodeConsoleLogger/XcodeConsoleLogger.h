#	pragma once

#	include "Interface/LoggerInterface.h"

#   include "Core/ServantBase.h"

#	include "Config/String.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class XcodeConsoleLogger
        : public ServantBase<LoggerInterface>
    {
    public:
		XcodeConsoleLogger();
        ~XcodeConsoleLogger();

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

    public:
        bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

    public:
        void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count ) override;
        void flush() override;

    protected:
        void createConsole_();
        void removeConsole_();

    protected:
        EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;
    };
}
