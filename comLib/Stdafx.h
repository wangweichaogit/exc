#ifndef	__STDAFX_H__
#define __STDAFX_H__

#ifdef WIN32
	#ifdef COMLIB_EXPORTS
		#define COMLIBAPI __declspec(dllexport)
	#else
		#define COMLIBAPI __declspec(dllimport)
	#endif
#else
	#define COMLIBAPI
#endif

#endif // __STDAFX_H__
