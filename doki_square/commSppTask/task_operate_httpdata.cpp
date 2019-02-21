/*
 *  Filename: task_operate_httpdata.cpp
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 通用的请求http数据的task
 *
 */

#include "task_operate_httpdata.h"

static int CheckHttpPacket(void *buf, int len, bool closed, void* msg_ctx, bool &msg_len_detected)
{
    return COptHttpData::CheckHttpPkt(buf,len);
}

COperateHttpData::COperateHttpData(ENUM_TASK_TYPE eTaskType, ENUM_TASK_BUFLEN eRecvBufLen)
:m_bAutoDecodeRspBuf(true)
{
    SetTaskType((int)eTaskType);
    SetRecvInitBufSize(eRecvBufLen);
    SetCheckFunc(CheckHttpPacket);

    SetTaskInfo(g_oMgSvrConn.GetSvrConnInfo(GetTaskType()),g_oMgMcall.GetMcallInfo(GetTaskType()),g_oMgMoni.GetMoniInfo(GetTaskType()));
}

COperateHttpData::~COperateHttpData()
{
}

int COperateHttpData::Process()
{
    int iRet = 0;

    ReportMoniAll();

    iRet = SetSendBufByExtSendData(m_strHttpPkgReq);
    if(0 != iRet)
    {
        SPP_ERROR_LOG("optHttpTask tName[%s]tKey[%s]: SetSendBuf_error, ret[%d]"
            ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iRet);

        ReportMoniEncodeErr();
        ReportMcall(iRet);
        return iRet;
    }
    SPP_TRACE_LOG("optHttpTask tName[%s]tKey[%s]: sendBufSize[%u]"
        ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),GetSendLen());

    iRet = CTaskBase::Process();
    if(0 != iRet)
    {
        SPP_ERROR_LOG("optHttpTask tName[%s]tKey[%s]: baseTaskProcess_error, ret[%d]"
            ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iRet);

        ReportMoniFail();
        ReportMcall(iRet);
        return iRet;
    }

    //解析数据
    SPP_TRACE_LOG("optHttpTask tName[%s]tKey[%s]: HttpRspPkt:\n%s"
        ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),GetRecvBuf());//测试使用，打印整个http包

    if(m_bAutoDecodeRspBuf)
    {
        iRet = COptHttpData::DecodeHttpPkt(GetRecvBuf(), GetRecvLen(), m_stHttpDataRsp);
        if(0 != iRet) //解包失败
        {
            SPP_ERROR_LOG("optHttpTask tName[%s]tKey[%s]: DecodeHttp_error, ret[%d]"
                ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iRet);

            ReportMoniDecodeErr();
            ReportMcall(iRet);
            return ERR_TASK_DECODE;
        }

        int iStatus = atoi(m_stHttpDataRsp.strRspStatus.c_str());
        if (200 != iStatus) //回包数据异常
        {
            SPP_ERROR_LOG("optHttpTask tName[%s]tKey[%s]: HttpRetErr, RspStatus[%d] HttpRspPkt[%s]"
                ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iStatus,GetRecvBuf());

            ReportMoniRetErr();
            ReportMcall(iStatus);
            return iStatus;
        }
    }

    SPP_TRACE_LOG("optHttpTask tName[%s]tKey[%s]: baseTaskProcess, optSucc"
        ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str());

    ReportMoniSucc();
    ReportMcall(iRet);
    return 0;
}
