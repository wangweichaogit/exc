// TTIniFile_Imp.h: interface for the TTIniFile_Imp class.
// version: 2008-5-25 1.0
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TTINIFILE_IMP_H__756960FC_834A_45A8_B4E9_2FA2398A80FC__INCLUDED_)
#define AFX_TTINIFILE_IMP_H__756960FC_834A_45A8_B4E9_2FA2398A80FC__INCLUDED_


#ifdef _MSC_VER
#pragma warning(disable:4786)	//for VC6 only
#pragma warning(disable:4503)
#endif

#include "comLib/Mutex.h"
#include "comLib/Stdafx.h"

#include <string>
#include <map>
#include <stdlib.h>
#include <string.h>
using namespace std;

namespace DHT
{
class  COMLIBAPI IniFile
{
public:
	IniFile();
	~IniFile();
	bool	LoadIniFile(const char* szFileName);
	int		GetInteger(const char* szSection, const char* szKey, int nDefault);
	string	GetString(const char* szSection, const char* szKey, const char* szDefault);
	string	GetStringLowercase(const char* szSection, const char* szKey, const char* szDefault);
	bool	GetBool(const char* szSection, const char* szKey, bool bDefault);

	int		GetIntegerEx(const char* szSection, const char* keyPrefix, int index, int nDefault);
	string	GetStringEx(const char* szSection, const char* keyPrefix, int index, const char* szDefault);
	string	GetStringLowercaseEx(const char* szSection, const char* keyPrefix, int index, const char* szDefault);
	bool	GetBoolEx(const char* szSection, const char* keyPrefix, int index, bool bDefault);

	void	SetInteger(const char* szSection, const char* szKey, int value);
	void	SetString(const char* szSection, const char* szKey, const char* value);
	void	SetBool(const char* szSection, const char* szKey, bool bDefault);
	string  makeSec(const char* keyPrefix, int index) const;
	void    flush_to_file(const std::string& strFileName = "");
	void    flush_to_string(std::string& strConfigure);
	bool	get_a_section(const char* szSection,std::map<std::string, std::string> &section);
	bool	add_a_key_value(const std::string& strSection, const std::string& strP, const std::string& strV);
	bool	delete_a_key_value(const std::string& strSection, const std::string& strP);
	bool	is_section_key_exist(const char* szSection, const char* szKey);

private:
	typedef std::map<std::string, std::string> TSection;
	std::map<std::string, TSection*> m_Inifile;
	inline void clear();
	inline char getLowercase(char in);
	inline bool analyzeLine(std::string& strCurrSec, const char* szLine);
	inline void addASection(const std::string& strSection);
	inline void addAValue(const std::string& strSection, const std::string& strP, const std::string& strV);
	inline TSection* getSection(const char* szSection, bool is_need_create=false);
	inline const char* getValue(const TSection* pSec, const char* szP);
	Mutex m_mutex;
	std::string m_strFilePath;
	
};
} //namespace VOIP

#endif // !defined(AFX_TTINIFILE_IMP_H__756960FC_834A_45A8_B4E9_2FA2398A80FC__INCLUDED_)
