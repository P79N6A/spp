/*
 *  Filename: task_operate_jcedata.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 通用的请求jce数据的task
 *     1，通用发送接收以videoPacket包封装jce编码数据的task类，支持TCP或UDP类型，支持自定义发送和接受buf的长度
 *     2，负责处理以videoPacket包封装jce编码数据，定义对象时指定输入输出的jce数据结构即可
 *     3，目前使用该task的主要有：所有以videoPacket包封装jce数据的请求
 *
 */

#ifndef __TASK_OPERATE_JCEDATA_H__
#define __TASK_OPERATE_JCEDATA_H__

#include "task_base.h"
#include "video_packet.h" //视频通用包头

using namespace std;

template <typename T_outJce>
class COperateJceData : public CTaskBase
{
  public:
    //eRecvBufLen接受数据默认初始buf，不够时spp框架会自动扩展的；为了避免多次重新分配，默认初始buf应该和平均真实包大小相当
    COperateJceData(ENUM_TASK_TYPE eTaskType, ENUM_TASK_BUFLEN eRecvBufLen = BUF_LEN_4K);
    ~COperateJceData();

    // ====设置请求jce结构体、命令字、videoPacket包头信息===================================================
    template <typename T_inJce>
    int SetReqData(T_inJce &jceInData, unsigned short usCmd, CVideoPacket &stVideoPacket)
    {
        string strJceData;
        try
        {
            taf::JceOutputStream<taf::BufferWriter> osJce;
            jceInData.writeTo(osJce);
            strJceData.assign(osJce.getBuffer(), osJce.getLength());
        }
        catch (exception &e)
        {
            SPP_ERROR_LOG("optJceTask tName[%s]tKey[%s]: jceEncodeErr errMsg[%s]"
                ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(), e.what());

            return ERR_TASK_ENCODE;
        }

        stVideoPacket.setCommand(usCmd);
        stVideoPacket.setBody(strJceData);
        int iRet = stVideoPacket.encode();
        if (0 != iRet)
        {
            SPP_ERROR_LOG("optJceTask tName[%s]tKey[%s]: videoPacketEncodeErr iRet[%d]"
                ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(), iRet);

            return ERR_TASK_ENCODE;
        }

        m_strReqData.assign((char *)(stVideoPacket.getPacket()), stVideoPacket.getPacketLen());

        return 0;
    }

    //jce回包数据中回包错误码字段的地址,主要用于监控上报的
    //例如回包jce中字段retCode表示回包错误码，则调用函数进行设置: SetRspJceRetCode((int*)&(GetRspJceData().retCode))
    void SetRspJceRetCode(int *pIRetCode)
    {
        m_pIRetCode = pIRetCode;
    }

    //jce回包数据（不能被修改），返回const类型数据
    const T_outJce& GetRspJceData()
    {
        return m_jceOutData;
    }

    //jce回包数据（可被修改的），有些业务需要修改回包数据，单独提供一个非const返回类型数据
    T_outJce& GetRspJceData_NoConst()
    {
        return m_jceOutData;
    }

    const CVideoPacket& GetRspVPacket()
    {
        return m_oVPacket;
    }

    // ====spp微线程框架的执行函数=============================================================================
    virtual int Process();

  private:
    string        m_strReqData; //请求数据
    T_outJce      m_jceOutData; //jce回包数据
    int          *m_pIRetCode;  //jce回包数据中回包错误码字段的地址
    CVideoPacket  m_oVPacket;   //videoPacket回包数据
};

#include "task_operate_jcedata.hpp"

#endif /*__TASK_OPERATE_JCEDATA_H__*/
