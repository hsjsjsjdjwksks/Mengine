#pragma once

#include <cstring>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRLEN
#define MENGINE_STRLEN(s) ::strlen(s)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRCMP
#define MENGINE_STRCMP(a, b) ::strcmp(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRCPY
#define MENGINE_STRCPY(a, b) ::strcpy(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRNCPY
#define MENGINE_STRNCPY(a, b, n) ::strncpy( a, b, n )
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRCAT
#define MENGINE_STRCAT(a, b) ::strcat(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRCHR
#define MENGINE_STRCHR(a, b) ::strchr(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRRCHR
#define MENGINE_STRRCHR(a, b) ::strrchr(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRNCMP
#define MENGINE_STRNCMP(a, b, n) ::strncmp(a, b, n)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRSTR
#define MENGINE_STRSTR(a, b) ::strstr(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WCSLEN
#define MENGINE_WCSLEN(s) ::wcslen(s)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WCSCHR
#define MENGINE_WCSCHR(s, c) ::wcschr(s, c)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WCSCPY
#define MENGINE_WCSCPY(a, b) ::wcscpy(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WCSCAT
#define MENGINE_WCSCAT(a, b) ::wcscat(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WCSCMP
#define MENGINE_WCSCMP(a, b) ::wcscmp(a, b)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MEMCPY
#define MENGINE_MEMCPY(a, b, n) ::memcpy(a, b, n)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MEMSET
#define MENGINE_MEMSET(d, v, s) ::memset(d, v, s)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_TOUPPER
#define MENGINE_TOUPPER(c) ::toupper(c)
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRICMP
#if defined(MENGINE_COMPILER_MSVC)
#   define MENGINE_STRICMP(a, b) _stricmp(a, b)
#elif defined(MENGINE_PLATFORM_LINUX) && defined(MENGINE_COMPILER_GCC)
#   define MENGINE_STRICMP(a, b) strcasecmp(a, b)
#else
#   define MENGINE_STRICMP(a, b) stricmp(a, b)
#endif
#endif
//////////////////////////////////////////////////////////////////////////