#ifndef _BASE58_H_
#define _BASE58_H_

#include <string>
#include <vector>
#include <string.h>
#include <assert.h>

std::string EncodeBase58(const unsigned char* pbegin, const unsigned char* pend);
std::string EncodeBase58(const unsigned char* pbegin,int length);
std::string EncodeBase58(const std::vector<unsigned char>& vch);


bool DecodeBase58(const char* psz, std::vector<unsigned char>& vchRet);
bool DecodeBase58(const std::string& str, std::vector<unsigned char>& vchRet);
std::string DecodeBase58(const char* psz);
#endif