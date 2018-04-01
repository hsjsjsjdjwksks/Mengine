#include "WIN32/WindowsIncluder.h"

#include "Win32Application.h"

//////////////////////////////////////////////////////////////////////////
//							Entry point									//
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( _In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{   
	// keep compiler happy
	UNREFERENCED_PARAMETER( hInstance );
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	UNREFERENCED_PARAMETER( nShowCmd );
	
	stdex_allocator_initialize();

#ifndef _DEBUG
	try
#endif
	{
		Mengine::Win32Application app;

		bool initialize = app.initialize();
    
		if( initialize == true )
		{
			app.loop();
		}
		else
		{
			MessageBoxA( NULL, "Mengine invalid initialization", "Mengine", MB_OK );
		}
    
		app.finalize();
	}
#ifndef _DEBUG
	catch( const std::exception & se )
	{		
		const char * se_what = se.what();

		MessageBoxA( NULL, se_what, "Mengine exception", MB_OK );
	}
#endif

	stdex_allocator_finalize();
	    
	return 0;
}
//////////////////////////////////////////////////////////////////////////