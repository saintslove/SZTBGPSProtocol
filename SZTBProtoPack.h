/*
 * SZTBProtocolAPI.h
 *
 *  Created on: 2016年7月29日
 *      Author: wangqi
 */

#ifndef SZTBPROTOPACK_H_
#define SZTBPROTOPACK_H_

#include <CCMS.h>
#include <vector>
#include <iconv.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif


typedef struct T_TRANSMIT_MSG_PACKET
{
	UInt8            MsgID;						  //消息ID
	UInt16			 MsgLen;						 //消息长度
}TRANSMIT_MSG_PACKET;



void TranscodOut(char *inbuf,char *outbuf,const char *tocode, const char *fromcode);

#ifdef __cplusplus
  }
#endif

#endif /* SZTBPROTOCOLAPI_H_ */
