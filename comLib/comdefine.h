// defines.h: interface for the Cdefines class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFINES_H__CE8DC853_F125_4758_A264_94CA9A9E5B5F__INCLUDED_)
#define AFX_DEFINES_H__CE8DC853_F125_4758_A264_94CA9A9E5B5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
///////////////////////////////////////////////////////////////////////////////////
#ifndef WIN32
#	ifdef _WIN32
#		define WIN32
#	endif
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x501
#endif
#include <WinSock2.h>
#include <windows.h>
#include <direct.h>

#   ifdef _MSC_VER
//     '...' : forcing value to bool 'true' or 'false' (performance warning)
#      pragma warning( disable : 4800 )
//     ... identifier was truncated to '255' characters in the debug information
#      pragma warning( disable : 4786 )
//     'this' : used in base member initializer list
#      pragma warning( disable : 4355 )
//     class ... needs to have dll-interface to be used by clients of class ...
#      pragma warning( disable : 4251 )
//     ... : inherits ... via dominance
#      pragma warning( disable : 4250 )
//     non dll-interface class ... used as base for dll-interface class ...
#      pragma warning( disable : 4275 )
//      ...: decorated name length exceeded, name was truncated
#      pragma warning( disable : 4503 )  
#   endif

///////////////////////////////////////////////////////////////////////////////////
#else  //linux
///////////////////////////////////////////////////////////////////////////////////
typedef unsigned long       DWORD;
#ifndef __IOS__
typedef int                 BOOL;
#endif

#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>			//mkdir, access
#include <dirent.h>			//DIR, dirent, opendir, readdir, closedir
#include <string.h>
#ifdef __IOS__
#include "ios_generic.h"
#endif

#endif
///////////////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_DEFINES_H__CE8DC853_F125_4758_A264_94CA9A9E5B5F__INCLUDED_)
