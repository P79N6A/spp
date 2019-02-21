/*
 *  Filename: udp_operate_transdata.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 通用的发送中转的task
 *     1，通用的中转操作，task是不支持udp只发不收操作的，这里自己定义udp发送逻辑
 *
 */

#ifndef __UDP_OPERATE_TRANSDATA_H__
#define __UDP_OPERATE_TRANSDATA_H__

#include "task_base.h"
#include "video_packet.h"
#include "cs_packet2.h"
#include "tcgi_udp_connector.h"

using namespace std;

static tcgi::CMyUDPConnector *pUdpCli = NULL;

class COperateTransData
{
public:

    //pVideoPacket为请求方的videopacket的指针，以带上相关包头信息
    /**
     * @brief  发送微博中转数据
     * @note
     * @param  &jceInData:    jce数据
     * @param  usCmd:         中转命令字
     * @param  *pVideoPacket: 视频videopacket指针
     * @param  strSignKey:    微博中转签名key，必须少于等于16个字符；strSignSky不为空的时候表示中转需要做签名，为空则不做签名
     * @param  iIsTest:       是否是测试环境 1:是，0:否
     * @retval
     */
    template <typename T_inJce>
    static int SendTransData(T_inJce  &jceInData, unsigned short usCmd, CVideoPacket *pVideoPacket
                    , string strSignKey = "" , int iIsTest = 0)
    {
        int iRet = 0;

        string strJceData;
        try
        {
            taf::JceOutputStream<taf::BufferWriter> osJce;
            jceInData.writeTo(osJce);
            strJceData.assign(osJce.getBuffer(),osJce.getLength());
        }
        catch(exception &e)
        {
            SPP_ERROR_LOG("wbTransData transCmd[0x%x]: jce_error errMsg[%s]",usCmd,e.what());
            return -1;
        }

        if(1 == iIsTest)
            pVideoPacket->setCtrlId(0x01); //测试环境标志

        pVideoPacket->setCommand(usCmd);
        pVideoPacket->setBody(strJceData);
        iRet = pVideoPacket->encode();
        if(0 != iRet)
        {
            SPP_ERROR_LOG("wbTransData transCmd[0x%x]: videoPacket_encode error[%d]",usCmd,iRet);
            return -1;
        }

        static CCsPacket cs_packet;
        cs_packet.setCommand(0x00ff);
        cs_packet.setCsCommand(0x00ff);
        cs_packet.setCsSubCommand(usCmd);
        cs_packet.resetWritePos();
        cs_packet.setBody((const uint8_t*)pVideoPacket->getPacket(), pVideoPacket->getPacketLen());

        string strSign;
        if(!strSignKey.empty())
        {
            iRet = cs_packet.setWbMqSign(strSignKey,strSign);
            if(0 != iRet)
            {
                SPP_ERROR_LOG("wbTransData transCmd[0x%x]: iRet[%d]key[%s]sign[%s]",usCmd,iRet,strSignKey.c_str(),strSign.c_str());
                return -208;
            }
        }

        iRet = cs_packet.encode();
        if(0 != iRet)
        {
            SPP_ERROR_LOG("wbTransData transCmd[0x%x]: cspacket_encode error[%d]",usCmd,iRet);
            return -1;
        }

        if(pUdpCli == NULL)
        {
            pUdpCli = new (nothrow) tcgi::CMyUDPConnector();
            if(NULL == pUdpCli)
            {
                SPP_ERROR_LOG("wbTransData transCmd[0x%x]: new udpCli error.",usCmd);
                return -2;
            }

            // if(!pUdpCli->Connect("127.0.0.1",28810))
            if(!pUdpCli->Connect("127.0.0.1",28810,"127.0.0.1")) //docker机器取本地ip时，CMyUDPConnector对象会core掉，需要改udp库才行，先通过传递本机ip来规避
            {
                delete pUdpCli;
                pUdpCli = NULL;

                SPP_ERROR_LOG("wbTransData transCmd[0x%x]: udp_connect errMsg[%s]",usCmd,pUdpCli->GetErrMsg());
                return -2;
            }
        }

        iRet = pUdpCli->Write((const char*)cs_packet.getPacket(), cs_packet.getPacketLen());
        if(iRet <= 0)
        {
            SPP_ERROR_LOG("wbTransData transCmd[0x%x]: error[%d] errMsg[%s]",usCmd,iRet,pUdpCli->GetErrMsg());
            return -3;
        }
        else
        {
            SPP_TRACE_LOG("wbTransData transCmd[0x%x]: succ sendLen[%u] key[%s]sign[%s]",usCmd,cs_packet.getPacketLen(),strSignKey.c_str(),strSign.c_str());
        }

        return 0;
    }
};

#endif /*__UDP_OPERATE_TRANSDATA_H__*/
