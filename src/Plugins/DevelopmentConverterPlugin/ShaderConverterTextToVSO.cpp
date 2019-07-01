#include "ShaderConverterTextToVSO.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ShaderConverterTextToVSO::ShaderConverterTextToVSO()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ShaderConverterTextToVSO::~ShaderConverterTextToVSO()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToVSO::_initialize()
    {
        m_convertExt = ".pso";

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToVSO::convert()
    {
        FilePath fxcPath = CONFIG_VALUE( "Engine", "FxcPath", "REGISTER"_fp );

        if( fxcPath == "REGISTER"_fp )
        {
            Char WindowsKitsInstallationFolder[256];
            PLATFORM_SERVICE()
                ->getLocalMachineRegValue( "SOFTWARE\\WOW6432Node\\Microsoft\\Windows Kits\\Installed Roots", "KitsRoot10", WindowsKitsInstallationFolder, 256 );

            PLATFORM_SERVICE()
                ->findFiles( "", WindowsKitsInstallationFolder, "x64\\fxc.exe", [&fxcPath]( const FilePath& _fp )
            {
                fxcPath = _fp;

                return false;
            } );
        }

        const ConstString & pakPath = m_options.fileGroup->getFolderPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        Char buffer[2048];
        sprintf( buffer, " /nologo /T vs_1_1 /O3 /Fo \"%s\" \"%s\""
            , full_output.c_str()
            , full_input.c_str()
        );

        LOGGER_MESSAGE( "converting file '%s' to '%s'\n%s"
            , full_input.c_str()
            , full_output.c_str()
            , buffer
        );

        if( PLATFORM_SERVICE()
            ->createProcess( fxcPath.c_str(), buffer ) == false )
        {
            LOGGER_ERROR( "invalid convert:\n%s"
                , buffer
            );

            return false;
        }

        return true;
    }
}