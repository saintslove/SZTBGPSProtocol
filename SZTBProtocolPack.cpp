/*
 * SZTBProtocolPack.cpp
 *
 *  Created on: 2016年8月27日
 *      Author: wangqi
 */

#include "SZTBProtoPack.h"


using namespace std;

void TranscodOut(char *inbuf,char *outbuf,const char *tocode, const char *fromcode)
{
        size_t len = strlen(inbuf);
        size_t outlen = 50;
        iconv_t cd = iconv_open(tocode,fromcode);
		if(cd == (iconv_t)-1)
		{
			cout<<"faid to open "<<endl;
			exit(3);
		}
		iconv(cd,&inbuf,&len,&outbuf,&outlen);//将转换后的文件放入缓存区
		iconv_close(cd);
}


