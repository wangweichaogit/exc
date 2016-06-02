// SysUtil.h: interface for the SysUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSUTIL_H__413D2D24_ED1A_4D82_AA1D_29E0F0679B8E__INCLUDED_)
#define AFX_SYSUTIL_H__413D2D24_ED1A_4D82_AA1D_29E0F0679B8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef unsigned short  UTIL_RESULT;

#define UTIL_SUCCESS            0
#define UTIL_GENERAL_FAIL       1
#define UTIL_INVALID_NAME       2
#define UTIL_FILE_NOT_EXISTS    3
#define UTIL_PERMISSION_DENIED  4

#include <string>
#include <vector>
#include "comLib/Stdafx.h"

namespace DHT
{

class COMLIBAPI SysUtil  
{
public:
	static bool CreateLongDirectory(const std::string& strDirectory);
	static void SleepMillisecond(unsigned long millisecond);
	static void SleepSecond(unsigned long second);
#ifdef _LINUX_
    static void SleepUSecond(unsigned long usecond);
#endif

	//Code taken from CCOD UtilLib
	static std::string util_itoa(int number);
	static UTIL_RESULT util_is_file_writable(const std::string& filename);
	static bool util_is_file_exist(const std::string& filename);
	static UTIL_RESULT util_delete_file(const std::string& pathname);
	static std::string util_path_cat(const std::string& dir,const std::string& filename, const char slash);
	static UTIL_RESULT util_list_dir(const std::string& dir, std::vector<std::string>& filelist);
	static UTIL_RESULT util_rename(const std::string& ofile, const std::string& nfile);
	static std::vector<std::string> util_strsplit(const std::string& str, const char c);
	static std::string util_strtrim(const std::string& str);
	static std::wstring s2ws(const std::string& s);
	static std::string ws2s(const std::wstring& ws);
	static std::string util_get_cwd();
	static size_t util_get_file_size(const std::string& filename);

};
} //namespace VOIP

#endif // !defined(AFX_SYSUTIL_H__413D2D24_ED1A_4D82_AA1D_29E0F0679B8E__INCLUDED_)
