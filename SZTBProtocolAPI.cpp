/*
 * SZTBProtocolAPI.cpp
 *
 *  Created on: 2016年7月29日
 *     Author: wangqi
 */

#include "SZTBGPSAPI.h"
#include "SZTBProtoPack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>


SZTBPROTOCOL_API int ParsePositionInfo(UInt8* pData, int nDataLen,
    T_CCMS_POSITION_DATA &pParam)
{
    static UInt64 count = 0;
    count++;

    if (pData == NULL || nDataLen <= 0)
        return CCMS_ERRORMSG_FAIL;

    TRANSMIT_MSG_PACKET Msg;

    int nReadPos_Msg = 0;
    UInt16 nID, ParamLen;

    memset(&Msg.MsgID, 0, 1);
    memcpy(&Msg.MsgID, pData + nReadPos_Msg, 1);
    nReadPos_Msg += 1;

    memset(&Msg.MsgLen, 0, 2);
    memcpy(&Msg.MsgLen, pData + nReadPos_Msg, 2);
    nReadPos_Msg += 2;

    if (Msg.MsgID != 0x02 && Msg.MsgID != 0x03 && Msg.MsgID != 0x04)
    {
        printf("count=%llu errMsgId=0x%x MsgLen=%d nDataLen=%d\n",
            count, Msg.MsgID, Msg.MsgLen, nDataLen);
        return nDataLen;
    }
    if (nDataLen < Msg.MsgLen + 3)
    {
        printf("count=%llu Incomplete MsgId=0x%x MsgLen=%d nDataLen=%d\n",
            count, Msg.MsgID, Msg.MsgLen, nDataLen);
        return 0;
    }

    memset(&pParam.nHeight, 0, 2);
    if (Msg.MsgID == 0x02)
    {
        bool b0x01 = false;
        while (nReadPos_Msg < (Msg.MsgLen + 3))
        {
            memset(&nID, 0, 2);
            memcpy(&nID, pData + nReadPos_Msg, 2);
            nReadPos_Msg += 2;

            memset(&ParamLen, 0, 2);
            memcpy(&ParamLen, pData + nReadPos_Msg, 2);
            nReadPos_Msg += 2;

            if (ParamLen == 0)
            {
                continue;
            }

            if (ParamLen + nReadPos_Msg > Msg.MsgLen + 3)
            {
                printf("count=%llu gps errParamLen=%d nID=0x%x msgLen=%d readPos=%d\n",
                    count, ParamLen, nID, Msg.MsgLen + 3, nReadPos_Msg);
                return Msg.MsgLen + 3;
            }

            switch (nID)
            {
            case 0x01:
                {
                    char inbuf[50] = {0};
                    char outbuf[50] = {0};
                    memset(pParam.chVehicleID, 0, 16);
                    memcpy(inbuf, pData + nReadPos_Msg, ParamLen);
                    TranscodOut(inbuf, outbuf, "utf-8", "gbk");
                    memcpy(pParam.chVehicleID, outbuf, strlen(outbuf));
                    //if (strstr(outbuf, "粤") == NULL || strlen(outbuf) != 9)
                    //{
                    //    printf("count=%llu gps err inbuf=%s outbuf=%s paramLen=%d\n",
                    //        count, inbuf, outbuf, ParamLen);
                    //    for (int i = 0; i < nDataLen; ++i) {
                    //        printf("%.2x ", *(pData + i));
                    //    }
                    //    printf("\n");
                    //}
                    b0x01 = true;
                }
                break;
            case 0x02:
                memset(&pParam.nLongitude, 0, 4);
                memcpy(&pParam.nLongitude, pData + nReadPos_Msg,
                    ParamLen);
                break;
            case 0x03:
                memset(&pParam.nLatitude, 0, 4);
                memcpy(&pParam.nLatitude, pData + nReadPos_Msg,
                    ParamLen);
                break;
            case 0x04:
                {
                    UInt8 ReportTime_Buff[7];
                    memset(&ReportTime_Buff, 0, 7);
                    memset(&pParam.stDateTime, 0, 7);
                    memcpy(ReportTime_Buff, pData + nReadPos_Msg,
                        ParamLen);
                    pParam.stDateTime.nYear = (ReportTime_Buff[1] << 8)
                        | ReportTime_Buff[0];
                    pParam.stDateTime.nMonth = ReportTime_Buff[2];
                    pParam.stDateTime.nDay = ReportTime_Buff[3];
                    pParam.stDateTime.nHour = ReportTime_Buff[4];
                    pParam.stDateTime.nMinute = ReportTime_Buff[5];
                    pParam.stDateTime.nSecond = ReportTime_Buff[6];
                }
                break;
            case 0x05:
                memset(&pParam.nDeviceID, 0, 4);
                memcpy(&pParam.nDeviceID, pData + nReadPos_Msg, ParamLen);
                break;
            case 0x06:
                memset(&pParam.nSpeed, 0, 2);
                memcpy(&pParam.nSpeed, pData + nReadPos_Msg,
                    ParamLen);
                break;
            case 0x07:
                memset(&pParam.nDirect, 0, 2);
                memcpy(&pParam.nDirect, pData + nReadPos_Msg,
                    ParamLen);
                break;
            case 0x08:
                memset(&pParam.nStatus, 0, 1);
                memcpy(&pParam.nStatus, pData + nReadPos_Msg, ParamLen);
                break;
            case 0x10:
                {
                    char SIMCard[50] = {0};
                    char outbuf[50] = {0};
                    memset(pParam.chSimCard, 0, 16);
                    memcpy(SIMCard, pData + nReadPos_Msg, ParamLen);
                    TranscodOut(SIMCard, outbuf, "utf-8", "gbk");
                    memcpy(pParam.chSimCard, outbuf, strlen(outbuf));
                }
                break;
            case 0x12:
                {
                    char *color = new char[ParamLen];
                    char outbuf[10] = {0};
                    memset(&pParam.nVehicleColor, 0, 1);
                    memcpy(color, pData + nReadPos_Msg, ParamLen);
                    TranscodOut(color, outbuf, "utf-8", "gbk");
                    if ((strcmp(outbuf, "黄") == 0)
                        || (strcmp(outbuf, "黄色") == 0))
                    {
                        pParam.nVehicleColor = CCMS_COLORTYPE_YELLOW;
                    }
                    else if ((strcmp(outbuf, "黑") == 0)
                        || (strcmp(outbuf, "黑色") == 0))
                    {
                        pParam.nVehicleColor = CCMS_COLORTYPE_BLACK;
                    }
                    else if ((strcmp(outbuf, "蓝") == 0)
                        || (strcmp(outbuf, "蓝色") == 0))
                    {
                        pParam.nVehicleColor = CCMS_COLORTYPE_BLUE;
                    }
                    else if ((strcmp(outbuf, "白") == 0)
                        || (strcmp(outbuf, "白色") == 0))
                    {
                        pParam.nVehicleColor = CCMS_COLORTYPE_WHITE;
                    }
                    delete[] color;
                }
                break;
            case 0x11:
            case 0x14:
                // do nothing
                break;
            default:
                printf("count=%llu gps errnID=0x%x ParamLen=%d\n",
                    count, nID, ParamLen);
                break;
            }
            nReadPos_Msg += ParamLen;
        }
        if (!b0x01)
        {
            for (int i = 0; i < Msg.MsgLen; ++i) {
                printf("%.2x ", *(pData + i));
            }
            printf("  no 0x01\n");
        }
    }
    else if (Msg.MsgID == 0x03)
    {
        printf("alarm!\n");
        while (nReadPos_Msg < (Msg.MsgLen + 3))
        {
            memset(&nID, 0, 2);
            memcpy(&nID, pData + nReadPos_Msg, 2);
            nReadPos_Msg += 2;

            memset(&ParamLen, 0, 2);
            memcpy(&ParamLen, pData + nReadPos_Msg, 2);
            nReadPos_Msg += 2;

            if (ParamLen == 0)
            {   
                continue;
            }

            if (ParamLen + nReadPos_Msg > Msg.MsgLen + 3)
            {   
                printf("count=%llu alarm errParamLen=%d nID=0x%x msgLen=%d readPos=%d\n",
                    count, ParamLen, nID, Msg.MsgLen + 3, nReadPos_Msg);
                return Msg.MsgLen + 3;
            }

            switch (nID)
            {
            case 0x09:
                {
                    UInt8 alarm;
                    memset(&pParam.nAlarmType, 0, 1);
                    memcpy(&alarm, pData + nReadPos_Msg, ParamLen);
                    if (alarm == 1)
                    {
                        pParam.nAlarmType = CCMS_ALARMTYPE_OVERSPEED;
                    }
                }
                break;
            case 0x10:
                memset(&pParam.chSimCard, 0, 16);
                memcpy(&pParam.chSimCard, pData + nReadPos_Msg,
                    ParamLen);
                break;
            case 0x12:
                {
                    char *color = new char[ParamLen];
                    char outbuf[10] = {0};
                    memset(&pParam.nVehicleColor, 0, 1);
                    memcpy(color, pData + nReadPos_Msg, ParamLen);
                    TranscodOut(color, outbuf, "utf-8", "gbk");
                    if ((strcmp(outbuf, "黄") == 0)
                        || (strcmp(outbuf, "黄色") == 0))
                    {
                        pParam.nVehicleColor = CCMS_COLORTYPE_YELLOW;
                    }
                    else if ((strcmp(outbuf, "黑") == 0)
                        || (strcmp(outbuf, "黑色") == 0))
                    {
                        pParam.nVehicleColor = CCMS_COLORTYPE_BLACK;
                    }
                    else if ((strcmp(outbuf, "蓝") == 0)
                        || (strcmp(outbuf, "蓝色") == 0))
                    {
                        pParam.nVehicleColor = CCMS_COLORTYPE_BLUE;
                    }
                    else if ((strcmp(outbuf, "白") == 0)
                        || (strcmp(outbuf, "白色") == 0))
                    {
                        pParam.nVehicleColor = CCMS_COLORTYPE_WHITE;
                    }
                    delete[] color;
                }
                break;
            default:
                printf("count=%llu alarm errnID=0x%x ParamLen=%d\n",
                    count, nID, ParamLen);
                break;
            }

            nReadPos_Msg += ParamLen;
        }
    }
    else if (Msg.MsgID == 0x04)
    {
        // do nothing
    }

    return Msg.MsgLen + 3;
}

SZTBPROTOCOL_API int ParsePositionInfo2(UInt8* pData,int nDataLen,T_CCMS_POSITION_DATA &data)
{
    if (pData == NULL || nDataLen <= 0)
    {
        return -1;
    }

    memset(&data, 0, sizeof(data));
    if (nDataLen >= (int)sizeof(data))
    {
        memcpy(&data, pData, sizeof(data));
        return sizeof(data);
    }

    return 0;
}

SZTBPROTOCOL_API int Msg_Packet(char* Pos_Buff,T_CCMS_POSITION_DATA* pParam,int pParam_Len)
{
    if (Pos_Buff == NULL || pParam == NULL)
        return CCMS_ERRORMSG_FAIL;

    int nPos = 0;
    char* temp_buff = new char[pParam_Len];
    memset(temp_buff, 0, pParam_Len);

    memcpy(temp_buff + nPos, &pParam->chVehicleID, 16);
    nPos += 16;

    memcpy(temp_buff + nPos, &pParam->chSimCard, 16);
    nPos += 16;

    memcpy(temp_buff + nPos, &pParam->nDeviceID, 4);
    nPos += 4;

    memcpy(temp_buff + nPos, &pParam->nLatitude, 4);
    nPos += 4;

    memcpy(temp_buff + nPos, &pParam->nLongitude, 4);
    nPos += 4;

    memcpy(temp_buff + nPos, &pParam->nHeight, 2);
    nPos += 2;

    memcpy(temp_buff + nPos, &pParam->nSpeed, 2);
    nPos += 2;

    memcpy(temp_buff + nPos, &pParam->nDirect, 2);
    nPos += 2;

    memcpy(temp_buff + nPos, &pParam->nStatus, 1);
    nPos += 1;

    memcpy(temp_buff + nPos, &pParam->nAlarmType, 1);
    nPos += 1;

    memcpy(temp_buff + nPos, &pParam->nVehicleColor, 1);
    nPos += 1;

    memcpy(temp_buff + nPos, &pParam->stDateTime, 7);
    nPos += 7;

    if (nPos == pParam_Len)
    {
        memcpy(Pos_Buff, temp_buff, nPos);
        delete[] temp_buff;
        return nPos;
    }
    delete[] temp_buff;
    return CCMS_ERRORMSG_FAIL;
}















