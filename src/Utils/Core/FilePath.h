#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"

#	include "stdex/array_string.h"

#	include <stdex/stl_vector.h>

#	include <stdint.h>

namespace Menge
{
#   ifndef MENGINE_MAX_PATH
#   define MENGINE_MAX_PATH 260
#   endif

#	define MENGINE_FILE_PATH_INVALID_ID	((uint32_t)-1)

	typedef ConstString FilePath;

	//typedef ConstString FilePath; //UTF8
	typedef stdex::array_string<MENGINE_MAX_PATH> PathString;

    typedef stdex::vector<FilePath> TVectorFilePath;

	namespace Helper
	{
		ConstString concatenationFilePath( ServiceProviderInterface * _serviceProvider, const ConstString & _left, const ConstString & _right );
		
		bool makeFullPath( ServiceProviderInterface * _serviceProvider, const ConstString & _fileGroupName, const ConstString & _fileName, ConstString & _fullPath );
	}
}  