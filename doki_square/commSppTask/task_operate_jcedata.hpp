/*
 *  Filename: task_operate_jcedata.hpp
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


/*
    @biref:对包的合法性进行检测
    @return:
    <0, 出错，包不合法
    =0, 包体没有接受完整,返回0让spp_proxy继续接受数据
    >0, 返回包体的长度
*/
static int CheckVideoPacket(void *buf, int len, bool closed, void* msg_ctx, bool &msg_len_detected)
{
    return CVideoPacket::checkPacket((char*)buf, (uint32_t)len);
}

template <typename T_outJce>
COperateJceData<T_outJce>::COperateJceData(ENUM_TASK_TYPE eTaskType, ENUM_TASK_BUFLEN eRecvBufLen)
:m_pIRetCode(NULL)
{
    SetTaskType((int)eTaskType);
    SetRecvInitBufSize(eRecvBufLen);
    SetCheckFunc(CheckVideoPacket); //无论tcp还是udp都设置下；udp也不会调用这个，无影响。

    SetTaskInfo(g_oMgSvrConn.GetSvrConnInfo(GetTaskType()),g_oMgMcall.GetMcallInfo(GetTaskType()),g_oMgMoni.GetMoniInfo(GetTaskType()));
}

template <typename T_outJce>
COperateJceData<T_outJce>::~COperateJceData()
{
}

template <typename T_outJce>
int COperateJceData<T_outJce>::Process()
{
    int iRet = 0;

    ReportMoniAll();

    iRet = SetSendBufByExtSendData(m_strReqData);
    if(0 != iRet)
    {
        SPP_ERROR_LOG("optJceTask tName[%s]tKey[%s]: SetSendBuf_error, ret[%d] ",GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iRet);

        ReportMoniEncodeErr();
        ReportMcall(iRet);
        return iRet;
    }
    SPP_TRACE_LOG("optJceTask tName[%s]tKey[%s]: sendBufSize[%u]"
        ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),GetSendLen());

    iRet = CTaskBase::Process();
    if(0 != iRet)
    {
        SPP_ERROR_LOG("optJceTask tName[%s]tKey[%s]: baseTaskProcess_error, ret[%d]",GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iRet);

        ReportMoniFail();
        ReportMcall(iRet);
        return iRet;
    }

    //解析回包
    m_oVPacket.set_packet((uint8_t *)GetRecvBuf(), GetRecvLen());
    iRet = m_oVPacket.decode();
    if (iRet != 0)
    {
        SPP_ERROR_LOG("optJceTask tName[%s]tKey[%s]: videoPacketDecodeErr iRet[%d]",GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(), iRet);

        ReportMoniDecodeErr();
        ReportMcall(ERR_TASK_DECODE);
        return ERR_TASK_DECODE;
    }

    try
    {
        taf::JceInputStream<taf::BufferReader> isJce;
        isJce.setBuffer(m_oVPacket.getBody().c_str(), m_oVPacket.getBody().length());
        m_jceOutData.readFrom(isJce);
    }
    catch (exception &e)
    {
        SPP_ERROR_LOG("optJceTask tName[%s]tKey[%s]: jceDecodeErr errMsg[%s] bodyLen[%u]"
            ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(), e.what(), m_oVPacket.getBody().length());

        ReportMoniDecodeErr();
        ReportMcall(ERR_TASK_DECODE);
        return ERR_TASK_DECODE;
    }

    SPP_TRACE_LOG("optJceTask tName[%s]tKey[%s]: baseTaskProcess, optSucc",GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str());

    if(NULL != m_pIRetCode && 0 != *m_pIRetCode)
    {
        int iRetCode = *m_pIRetCode;

        SPP_ERROR_LOG("optJceTask tName[%s]tKey[%s]: RetErr[%d] ",GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(), iRetCode);

        ReportMoniRetErr();
        ReportMcall(iRetCode);
        return iRetCode;
    }

    ReportMoniSucc();
    ReportMcall(0);
    return 0;
}
