/*
 *  Filename: task_base.cpp
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: task的基类
 *     1，做一些内存分配回收，解析ip端口、模调上报等基础操作.
 *
 */

#include "task_base.h"

//全局变量
CManagerSvrConn g_oMgSvrConn;          //下层接口连接信息
CManagerMcall   g_oMgMcall;            //监控信息--模调
CManagerMoni    g_oMgMoni;             //监控信息--monitor

CTaskBase::CTaskBase()
    :m_pSendBuf(NULL)
    ,m_iSendBufSize(0)
    ,m_bNeedFreeSendBuf(true)
    ,m_pRecvBuf(NULL)
    ,m_iRecvBufSize(0)
    ,m_pFuncCheckPkt(NULL)
    ,m_iDstPort(0)
    ,m_iOptCostTime_us(0)
{
    SetTaskType(TASK_UNDEF);
    SetResult(1); //设置初始值为1（0表示成功，负数表示操作错误码，1表示初始值）
}

CTaskBase::~CTaskBase()
{
    if(NULL != m_pSendBuf && m_bNeedFreeSendBuf) //部分task请求是使用string存储发送数据的，无需释放内存
    {
        free(m_pSendBuf);
        m_pSendBuf = NULL;
    }

    if(NULL != m_pRecvBuf)
    {
        free(m_pRecvBuf);
        m_pRecvBuf = NULL;
    }
}

int CTaskBase::Process()
{
    int iRet = 0;

    //做必要的数据校验
    if(EM_CONN_TCP != m_stConnInfo.eConnType && EM_CONN_UDP != m_stConnInfo.eConnType)
    {
        SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: paramErr, conn_type error "
            ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str());
        return ERR_TASK_PARAM;
    }
    if(EM_CONN_TCP == m_stConnInfo.eConnType && NULL == m_pFuncCheckPkt)
    {
        SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: paramErr, TCP FuncCheckPkt == NULL "
            ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str());
        return ERR_TASK_PARAM;
    }

    //发送buf校验
    if(NULL == m_pSendBuf || m_iSendBufSize <= 0 || m_iRecvBufSize <= 0)
    {
        SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: paramErr, SendBuf is NULL "
            ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str());
        return ERR_TASK_SEND_EMPTY;
    }

    //解析目标ip和端口
    sockaddr_in dstAddr;
    QOSREQUEST stQosReq;
    iRet = GetSockAddr(dstAddr,stQosReq);
    if(iRet != 0)
        return iRet; //详细的错误日志在GetSockAddr函数中

    //统计请求耗时
    timeval time_begin;
    gettimeofday(&time_begin, NULL);

    //调用spp微线程接口
    if(EM_CONN_TCP == m_stConnInfo.eConnType)
    {
        iRet = mt_tcpsendrcv(&dstAddr, (void*)m_pSendBuf, m_iSendBufSize, m_pRecvBuf, m_iRecvBufSize
                    , m_stConnInfo.iTimeOut_ms, m_pFuncCheckPkt, NULL, true);
    }
    else
    {
        iRet = MallocUdpRecvBuf(); //udp 需要自己分配接受buf
        if(0 != iRet)
        {
            SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: MallocUdpBufErr, iRet[%d]recvBufSize[%d] "
                ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),iRet,m_iRecvBufSize);

            return iRet;
        }

        iRet = mt_udpsendrcv(&dstAddr, (void*)m_pSendBuf, m_iSendBufSize, m_pRecvBuf, m_iRecvBufSize, m_stConnInfo.iTimeOut_ms);
    }

    //统计请求耗时
    timeval time_end;
    gettimeofday(&time_end, NULL);
    m_iOptCostTime_us = (1000000 * (time_end.tv_sec - time_begin.tv_sec) + time_end.tv_usec - time_begin.tv_usec);

    //上报l5调用结果
    if(EM_GETADDR_L5 == m_stConnInfo.eGetAddrType)
    {
        string strErrMsg;
        ApiRouteResultUpdate(stQosReq,iRet,m_iOptCostTime_us,strErrMsg);
    }

    if(iRet != 0)
    {
        SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: SendRecv_error, dst_ipport[%s:%d] errRet[%d]costTimeUs[%d]sendLen[%d]recvLen[%d]"
            ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),m_strDstIP.c_str(),m_iDstPort,iRet,m_iOptCostTime_us,m_iSendBufSize,m_iRecvBufSize);

        return iRet;
    }

    SPP_TRACE_LOG("optBaseTask tName[%s]tKey[%s]: SendRecv_succ, dst_ipport[%s:%d] costTimeUs[%d] sendLen[%d]recvLen[%d]"
        ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),m_strDstIP.c_str(),m_iDstPort,m_iOptCostTime_us,m_iSendBufSize,m_iRecvBufSize);

    return 0;
}

int CTaskBase::GetSockAddr(sockaddr_in &dstAddr,QOSREQUEST &stQosReq)
{
    int iRet = 0;

    if(EM_GETADDR_NAME == m_stConnInfo.eGetAddrType) //名字服务获取ip port
    {
        ZkHost host;
        iRet = getHostByKey(m_stConnInfo.strName.c_str(),&host) ;
        if(iRet != 0)
        {
            SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: GetSockAddr_Error, iRet[%d] nameapi[%s]"
                ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),iRet,m_stConnInfo.strName.c_str());

            return ERR_TASK_GETADDR;
        }

        dstAddr.sin_family      = AF_INET;
        dstAddr.sin_addr.s_addr = inet_addr(host.ip);
        dstAddr.sin_port        = htons(host.port);

        m_strDstIP = string(host.ip);
        m_iDstPort = host.port;

        SPP_TRACE_LOG("optBaseTask tName[%s]tKey[%s]: GetSockAddr_Succ, nameapi[%s] ipport[%s:%d] timeout[%dms]"
            ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),m_stConnInfo.strName.c_str()
            ,m_strDstIP.c_str(),m_iDstPort,m_stConnInfo.iTimeOut_ms);
    }
    else if(EM_GETADDR_L5 == m_stConnInfo.eGetAddrType) //l5获取ip port
    {
        stQosReq._modid = m_stConnInfo.iModId;
        stQosReq._cmd   = m_stConnInfo.iCmdId;
        string strErrMsg;
        iRet = ApiGetRoute(stQosReq, 0.1, strErrMsg);
        if(iRet < 0 || stQosReq._host_ip.empty() || stQosReq._host_port <= 0)
        {
            SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: GetSockAddr_Error, iRet[%d] l5[%d:%d] ipport[%s:%d]"
                ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),iRet,m_stConnInfo.iModId,m_stConnInfo.iCmdId
                ,stQosReq._host_ip.c_str(),stQosReq._host_port);

            return ERR_TASK_GETADDR;
        }

        dstAddr.sin_family      = AF_INET;
        dstAddr.sin_addr.s_addr = inet_addr(stQosReq._host_ip.c_str());
        dstAddr.sin_port        = htons(stQosReq._host_port);

        m_strDstIP = stQosReq._host_ip;
        m_iDstPort = stQosReq._host_port;

        SPP_TRACE_LOG("optBaseTask tName[%s]tKey[%s]: GetSockAddr_Succ, l5[%d:%d] ipport[%s:%d] timeout[%dms]"
            ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),m_stConnInfo.iModId,m_stConnInfo.iCmdId
            ,m_strDstIP.c_str(),m_iDstPort,m_stConnInfo.iTimeOut_ms);
    }
    else if(EM_GETADDR_IPPORT == m_stConnInfo.eGetAddrType)
    {
        if(m_stConnInfo.strIP.empty() || 0 == m_stConnInfo.iPort)
        {
            SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: GetSockAddr_Error, only_use_ipport[%s:%d]"
                ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),m_stConnInfo.strIP.c_str(),m_stConnInfo.iPort);

            return ERR_TASK_GETADDR;
        }

        dstAddr.sin_family      = AF_INET;
        dstAddr.sin_addr.s_addr = inet_addr(m_stConnInfo.strIP.c_str());
        dstAddr.sin_port        = htons(m_stConnInfo.iPort);

        m_strDstIP = m_stConnInfo.strIP;
        m_iDstPort = m_stConnInfo.iPort;

        SPP_TRACE_LOG("optBaseTask tName[%s]tKey[%s]: GetSockAddr_Succ, only_use_ipport[%s:%d] timeout[%dms]"
            ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str()
            ,m_strDstIP.c_str(),m_iDstPort,m_stConnInfo.iTimeOut_ms);
    }
    else
    {
        SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: GetSockAddr_Error, no connInfo"
            ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str());
        return ERR_TASK_GETADDR;
    }

    return 0;
}
