/*
base58编解码
g++ base58.cpp -o base58 -lcrypto
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <vector>
#include <iostream>
#include <openssl/bn.h> 
#include <openssl/sha.h> 
#include <algorithm>
#define DOMAIN_CHECK(c) ('0'<=(c)&&(c)<='9'||'a'<=(c)&&(c)<='f'||'A'<=(c)&&(c)<='F')  
const char * BASE58TABLE="123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const char* pszBase = "BCDEab467813A9cjkmnFMNPQRGHJKLSTUVdefghiW2XYZopqrstuvwxyz5";
using namespace std;
//网上的做法，编码后的长度不定
std::string base58encode(const std::string & hexstring) 
{ 	
	std::string result = "";
	 //BN_CTX *BN_CTX_new(void);申请一个新的上下文结构 
	BN_CTX * bnctx = BN_CTX_new(); 	
	//void BN_CTX_init(BN_CTX *c);将所有的项赋值为0，一般BN_CTX_init(&c)
	BN_CTX_init(bnctx); 
	//BIGNUM *BN_new(void);    新生成一个BIGNUM结构 
	BIGNUM * bn = BN_new(); 	
	BIGNUM * bn0= BN_new(); 	
	BIGNUM * bn58=BN_new(); 	
	BIGNUM * dv = BN_new(); 	
	BIGNUM * rem= BN_new(); 	
	//void BN_init(BIGNUM *);    初始化所有项均为0，一般为BN_ init(&c) 
	BN_init(bn);  	
	BN_init(bn0); 	
	BN_init(bn58); 	
	BN_init(dv); 	
	BN_init(rem);  
	/*
	*int BN_hex2bn(BIGNUM **a, const char *str);
	*16进制字符串转换为BIGNUM类型
	*/
	BN_hex2bn(&bn, hexstring.c_str()); 	
	BN_hex2bn(&bn58, "3a");
	
	/*
	*int BN_cmp(BIGNUM *a, BIGNUM *b);   -1 if a < b, 0 if a == b and 1 if a > b. 
	比较a，b的绝对值大小
	*/
	while(BN_cmp(bn, bn0)>0){ 	
		//	dv = bn/bn58； rem = bn % bn58； bnctx是上下文结构
		BN_div(dv, rem, bn, bn58, bnctx); 
		
		//BIGNUM *BN_copy(BIGNUM *a, const BIGNUM *b);
		//将b复制给a,正确返回a，错误返回NULL 		
		BN_copy(bn, dv); 		
		printf("dv: %s\n", BN_bn2dec(dv)); 		
		printf("rem:%s\n", BN_bn2dec(rem)); 	
		printf("word:%ld\n",BN_get_word(rem));	
		char base58char = BASE58TABLE[BN_get_word(rem)]; 		
		result += base58char; 	
		}  	
		//string 逆置
		std::string::iterator pbegin = result.begin(); 	
		std::string::iterator pend   = result.end(); 	
		while(pbegin < pend) { 		
					char c = *pbegin; 		
					*(pbegin++) = *(--pend); 		
					*pend = c; 	
		} 	
		return result; 
}  

/////////////////////////////////////////////////
// 转换为hex字符串
template<typename T>
std::string HexStr(const T itbegin, const T itend, bool fSpaces=false)
{
    std::string rv;
    static const char hexmap[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    rv.reserve((itend-itbegin)*3);
    for(T it = itbegin; it < itend; ++it)
    {
        unsigned char val = (unsigned char)(*it);
        if(fSpaces && it != itbegin)
            rv.push_back(' ');
        rv.push_back(hexmap[val>>4]);
        rv.push_back(hexmap[val&15]);
    }

    return rv;
}
int witchNum(char c)
{
	int num = 0;
	switch (c)
	{
		case '0': num = 0; break; 
		case '1': num = 1; break; 
		case '2': num = 2; break; 
		case '3': num = 3; break;
		case '4': num = 4; break; 
		case '5': num = 5; break; 
		case '6': num = 6; break; 
		case '7': num = 7; break;	
		case '8': num = 8; break; 
		case '9': num = 9; break; 
		case 'a': num = 10; break; 
		case 'b': num = 11; break;
		case 'c': num = 12; break; 
		case 'd': num = 13; break; 
		case 'e': num = 14; break; 
		case 'f': num = 15; break;	
	}
	return num;
}


//正常的转换
void hexStr2vch(string hexStr,std::vector<unsigned char> &vch)    
{      

		int i = 0 , j = 0;
    for (; i < hexStr.size(); )    
    {      
    			unsigned char num = witchNum(hexStr[i]) * 16 + witchNum(hexStr[i+1]);
          vch.push_back(num);
          i+=2;
          j++;
    }          
} 

//egd转换为hex时候逆置了，我们把它倒置回来
void hexStr2vch2(string hexStr,std::vector<unsigned char> &vch)    
{      

		int i = 0 , j = 0;
    for (i = hexStr.size()-1 ; i >= 0; )    
    {      
    			unsigned char num = witchNum(hexStr[i-1]) * 16 + witchNum(hexStr[i]);
          vch.push_back(num);
          i-=2;
          j++;
    }          
} 

std::string HexStr(const std::vector<unsigned char>& vch, bool fSpaces=false)
{
    return HexStr(vch.begin(), vch.end(), fSpaces);
}
std::string HexStr(const std::string instr, bool fSpaces=false)
{
	std::vector<unsigned char> vchData;
	int i = 0;
	for (i = 0 ; i < instr.size(); i++)
		vchData.push_back(instr[i]);
  return HexStr(vchData, fSpaces);
}
//////////////////////////////////////////////////  hash
template<typename T1>
void Hash(const T1 p1begin, const T1 p1end,unsigned char * hash2)
{
    static unsigned char pblank[1];
    unsigned char  hash1[32];
    SHA256((p1begin == p1end ? pblank : (unsigned char*)&p1begin[0]), (p1end - p1begin) * sizeof(p1begin[0]),hash1);
    SHA256(hash1, sizeof(hash1),hash2);
}
//bitcoin的base58编码

void setvch(const std::vector<unsigned char>& vch,BIGNUM * bn )
{
    std::vector<unsigned char> vch2(vch.size() + 4);
    unsigned int nSize = vch.size();
    // BIGNUM's byte stream format expects 4 bytes of
    // big endian size data info at the front
    vch2[0] = (nSize >> 24) & 0xff;
    vch2[1] = (nSize >> 16) & 0xff;
    vch2[2] = (nSize >> 8) & 0xff;
    vch2[3] = (nSize >> 0) & 0xff;
    // swap data to big endian
    reverse_copy(vch.begin(), vch.end(), vch2.begin() + 4);
    BN_mpi2bn(&vch2[0], vch2.size(), bn);
}
//egd supper的base58
std::string EncodeBase58Super(std::vector<unsigned char> vch)
{
	const unsigned char* pbegin = &vch[0];
	const unsigned char* pend = &vch[0] + vch.size();
	//开始base58编码
	std::vector<unsigned char> vchTmp(pend-pbegin+1, 0);
  reverse_copy(pbegin, pend, vchTmp.begin());
  
	BN_CTX * pctx = BN_CTX_new(); 	
	BN_CTX_init(pctx); 
	BIGNUM * bn = BN_new(); 	
	BIGNUM * bn0= BN_new(); 	
	BIGNUM * bn58=BN_new(); 	
	BIGNUM * dv = BN_new(); 	
	BIGNUM * rem= BN_new(); 	
	BN_init(bn);  	
	BN_init(bn0); 	
	BN_init(bn58); 	
	BN_init(dv); 	
	BN_init(rem); 
	
	BN_set_word(bn0,(long unsigned int)0);
	BN_set_word(bn58,(long unsigned int)58);
  setvch(vchTmp,bn);
  
 	std::string str;
 	str.reserve((pend - pbegin) * 138 / 100 + 1);
 
  while(BN_cmp(bn, bn0)>0){ 		
		BN_div(dv, rem, bn, bn58, pctx); 	
		BN_copy(bn, dv); 		
		char base58char = pszBase[BN_get_word(rem)]; 		
		str += base58char; 	
	} 
	
	for (const unsigned char* p = pbegin; p < pend && *p == 0; p++)
        str += pszBase[0];
	//  reserve
 	reverse(str.begin(), str.end());
	return str;
}
// egd 的base58
std::string EncodeBase58(std::vector<unsigned char> vchData)
{
	unsigned char nVersion = 33;
	//加一个前置 33
	std::vector<unsigned char > vch(1,nVersion);
	vch.insert(vch.end(),vchData.begin(),vchData.end());
	
	//取hash后四位
	unsigned char  hash[32];
	Hash(vch.begin(),vch.end(),hash);
	vch.insert(vch.end(),hash,hash + 4);

	const unsigned char* pbegin = &vch[0];
	const unsigned char* pend = &vch[0] + vch.size();
	//开始base58编码
	std::vector<unsigned char> vchTmp(pend-pbegin+1, 0);
  reverse_copy(pbegin, pend, vchTmp.begin());
  
	BN_CTX * pctx = BN_CTX_new(); 	
	BN_CTX_init(pctx); 
	BIGNUM * bn = BN_new(); 	
	BIGNUM * bn0= BN_new(); 	
	BIGNUM * bn58=BN_new(); 	
	BIGNUM * dv = BN_new(); 	
	BIGNUM * rem= BN_new(); 	
	BN_init(bn);  	
	BN_init(bn0); 	
	BN_init(bn58); 	
	BN_init(dv); 	
	BN_init(rem); 
	
	BN_set_word(bn0,(long unsigned int)0);
	BN_set_word(bn58,(long unsigned int)58);
  setvch(vchTmp,bn);
  
 	std::string str;
 	str.reserve((pend - pbegin) * 138 / 100 + 1);
 
  while(BN_cmp(bn, bn0)>0){ 		
		BN_div(dv, rem, bn, bn58, pctx); 	
		BN_copy(bn, dv); 		
		char base58char = BASE58TABLE[BN_get_word(rem)]; 		
		str += base58char; 	
	} 
	
	for (const unsigned char* p = pbegin; p < pend && *p == 0; p++)
        str += BASE58TABLE[0];
	//  reserve
 	reverse(str.begin(), str.end());
	return str;
}

//hextostring


/////////////////////////////////////////////
int main(int argc, char * argv []) 
{ 	
		if (argc != 2)
			{
				printf("input ./base58 string\n");
		}
		
		
		//TEST ckeyid to address
		#if 0
		std::string hexstring(argv[1]); 
					//fprintf(stdout, "%s\n", base58encode(hexstring).c_str()); 
		std::vector<unsigned char> vch;
	
		hexStr2vch2(hexstring,vch) ;
		cout<<EncodeBase58(vch).c_str()<<endl;	
		cout<<EncodeBase58(vch).size()<<endl;	
		#endif
		
		#if 1
		//test egd address to egdsuper address
		std::string egdstring(argv[1]); 
					
		std::vector<unsigned char> vch1(egdstring.begin()+1,egdstring.end());
		std::string straddr = EncodeBase58Super(vch1);
		straddr = "ES" + straddr;
		cout<<straddr.c_str()<<endl;
		#endif
		return 0; 
}

