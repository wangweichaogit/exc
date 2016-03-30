#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_op.h"

#if 0
int GetCfgItem(char *pFileName /*in*/, char *pKey /*in*/, char * pValue/*in out*/, int * pValueLen /*out*/);
int WriteCfgItem(char *pFileName /*in*/, char *pItemName /*in*/, char *pItemValue/*in*/, int itemValueLen /*in*/);

#endif
//实现流程
//打开文件
//按照行的方式 循环读文件
//解析每一行，若匹配key关键字，在进行value值的提取
//提取value值需要 1去除前后空格 1级指针典型应用

#define  LineMaxLen 2048
#define  KeyMaxLen 64

int GetCfgItem(char *pFileName /*in*/, char *pKey /*in*/, char * pValue/*in out*/, int * pValueLen /*out*/)
{
	int		rv = 0;
	FILE	*fp = NULL;
	char	lineBuf[LineMaxLen];
	char	*pTmp = NULL, *pBegin = NULL, *pEnd = NULL;

	if (pFileName==NULL || pKey==NULL || pValue==NULL || pValueLen==NULL) 
	{
		rv = -1;
		printf("GetCfgItem() err. param err \n");
		goto End;
	}

	fp = fopen(pFileName, "r");
	if (fp == NULL)
	{
		rv = -2;
		printf("fopen() err. \n");
		goto End;
	}
	while (!feof(fp))
	{
		//读每一行
		memset(lineBuf, 0, sizeof(lineBuf));
		pTmp = fgets(lineBuf, LineMaxLen, fp);
		if (pTmp == NULL) 
		{
			break;
		}

		//不含=, 非配置项
		pTmp = strchr(lineBuf, '=');
		if (pTmp == NULL)
		{
			continue;
		}
		//key是否在本行
		pTmp = strstr(lineBuf, pKey);
		if (pTmp == NULL)
		{
			continue;
		}
		
		//调整到=右边，取value准备
		pTmp = strchr(lineBuf, '=');
		if (pTmp == NULL)
		{
			continue;
		}
		pTmp = pTmp + 1;

		//获取value 起点
		while (1) 
		{
			if (*pTmp == ' ')
			{
				pTmp ++ ;
			} 
			else
			{
				pBegin = pTmp;
				if (*pBegin == '\n')
				{
					//没有配置value
					printf("配置项:%s 没有配置value \n", pKey);
					goto End;
				}
				break;
			}
		}
		
		//获取valude结束点
		while (1) 
		{
			if ((*pTmp == ' ' || *pTmp == '\n'))
			{
				break;
			}
			else 
			{
				pTmp ++;
			}
		}
		pEnd = pTmp;

		//赋值
		*pValueLen = pEnd-pBegin;
		memcpy(pValue, pBegin, pEnd-pBegin);
		break;
	}

End:
	if (fp != NULL)
	{
		fclose(fp); 
	}
	
	return rv;
}

//实现流程
//循环读每一行，检查key配置项是否存在 若存在修改对应value值
//若不存在，在文件末尾 添加 "key = value"
//难点：如何修改文件流中的值

int WriteCfgItem(char *pFileName /*in*/, char *pKey /*in*/, char * pValue/*in*/, int ValueLen /*in*/)
{
	int		rv = 0, iTag = 0, length = 0;
	FILE	*fp = NULL;
	char	lineBuf[LineMaxLen];
	char	*pTmp = NULL, *pBegin = NULL, *pEnd = NULL;
	char	filebuf[1024*8] = {0};
	
	if (pFileName==NULL || pKey==NULL || pValue==NULL) 
	{
		rv = -1;
		printf("SetCfgItem() err. param err \n");
		goto End;
	}
	
	fp = fopen(pFileName, "r+");
	if (fp == NULL)
	{
		rv = -2;
		printf("fopen() err. \n");
		//goto End;
	}

	if (fp == NULL)
	{
		fp = fopen(pFileName, "w+t");
		if (fp == NULL)
		{
			rv = -3;
			printf("fopen() err. \n");
			goto End;
		}
	}
	
	fseek(fp, 0L, SEEK_END); 
	//获取文件长度;
	length = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	
	if (length > 1024*8) 
	{
		rv = -3;
		printf("文件超过1024*8, nunsupport");
		goto End;
	}
	
	while (!feof(fp))
	{
		//读每一行
		memset(lineBuf, 0, sizeof(lineBuf));
		pTmp = fgets(lineBuf, LineMaxLen, fp);
		if (pTmp == NULL) 
		{
			break;
		}
	
		//key关键字是否在本行
		pTmp = strstr(lineBuf, pKey);
		if (pTmp == NULL)
		{
			strcat(filebuf, lineBuf);
			continue;
		}
		else
		{
			sprintf(lineBuf, "%s = %s\n", pKey, pValue);
			strcat(filebuf, lineBuf);
			//若存在key
			iTag = 1; 
		}
	}

	//若不存在 追加
	if (iTag == 0) 
	{
		fprintf(fp, "%s = %s\n", pKey, pValue);
	}
	else //若存在
	{
		if (fp != NULL) 
		{ 
			fclose(fp); 
			fp = NULL; //避免野指针
		}

		fp = fopen(pFileName, "w+t");

		if (fp == NULL)
		{
			rv = -4;
			printf("fopen() err. \n");
			goto End;
		}
		fputs(filebuf, fp);
		//fwrite(filebuf, sizeof(char), strlen(filebuf), fp);
	}
	
End:
	if (fp != NULL)
	{
		fclose(fp); 
	}
	return rv;
}
