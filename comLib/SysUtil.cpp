// SysUtil.cpp: implementation of the SysUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "comLib/comdefine.h"
#include "comLib/SysUtil.h"

#include <sstream>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#   include <io.h>
#   include <direct.h>
#	include <tchar.h>
#else
#   include <unistd.h>
#   include <dirent.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace DHT
{
bool SysUtil::CreateLongDirectory(const std::string& strDirectory)
{
	int index1 = -1, index2 = -1;
	std::string sNew;
	std::string slashset="/\\";
	while(1)
	{
		index2 = index1 + 1;
		index1 = strDirectory.find_first_of(slashset,index2);
		if(index1 == (int)std::string::npos)
			break;
		if(index1 == 0)
			continue;
		if(strDirectory[index1 - 1] == ':')
			continue;
		sNew = strDirectory.substr(0, index1);
#ifdef WIN32
		_mkdir(sNew.c_str());    // CreateDirectory(sNew.c_str() ,NULL);
#else
        mkdir(sNew.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
	}
	return true;
}

void SysUtil::SleepMillisecond(unsigned long millisecond)
{
#ifdef WIN32
	Sleep(millisecond);
#else
#ifndef _AIX
    struct timeval tm;
    int sec = 0;
    
    sec=(int) millisecond / 1000;
    if (sec > 0) {
        tm.tv_sec = sec;
        tm.tv_usec = 0;
    } else {
        tm.tv_sec = 0;
        tm.tv_usec = millisecond * 1000; //—” ±∫¡√Î
    }
    select(0,NULL,NULL,NULL,&tm);
#else
	usleep(millisecond * 1000);
#endif
#endif
}

#ifdef _LINUX_
void SysUtil::SleepUSecond(unsigned long usecond)
{//SleepŒ¢√Îº∂
    struct timeval tm;    
    tm.tv_sec = 0; 
    tm.tv_usec = usecond; //—” ±Œ¢√Î 
    select(0,NULL,NULL,NULL,&tm);
}
#endif

void SysUtil::SleepSecond(unsigned long second)
{
#ifdef WIN32
	Sleep(second * 1000);
#else
#ifndef _AIX
    struct timeval tm;    
    tm.tv_sec = second; 
    tm.tv_usec = 0; //—” ±∫¡√Î 
    select(0,NULL,NULL,NULL,&tm);
#else
	sleep(second);
#endif
#endif
}

//Code taken from CCOD UtilLib
std::string SysUtil::util_itoa(int number)
{
	std::ostringstream s;
	s << number;

	return s.str();
}

UTIL_RESULT SysUtil::util_is_file_writable(const std::string& filename)
{
	UTIL_RESULT res = UTIL_SUCCESS;
#ifdef WIN32
	if ( _access(filename.c_str(), 2) != 0) {
#else
	if ( access(filename.c_str(), W_OK) != 0) {
#endif
		if (errno == EACCES)
			res = UTIL_PERMISSION_DENIED;
		if (errno == ENOENT)
			res = UTIL_FILE_NOT_EXISTS;
	}    

	return res;
}

bool SysUtil::util_is_file_exist(const std::string& filename)
{
	bool bRet = false;
#ifdef WIN32
	bRet = (_access(filename.c_str(), 0) == 0);
#else
	bRet = (access(filename.c_str(), F_OK)  == 0);
#endif
	return bRet;
}

UTIL_RESULT SysUtil::util_delete_file(const std::string& pathname)
{
	UTIL_RESULT res = UTIL_SUCCESS;
	if ( (res = util_is_file_writable(pathname)) != UTIL_SUCCESS)
		return res;

#ifdef WIN32
	if (_unlink(pathname.c_str()) != 0)
#else
	if (unlink(pathname.c_str()) != 0)
#endif
	{
		if (errno == EACCES)
			res = UTIL_PERMISSION_DENIED;
		else
			res = UTIL_FILE_NOT_EXISTS;
	}

	return res;
}

std::string SysUtil::util_path_cat(const std::string& dir,const std::string& filename, const char slash)
{
	std::string res(dir);
	if (res[res.size()-1] != slash)
		res += slash;
	res.append(filename);

	return res;
}

UTIL_RESULT SysUtil::util_list_dir(const std::string& dir, std::vector<std::string>& filelist)
{
	UTIL_RESULT res = UTIL_SUCCESS;

	filelist.clear();

#ifdef WIN32
	WIN32_FIND_DATA data;
#ifdef _UNICODE
	HANDLE handle = FindFirstFile(s2ws(dir + "\\*.*").c_str(), &data);
#else
	HANDLE handle = FindFirstFileA((dir + "\\*.*").c_str(), &data);
#endif
	if (handle != INVALID_HANDLE_VALUE) {
		do {
			// '..', '.' should not be included
			if ( (_tcscmp(data.cFileName, _T("..")) != 0) &&
				(_tcscmp(data.cFileName, _T(".")) != 0) )
#ifdef _UNICODE
				filelist.push_back(ws2s(data.cFileName));
#else
				filelist.push_back(data.cFileName);
#endif
		} while (FindNextFile(handle, &data));

		if (GetLastError() != ERROR_NO_MORE_FILES)
			res = UTIL_GENERAL_FAIL;
		FindClose(handle);
	} else {
		res = UTIL_GENERAL_FAIL;
	}
#else
	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(dir.c_str())) == NULL) {
		res = UTIL_GENERAL_FAIL;
	} else {
		while ((dirp = readdir(dp)) != NULL) {
			// '..', '.' should not be included
			if ( (strcmp(dirp->d_name, "..") != 0) &&
				(strcmp(dirp->d_name, ".") != 0) )
				filelist.push_back(std::string(dirp->d_name));
		}
		closedir(dp);
	}
#endif

	return res;
}

UTIL_RESULT SysUtil::util_rename(const std::string& ofile, const std::string& nfile)
{
	UTIL_RESULT res = UTIL_SUCCESS;
	if ( (res = util_is_file_writable(ofile)) != UTIL_SUCCESS)
		return res;

	if (rename(ofile.c_str(), nfile.c_str()) != 0) {
		res = UTIL_GENERAL_FAIL;
		printf("error:%d\n",errno);
	}

	return res;
}

std::vector<std::string> SysUtil::util_strsplit(const std::string& str, const char c)
{
	std::vector<std::string> v;

	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(c, 0);
	// Find first "non-delimiter".
	std::string::size_type pos     = str.find_first_of(c, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		v.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(c, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(c, lastPos);
	}

	return v;

}

std::string SysUtil::util_strtrim(const std::string& str)
{
	if (str.empty())
		return str;

	std::string s;
	s.assign(str);
	std::string::iterator iter;
	for (iter = s.begin(); iter != s.end();) {
		if ( (*iter == ' ') || (*iter == '\t') ) {
			iter = s.erase(iter);
		} else {
			break;
		}
	}
	for (iter = s.end() - 1; iter != s.begin(); --iter)
		if ((*iter != ' ') && (*iter != '\t'))
			break;
	++iter;
	for (;iter != s.end();) {
		if (*iter == ' ' || *iter == '\t') {
			iter = s.erase(iter);
		} else {
			break;
		}
	}

	return s;
}

std::string SysUtil::ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest,0,_Dsize);
#ifdef _ANDROID_DHN_
	wcsrtombs(_Dest,&_Source,_Dsize,NULL);
#else
	wcstombs(_Dest,_Source,_Dsize);
#endif
	std::string result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

std::wstring SysUtil::s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs"); 
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
#ifdef _ANDROID_DHN_
	mbsrtowcs(_Dest,&_Source,_Dsize,NULL);
#else
	mbstowcs(_Dest,_Source,_Dsize);
#endif
	std::wstring result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

std::string SysUtil::util_get_cwd()
{
	char buf[256];
#ifdef WIN32
	_getcwd( buf, sizeof(buf) );
#else
	getcwd( buf, sizeof(buf) );
#endif

	return buf;
}

size_t SysUtil::util_get_file_size(const std::string& filename)
{
	int fd, result;
#ifdef WIN32
	struct __stat64 buf;
	if ( (fd = _open( filename.c_str(), _O_RDONLY)) < 0 )
		return 0;
	if ( (result = _fstat64( fd, &buf )) != 0)
		return 0;

	_close(fd);
#else
	struct stat buf;
	if ( (fd = open( filename.c_str(), O_RDONLY)) < 0 )
		return 0;
	if ( (result = fstat( fd, &buf )) != 0)
		return 0;

	close(fd);
#endif

	return (size_t)buf.st_size;
}

} //namespace VOIP
