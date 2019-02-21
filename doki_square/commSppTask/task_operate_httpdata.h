/*
 *  Filename: task_operate_httpdata.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 通用的请求http数据的task
 *
 */

#ifndef __TASK_OPERATE_HTTPDATA_H__
#define __TASK_OPERATE_HTTPDATA_H__

#include "task_base.h"
#include "http_tool.h"

using namespace std;

class COperateHttpData : public CTaskBase
{
  public:
    //eRecvBufLen接受数据默认初始buf，不够时spp框架会自动扩展的；为了避免多次重新分配，默认初始buf应该和平均真实包大小相当
    COperateHttpData(ENUM_TASK_TYPE eTaskType, ENUM_TASK_BUFLEN eRecvBufLen = BUF_LEN_8K);
    ~COperateHttpData();

    // ====设置请求包===============================================================
    void SetHttpReq(const string &strCgi, const string &strBody, EmHttpType eHttpType, MMapHttpHead &mmapHttpHead)
    {
        COptHttpData::ConstructHttpReqPkt(m_strHttpPkgReq, strCgi, strBody, eHttpType, mmapHttpHead);
        return;
    }

    void SetHttpReq(const string &strHost, const string &strCgi, const string &strBody, EmHttpType eHttpType, const string &strCookie, const string &strRefer)
    {
        COptHttpData::ConstructHttpReqPkt(m_strHttpPkgReq, strHost, strCgi, strBody, eHttpType, strCookie, strRefer);
        return;
    }

    void SetHttpReq(StHttpData &stHttpData)
    {
        stHttpData.WriteStHttpDataToStr(m_strHttpPkgReq);
        return;
    }

    // ====获取请求包的字符串，一般用于打印数据
    const string &GetReqHttpPkt()
    {
        return m_strHttpPkgReq;
    }

    // ====获取http回包body数据=====================================================
    StHttpData &GetRspHttpData()
    {
        return m_stHttpDataRsp;
    }

    void SetNoAutoDecodeRspBuf() //设置不自动解析http请求包，调用者从WriteRspBufToStr()获取buf自己解析
    {
        m_bAutoDecodeRspBuf = false;
        return;
    }
    void WriteRspBufToStr(string &strHttpRspPkg)
    {
        strHttpRspPkg.assign(GetRecvBuf(), GetRecvLen());
        return;
    }

    // ====spp微线程框架的执行函数==================================================
    virtual int Process();

  private:
    string     m_strHttpPkgReq;       //存储组装好的http请求包，按照http协议存储在字符串中
    StHttpData m_stHttpDataRsp;       //http回包数据结构
    bool       m_bAutoDecodeRspBuf;   //是否自动解析出http包，默认是自动解析的
};

#endif /*__TASK_OPERATE_HTTPDATA_H__*/
