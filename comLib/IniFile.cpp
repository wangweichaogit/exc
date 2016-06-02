// TTIniFile_Imp.cpp: implementation of the TTIniFile_Imp class.
// version: 2009-11-6 1.1
//////////////////////////////////////////////////////////////////////
#include "comLib/IniFile.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace DHT
{

IniFile::IniFile()
{

}

IniFile::~IniFile()
{
	clear();
}

int IniFile::GetInteger(const char* szSection, const char* szKey, int nDefault)
{
	SingleMutex s(m_mutex);
	TSection* p = getSection(szSection);
	if(!p)
		return nDefault;
	const char* szValue = getValue(p, szKey);
	if(!szValue)
		return nDefault;
    else if ('0' == szValue[0]
        &&('x'==szValue[1]||'X'==szValue[1]))
    {
        int nRet=0;
        sscanf(szValue,"%x",&nRet);
        return nRet;
    }
	return atoi(szValue);
}

std::string IniFile::GetString(const char* szSection, const char* szKey, const char* szDefault)
{
	SingleMutex s(m_mutex);
	TSection* p = getSection(szSection);
	if(!p)
		return szDefault;
	const char* szValue = getValue(p, szKey);
	if(!szValue)
		return szDefault;
	return szValue;
}

std::string IniFile::GetStringLowercase(const char* szSection, const char* szKey, const char* szDefault)
{
	SingleMutex s(m_mutex);
	TSection* p = getSection(szSection);
	if(!p)
		return szDefault;
	const char* szValue = getValue(p, szKey);
	if(!szValue)
		return szDefault;
	std::string str;
	while(*szValue)
	{
		str += getLowercase(*szValue);
		++szValue;
	}
	return str;
}

bool IniFile::GetBool(const char* szSection, const char* szKey, bool bDefault)
{
	SingleMutex s(m_mutex);
	std::string str = GetStringLowercase(szSection, szKey, bDefault?"yes":"no");
	return "yes"==str || "true"==str;
}

int	IniFile::GetIntegerEx(const char* szSection, const char* keyPrefix, int index, int nDefault)
{
	SingleMutex s(m_mutex);
	return GetInteger(szSection, makeSec(keyPrefix, index).c_str(), nDefault);
}

std::string IniFile::GetStringEx(const char* szSection, const char* keyPrefix, int index, const char* szDefault)
{
	SingleMutex s(m_mutex);
	return GetString(szSection, makeSec(keyPrefix, index).c_str(), szDefault);
}

std::string IniFile::GetStringLowercaseEx(const char* szSection, const char* keyPrefix, int index, const char* szDefault)
{
	SingleMutex s(m_mutex);
	return GetStringLowercase(szSection, makeSec(keyPrefix, index).c_str(), szDefault);
}

bool IniFile::GetBoolEx(const char* szSection, const char* keyPrefix, int index, bool bDefault)
{
	SingleMutex s(m_mutex);
	return GetBool(szSection, makeSec(keyPrefix, index).c_str(), bDefault);
}

string IniFile::makeSec(const char* keyPrefix, int index) const
{
	std::ostringstream s;
	s<<keyPrefix<<index;
	return s.str();
}

inline void IniFile::clear()
{
	SingleMutex s(m_mutex);
	std::map<std::string, TSection*>::iterator iter = m_Inifile.begin();
	for(; iter != m_Inifile.end(); ++iter)
	{
		TSection* p = iter->second;
		delete p;
	}
	m_Inifile.clear();
}

inline IniFile::TSection* IniFile::getSection(const char* szSection, bool is_need_create)
{
	SingleMutex s(m_mutex);
	std::string strSec;
	while(*szSection)
	{
		strSec += getLowercase(*szSection);
		szSection++;
	}
	
	std::map<std::string, TSection*>::iterator iter = m_Inifile.find(strSec);
	if(iter == m_Inifile.end())
	{
		if (!is_need_create)
			return 0;

		std::pair<std::map<std::string, TSection*>::iterator, bool> result = m_Inifile.insert(make_pair(strSec,new TSection));
		if (result.second)
			return result.first->second;
		else
			return 0;
	}
	else
	{
		return iter->second;
	}
}

inline const char* IniFile::getValue(const TSection* pSec, const char* szP)
{
	SingleMutex s(m_mutex);
	std::string strP;
	while(*szP)
	{
		strP += getLowercase(*szP);
		szP++;
	}

	TSection::const_iterator iter = pSec->find(strP);
	if(iter == pSec->end())
		return 0;
	const std::string& str = iter->second;
	return str.c_str();
}

inline char IniFile::getLowercase(char in)
{
	if(in>='A' && in<='Z')
		return in - 'A' + 'a';
	return in;
}

inline void IniFile::addASection(const std::string& strSection)
{
	SingleMutex s(m_mutex);
	//only add the first one
	if(m_Inifile.find(strSection) != m_Inifile.end())
		return;

	m_Inifile[strSection] = new TSection;
}

inline void IniFile::addAValue(const std::string& strSection, const std::string& strP, const std::string& strV)
{
	SingleMutex s(m_mutex);
	std::map<std::string, TSection*>::iterator iter =  m_Inifile.find(strSection);
	if(iter == m_Inifile.end())
		return;
	TSection* pSec = iter->second;

	//only do add action on the 1st time
	if(pSec->find(strP) != pSec->end())
		return;
	(*pSec)[strP] = strV;
//	std::cout<<strSection << " "<< strP<< "="<<strV<<"<"<<std::endl;
}

bool IniFile::LoadIniFile(const char* szFileName)
{
	SingleMutex s(m_mutex);
	clear();
	std::ifstream inif(szFileName);
//	if(!inif.good())	//AIX5.3下不能使用该判断，也不能使用if(!inif)——可能是C++运行库版本问题
	if(!inif.is_open())
		return false;
	m_strFilePath = szFileName;
	const int CFG_LINE_LEN = 1024;
	char line[CFG_LINE_LEN];
	std::string strCurrSection;
	while(!inif.eof())
	{
		inif.getline(line, CFG_LINE_LEN);
		if(strlen(line) == 0)
			continue;
		if(!analyzeLine(strCurrSection, line))
			break;
	}

	inif.close();
	return true;
}

//special marks in a inifile: 
//[ ] # ; ' " = space
inline bool IniFile::analyzeLine(std::string& strCurrSec, const char* szLine)
{
	char* pEqMark = strchr((char*)szLine, '=');
	if(!pEqMark)	//may be a section name
	{
		std::string strSec;
		bool bSection = false;
		for(int i=0; ;++i)
		{
			switch(szLine[i])
			{
			case ' ':
			case '\t':
				break;				//spaces in section name will be skipped;
			case '[':
				if(bSection)		//like: "[x["
					return true;
				bSection = true;
				break;
			case ']':
				if(strSec.length())
				{
					strCurrSec = strSec;	//set a new current section
					addASection(strSec);
				}
				return true;		//like: "]"
			//finish marks.
			case '#':
			case ';':
			case '\r':				//on linux \r is the last character in a line
			case 0:
				return true;
			default:
				if(bSection)
					strSec += getLowercase(szLine[i]);
				else				//like: "a["
					return true;
				break;
			}
		}
	}
	else
	{
		if(!strCurrSec.length())
			return true;

		const char* p = szLine;
		//get parameter 1st
		std::string strPara;
		while(p != pEqMark)
		{
			switch(*p)
			{
			case ' ':
			case '\t':
				break;
			case '#':
			case ';':
			case '\r':
			case 0:
				return true;
			//invalid characters.
			case '[':
			case ']':
			case '\"':
			case '\'':
				return true;
			default:
				{
					if (strCurrSec == "domain_isp_map")
						strPara += (*p);
					else
						strPara += getLowercase(*p);
				}	
			}
			++p;
		}
		if(!strPara.length())
			return true;

		++p;	//skip the '=' mark
		std::string strValue;
		bool bInBarcket = false;	//is there a ' or a "?
		for(;;)
		{
			switch(*p)
			{
			case ' ':
				if(bInBarcket)
					strValue += *p;
				break;
			case '\'':
			case '\"':
				if(bInBarcket)		//finished
				{
					addAValue(strCurrSec, strPara, strValue);
					return true;
				}
				else
					bInBarcket = true;
				break;
			case '#':
			case ';':
			case '\r':
			case 0:
				addAValue(strCurrSec, strPara, strValue);
				return true;
			default:
				strValue += *p;
			}
			++p;
		}
		return true;
	}
}

void IniFile::flush_to_file(const std::string& strFileName)
{
	SingleMutex s(m_mutex);

	int nKeyLength = 21;
	std::string strEndOfLine = "\n";

	std::string strFlushFileName = strFileName;
	if (strFlushFileName.empty())
	{
		strFlushFileName = "Host.ini";
	}

	FILE *pFile = fopen(strFlushFileName.c_str(),"w+");
	fputs("#Host configure file",pFile);
	fputs(strEndOfLine.c_str(),pFile);
	fputs(strEndOfLine.c_str(),pFile);
	
	std::map<std::string, TSection*>::iterator iter1 = m_Inifile.begin();
	for (;iter1 != m_Inifile.end();iter1++)
	{
		std::string strSection = "[";
		strSection += iter1->first.c_str();
		strSection += "]";
		fputs(strSection.c_str(),pFile);
		fputs(strEndOfLine.c_str(),pFile);
		std::map<std::string, std::string>::iterator iter2 = iter1->second->begin();
		for (;iter2 != iter1->second->end();iter2++)
		{
			std::string strLine = iter2->first.c_str();
			int nSpaceNum = (int)(nKeyLength-strLine.length());
			for (int i = 0;i < nSpaceNum;i++)
			{
				strLine += " ";
			}

			strLine += "= ";
			strLine += iter2->second.c_str();
			fputs(strLine.c_str(),pFile);
			fputs(strEndOfLine.c_str(),pFile);
		}

		fputs(strEndOfLine.c_str(),pFile);
	}
	fclose(pFile);

}

void IniFile::flush_to_string(std::string& strConfigure)
{
	SingleMutex s(m_mutex);

	int nKeyLength = 21;
	std::string strEndOfLine = "\n";

	strConfigure += "#Host version 1.0.0.1";
	strConfigure += strEndOfLine;
	strConfigure += "#2014.04.18";;
	strConfigure += strEndOfLine;
	strConfigure += strEndOfLine;

	std::map<std::string, TSection*>::iterator iter1 = m_Inifile.begin();
	for (;iter1 != m_Inifile.end();iter1++)
	{
		std::string strSection = "[";
		strSection += iter1->first.c_str();
		strSection += "]";
		strConfigure += strSection;
		strConfigure += strEndOfLine;
		std::map<std::string, std::string>::iterator iter2 = iter1->second->begin();
		for (;iter2 != iter1->second->end();iter2++)
		{
			std::string strLine = iter2->first.c_str();
			int nSpaceNum = (int)(nKeyLength-strLine.length());
			for (int i = 0;i < nSpaceNum;i++)
			{
				strLine += " ";
			}

			strLine += "= ";
			strLine += iter2->second.c_str();
			strConfigure += strLine;
			strConfigure += strEndOfLine.c_str();
		}

		strConfigure += strEndOfLine;
	}
}

void IniFile::SetInteger(const char* szSection, const char* szKey, int value)
{
	SingleMutex s(m_mutex);
	int nRet = 0;
	do 
	{
		if (szSection == NULL || szKey == NULL)
		{
			nRet = 1;
			break;
		}

		TSection* p = getSection(szSection,true);
		if (p == NULL)
		{
			nRet = 2;
			break;
		}

		//TSection::iterator iter = p->find(szKey);
		//if (iter == p->end())
		//{
		//	nRet = 3;
		//	break;
		//}

		char szValue[1024] = {0};
#ifdef WIN32
		_snprintf(szValue,1024,"%d",value);
#else
		snprintf(szValue,1024,"%d",value);
#endif
		(*p)[szKey] = szValue;
		
	} while (0);
}

void IniFile::SetString(const char* szSection, const char* szKey, const char* value)
{
	SingleMutex s(m_mutex);
	int nRet = 0;
	do 
	{
		if (szSection == NULL || szKey == NULL || value == NULL || strlen(value) == 0)
		{
			nRet = 1;
			break;
		}

		TSection* p = getSection(szSection, true);
		if (p == NULL)
		{
			nRet = 2;
			break;
		}

// 		TSection::iterator iter = p->find(szKey);
// 		if (iter == p->end())
// 		{
// 			nRet = 3;
// 			break;
// 		}

		(*p)[szKey] = value;

	} while (0);
}

void IniFile::SetBool(const char* szSection, const char* szKey, bool bValue)
{
	SingleMutex s(m_mutex);
	int nRet = 0;
	do 
	{
		if (szSection == NULL || szKey == NULL)
		{
			nRet = 1;
			break;
		}

		TSection* p = getSection(szSection,true);
		if (p == NULL)
		{
			nRet = 2;
			break;
		}

		//TSection::iterator iter = p->find(szKey);
		//if (iter == p->end())
		//{
		//	nRet = 3;
		//	break;
		//}

		(*p)[szKey] = bValue?"yes":"no";

	} while (0);
}

bool IniFile::get_a_section(const char* szSection,std::map<std::string, std::string> &section)
{
	SingleMutex s(m_mutex);
	TSection* p = getSection(szSection);
	if(!p)
		return false;
	section = *p;
	return true;
}

bool IniFile::add_a_key_value(const std::string& strSection, const std::string& strP, const std::string& strV)
{
	SingleMutex s(m_mutex);
	std::map<std::string, TSection*>::iterator iter =  m_Inifile.find(strSection);
	if(iter == m_Inifile.end())
		return false;
	TSection* pSec = iter->second;

	(*pSec)[strP] = strV;
	return true;
}

bool IniFile::delete_a_key_value(const std::string& strSection, const std::string& strP)
{
	SingleMutex s(m_mutex);
	std::map<std::string, TSection*>::iterator iter =  m_Inifile.find(strSection);
	if(iter == m_Inifile.end())
		return false;
	TSection* pSec = iter->second;
	std::map<std::string,std::string>::iterator iter1 = pSec->find(strP);
	if (iter1 != pSec->end())
	{
		pSec->erase(iter1);
	}
	return true;
}

bool IniFile::is_section_key_exist(const char* szSection, const char* szKey)
{
	SingleMutex s(m_mutex);
	bool bRet = true;
	do 
	{
		if (szSection == NULL || szKey == NULL)
		{
			bRet = false;
			break;
		}

		TSection* p = getSection(szSection);
		if (p == NULL)
		{
			bRet = false;
			break;
		}

		TSection::iterator iter = p->find(szKey);
		if (iter == p->end())
		{
			bRet = false;
			break;
		}
	}while(0);
	return bRet;
}

} //namespace comLib
