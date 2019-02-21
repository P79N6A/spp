/*
 *  Filename: task_base.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: task的基类
 *     1，做一些内存分配回收，解析ip端口、模调上报等基础操作.
 *
 */

#ifndef __TASK_BASE_H__
#define __TASK_BASE_H__

#include <new>
#include <map>
#include <cstddef>

#include "mt_incl.h"           //spp微线程需要包含的头文件
#include "task_commdef.h"      //task框架定义的枚举、结构、宏定义
#include "task_commdef_proj.h" //项目自定义的枚举或宏定义，用于项目和task框架交互定义

#include "nameapi.h"           //名字服务api
#include "qos_client.h"        //l5 api

#include "Attr_API.h"          //monitor监控上报
#include "bossapi.h"           //模调上报的时候使用
using namespace BossAPI;

class CManagerSvrConn
{
  public:
    CManagerSvrConn() {};

    ~CManagerSvrConn()
    {
        for(map<int,StSvrConnInfo*>::iterator itMap = m_mapPSvrConn.begin(); itMap != m_mapPSvrConn.end(); ++itMap)
        {
            if(NULL != itMap->second)
            {
                delete (itMap->second);
                itMap->second = NULL;
            }
        }
    };

    int SetSvrConnInfo(int iSvrType, const string &strName, int iTimeOut_ms, ENUM_CONN eConnType = EM_CONN_TCP)
    {
        StSvrConnInfo *pConn = GetSvrConnInfo(iSvrType);
        if(NULL == pConn)
        {
            pConn = new (nothrow) StSvrConnInfo();
            if(NULL == pConn)
            {
                SPP_ERROR_LOG("SetConnInfoError: new Error");
                return -1;
            }

            m_mapPSvrConn[iSvrType] = pConn;
        }

        pConn->strName      = strName;
        pConn->iTimeOut_ms  = iTimeOut_ms;
        pConn->eConnType    = eConnType;
        pConn->eGetAddrType = EM_GETADDR_NAME;

        return 0;
    };

    int SetSvrConnInfo(int iSvrType, int iModId, int iCmdId, int iTimeOut_ms, ENUM_CONN eConnType = EM_CONN_TCP)
    {
        StSvrConnInfo *pConn = GetSvrConnInfo(iSvrType);
        if(NULL == pConn)
        {
            pConn = new (nothrow) StSvrConnInfo();
            if(NULL == pConn)
            {
                SPP_ERROR_LOG("SetConnInfoError: new Error");
                return -1;
            }

            m_mapPSvrConn[iSvrType] = pConn;
        }

        pConn->iModId       = iModId;
        pConn->iCmdId       = iCmdId;
        pConn->iTimeOut_ms  = iTimeOut_ms;
        pConn->eConnType    = eConnType;
        pConn->eGetAddrType = EM_GETADDR_L5;

        return 0;
    };

    int SetSvrConnInfo(int iSvrType, const string &strIp, int iPort, int iTimeOut_ms, ENUM_CONN eConnType = EM_CONN_TCP)
    {
        StSvrConnInfo *pConn = GetSvrConnInfo(iSvrType);
        if(NULL == pConn)
        {
            pConn = new (nothrow) StSvrConnInfo();
            if(NULL == pConn)
            {
                SPP_ERROR_LOG("SetConnInfoError: new Error");
                return -1;
            }

            m_mapPSvrConn[iSvrType] = pConn;
        }

        pConn->strIP        = strIp;
        pConn->iPort        = iPort;
        pConn->iTimeOut_ms  = iTimeOut_ms;
        pConn->eConnType    = eConnType;
        pConn->eGetAddrType = EM_GETADDR_IPPORT;

        return 0;
    };

    StSvrConnInfo* GetSvrConnInfo(int iSvrType)
    {
        map<int,StSvrConnInfo*>::iterator itMap = m_mapPSvrConn.find(iSvrType);
        if(itMap != m_mapPSvrConn.end())
            return itMap->second;

        return NULL;
    };

    string OutConnInfo()
    {
        stringstream oss;
        map<int,StSvrConnInfo*>::iterator itMap = m_mapPSvrConn.begin();
        for(; itMap != m_mapPSvrConn.end(); ++itMap)
        {
            oss<<"taskType["<<GetTaskName(itMap->first)<<"]==>timeOutMs["<<itMap->second->iTimeOut_ms<<"]";

            if(EM_GETADDR_NAME == itMap->second->eGetAddrType) //名字服务获取ip port
                oss<<"Name["<<itMap->second->strName<<"]"<<endl;
            else if(EM_GETADDR_L5 == itMap->second->eGetAddrType) //l5获取ip port
                oss<<"L5["<<itMap->second->iModId<<":"<<itMap->second->iCmdId<<"]"<<endl;
            else if(EM_GETADDR_IPPORT == itMap->second->eGetAddrType)
                oss<<"IpPort["<<itMap->second->strIP<<":"<<itMap->second->iPort<<"]"<<endl;
        }

        return oss.str();
    }

  private:
    map<int,StSvrConnInfo*> m_mapPSvrConn;
};


class CManagerMcall
{
  public:
    CManagerMcall() {};

    ~CManagerMcall()
    {
        for(map<int,StMcallInfo*>::iterator itMap = m_mapPMcallInfo.begin(); itMap != m_mapPMcallInfo.end(); ++itMap)
        {
            if(NULL != itMap->second)
            {
                delete (itMap->second);
                itMap->second = NULL;
            }
        }
    };

    int SetMcallInfo(int iIFaceType, int iInitModId,int iInitIface,int iPassModId,int iPassIface)
    {
        StMcallInfo *pMcall = GetMcallInfo(iIFaceType);
        if(NULL == pMcall)
        {
            pMcall = new (nothrow) StMcallInfo();
            if(NULL == pMcall)
            {
                SPP_ERROR_LOG("SetConnInfoError: new Error");
                return -1;
            }

            m_mapPMcallInfo[iIFaceType] = pMcall;
        }

        pMcall->bMcall     = true;

        pMcall->iInitModId = iInitModId;
        pMcall->iInitIface = iInitIface;
        pMcall->iPassModId = iPassModId;
        pMcall->iPassIface = iPassIface;

        return 0;
    };

    StMcallInfo* GetMcallInfo(int iIFaceType)
    {
        map<int,StMcallInfo*>::iterator itMap = m_mapPMcallInfo.find(iIFaceType);
        if(itMap != m_mapPMcallInfo.end())
            return itMap->second;

        return NULL;
    };

  private:
    map<int,StMcallInfo*> m_mapPMcallInfo;
};


class CManagerMoni
{
  public:
    CManagerMoni() {};

    ~CManagerMoni()
    {
        for(map<int,StMoniInfo*>::iterator itMap = m_mapPMoniInfo.begin(); itMap != m_mapPMoniInfo.end(); ++itMap)
        {
            if(NULL != itMap->second)
            {
                delete (itMap->second);
                itMap->second = NULL;
            }
        }
    };

    int SetMonitorInfo(int iIFaceType, int iMoniAll, int iMoniFail, int iMoniRetErr, int iMoniSucc, int iMoniEncodeErr, int iMoniDecodeErr)
    {
        StMoniInfo *pMoni = GetMoniInfo(iIFaceType);
        if(NULL == pMoni)
        {
            pMoni = new (nothrow) StMoniInfo();
            if(NULL == pMoni)
            {
                SPP_ERROR_LOG("SetConnInfoError: new Error");
                return -1;
            }

            m_mapPMoniInfo[iIFaceType] = pMoni;
        }

        pMoni->bMoni          = true;

        pMoni->iMoniAll       = iMoniAll;
        pMoni->iMoniFail      = iMoniFail;
        pMoni->iMoniRetErr    = iMoniRetErr;
        pMoni->iMoniSucc      = iMoniSucc;

        pMoni->iMoniEncodeErr = iMoniEncodeErr;
        pMoni->iMoniDecodeErr = iMoniDecodeErr;

        return 0;
    };

    StMoniInfo* GetMoniInfo(int iIFaceType)
    {
        map<int,StMoniInfo*>::iterator itMap = m_mapPMoniInfo.find(iIFaceType);
        if(itMap != m_mapPMoniInfo.end())
            return itMap->second;

        return NULL;
    };

  private:
    map<int,StMoniInfo*> m_mapPMoniInfo;
};

//全局变量
extern CManagerSvrConn g_oMgSvrConn;          //下层接口连接信息
extern CManagerMcall   g_oMgMcall;            //监控信息--模调
extern CManagerMoni    g_oMgMoni;             //监控信息--monitor

class CTaskBase : public IMtTask
{
  public:
	CTaskBase();
    virtual ~CTaskBase();

    // ====初始化task-请求地址、模调监控、monitor监控=====================================================================
    /**
     * @brief  初始化task-请求地址、模调监控、monitor监控;
     * @note   本框架会自动调用SetTaskInfo，业务需要设置好g_oMgSvrConn、g_oMgMcall、g_oMgMoni中对应的task_type的值。
     *         业务也可以自行调用SetTaskInfo，此时本框架不再自动调用SetTaskInfo函数
     * @param  *pStConnInfo:
     * @param  *pStMcallInfo:
     * @param  *pStMoniInfo:
     * @retval None
     */
    void SetTaskInfo(const StSvrConnInfo *pStConnInfo, const StMcallInfo *pStMcallInfo = NULL, const StMoniInfo *pStMoniInfo = NULL)
    {
        if(NULL != pStConnInfo)
            m_stConnInfo  = *pStConnInfo;  //映射到task内部

        if(NULL != pStMcallInfo)
            m_stMcallInfo = *pStMcallInfo; //映射到task内部

        if(NULL != pStMoniInfo)
            m_stMoniInfo  = *pStMoniInfo;  //映射到task内部

        return;
    }

    // ====设置连接信息(老接口，兼容之前的逻辑)==========================================================================
    void SetConnInfo(const string &strName, int iTimeOut_ms, ENUM_CONN eConnType = EM_CONN_TCP) //设置连接信息--名字服务
    {
        m_stConnInfo.strName      = strName;
        m_stConnInfo.iTimeOut_ms  = iTimeOut_ms;
        m_stConnInfo.eConnType    = eConnType;
        m_stConnInfo.eGetAddrType = EM_GETADDR_NAME;

        return;
    }
    void SetConnInfo(int iModId, int iCmdId, int iTimeOut_ms, ENUM_CONN eConnType = EM_CONN_TCP) //设置连接信息--L5
    {
        m_stConnInfo.iModId       = iModId;
        m_stConnInfo.iCmdId       = iCmdId;
        m_stConnInfo.iTimeOut_ms  = iTimeOut_ms;
        m_stConnInfo.eConnType    = eConnType;
        m_stConnInfo.eGetAddrType = EM_GETADDR_L5;

        return;
    }
    void SetConnInfo(const string &strIp, int iPort, int iTimeOut_ms, ENUM_CONN eConnType = EM_CONN_TCP) //设置连接信息--直连IP和端口
    {
        m_stConnInfo.strIP        = strIp;
        m_stConnInfo.iPort        = iPort;
        m_stConnInfo.iTimeOut_ms  = iTimeOut_ms;
        m_stConnInfo.eConnType    = eConnType;
        m_stConnInfo.eGetAddrType = EM_GETADDR_IPPORT;

        return;
    }

    // ====初始化模调监控信息(老接口，兼容之前的逻辑)=====================================================================
    void InitMcallInfo(int iInitModId,int iInitIface,int iPassModId,int iPassIface) //初始化模调监控信息
    {
        m_stMcallInfo.bMcall     = true;
        m_stMcallInfo.iInitModId = iInitModId;
        m_stMcallInfo.iInitIface = iInitIface;
        m_stMcallInfo.iPassModId = iPassModId;
        m_stMcallInfo.iPassIface = iPassIface;

        return;
    }
    int InitMoniInfo(int iMoniAll, int iMoniFail, int iMoniSucc, int iMoniRetErr, int iMoniEncodeErr, int iMoniDecodeErr)
    {
        m_stMoniInfo.bMoni          = true;
        m_stMoniInfo.iMoniAll       = iMoniAll;
        m_stMoniInfo.iMoniFail      = iMoniFail;
        m_stMoniInfo.iMoniRetErr    = iMoniRetErr;
        m_stMoniInfo.iMoniSucc      = iMoniSucc;
        m_stMoniInfo.iMoniEncodeErr = iMoniEncodeErr;
        m_stMoniInfo.iMoniDecodeErr = iMoniDecodeErr;

        return 0;
    };

    // ====设置tcp校验包完整性的函数指针，UDP请求不用设置=====================
    /**
     * [SetCheckFunc 设置tcp请求的包校验函数]
     * @param pFuncCheckPkt [函数指针，其在spp中的定义：
     *                        typedef int (*MtFuncTcpMsgChecker)(void* buf, int len, bool closed, void* msg_ctx, bool &msg_len_detected);]
     */
    void SetCheckFunc(MtFuncTcpMsgChecker pFuncCheckPkt)
    {
        m_pFuncCheckPkt = pFuncCheckPkt;
        return;
    }

    // ====设置发送接受buf====================================================
    /**
     * [SetSendBufByMalloc 分配发送buf的内存]
     * @param  eTaskSendBuf [发送buf的长度，程序会malloc指定大小的内存]
     * @return              [0成功，非0失败]
     */
    int SetSendBufByMalloc(ENUM_TASK_BUFLEN eTaskSendBuf)
    {
        if (m_iSendBufSize <= 0)
            m_iSendBufSize = (int)eTaskSendBuf;

        if (NULL == m_pSendBuf && m_iSendBufSize > 0)
        {
            void *tmp = malloc(m_iSendBufSize);
            if (NULL == tmp)
            {
                SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: MallocSendBufErr",GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str());
                return ERR_TASK_MALLOCBUF; // 申请内存失败
            }

            m_pSendBuf = (char *)tmp;
        }

        return 0;
    }

    /**
     * [SetSendBufByExtSendData 部分类型的task使用字符串存储发送数据，不需要分配发送buf]
     * @param  strSendData  [发送数据]
     * @return              [0成功，非0失败]
     */
    int SetSendBufByExtSendData(const string &strSendData)
    {
        if (NULL == m_pSendBuf && !strSendData.empty())
        {
            m_pSendBuf = const_cast<char *>(strSendData.c_str());
            m_iSendBufSize = strSendData.length();
            m_bNeedFreeSendBuf = false;
        }
        else
        {
            SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: LogicOptErr, sendDataSetAgain or sendDataIsEmpty,pSendBuf[0x%x]sendDataSize[%u]"
                ,GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str(),m_pSendBuf,strSendData.size());
            return ERR_TASK_ENCODE;
        }

        return 0;
    }

    /**
     * [SetRecvInitBufSize tcp请求的接受buf在spp3.0.1版本之后由框架自己分配，udp请求还是需要自己分配接受buf的内存
     *                     对于TCP请求：spp3.0.1之后tcp请求自动扩展接受buf,初始buf大小为4K
     *                     一次接受到完整数据且tcp校验函数能返回具体包大小时只需要2次分配；
     *                     否则会成倍扩展buf大小，效率不高，对于大包最好设置一个初始大小]
     * @param  eTaskRecvBuf [接受buf的长度；tcp请求spp框架会自动分配内存；udp需要自己分配内存，由私有函数MallocUdpRecvBuf来申请内存]
     * @return              [0成功，非0失败]
     */
    int SetRecvInitBufSize(ENUM_TASK_BUFLEN eTaskRecvBuf)
    {
        m_iRecvBufSize = (int)eTaskRecvBuf;

        return 0;
    }

    // =====获取发送和接受buf的地址和长度信息，用于外部打解包函数调用========
    char* GetSendBuf()
    {
        return m_pSendBuf;
    }

    int&  GetSendLen()
    {
        return m_iSendBufSize;
    }

    char* GetRecvBuf()
    {
        return (char*)m_pRecvBuf;
    }

    int&  GetRecvLen()
    {
        return m_iRecvBufSize;
    }

    // ====设置和获取task的key，用于识别相同类型的task操作====================
    void SetTaskKey(const string &strTaskKey)
    {
        m_strTaskKey = strTaskKey;
        return;
    }

    const string& GetTaskKey()
    {
        return m_strTaskKey;
    }

    //额外的key，有些操作一个taskKey不能满足，额外增加key的存储和读取
    void SetKeyExt1(const string &strKey)
    {
        m_strKeyExt1 = strKey;
        return;
    }

    const string& GetKeyExt1()
    {
        return m_strKeyExt1;
    }

    //额外的key，有些操作一个taskKey不能满足，额外增加key的存储和读取
    void SetKeyExt2(const string &strKey)
    {
        m_strKeyExt2 = strKey;
        return;
    }

    const string& GetKeyExt2()
    {
        return m_strKeyExt2;
    }

    // ====模调监控上报===========================================================
    void ReportMcall(int iRetValue)
    {
        if(m_stMcallInfo.bMcall)
        {
            MODCALL_REPORT(m_stMcallInfo.iInitModId,m_stMcallInfo.iInitIface,m_stMcallInfo.iPassModId,m_stMcallInfo.iPassIface
                ,m_iOptCostTime_us,iRetValue,m_strDstIP,0);
        }

        return;
    }
    // ====monitor上报===========================================================
    void ReportMoniAll()
    {
        if(m_stMoniInfo.bMoni)
            Attr_API(m_stMoniInfo.iMoniAll,1);
    }
    void ReportMoniFail()
    {
        if(m_stMoniInfo.bMoni)
            Attr_API(m_stMoniInfo.iMoniFail,1);
    }
    void ReportMoniRetErr()
    {
        if(m_stMoniInfo.bMoni)
            Attr_API(m_stMoniInfo.iMoniRetErr,1);
    }
    void ReportMoniSucc()
    {
        if(m_stMoniInfo.bMoni)
            Attr_API(m_stMoniInfo.iMoniSucc,1);
    }
    void ReportMoniEncodeErr()
    {
        if(m_stMoniInfo.bMoni)
            Attr_API(m_stMoniInfo.iMoniEncodeErr,1);
    }
    void ReportMoniDecodeErr()
    {
        if(m_stMoniInfo.bMoni)
            Attr_API(m_stMoniInfo.iMoniDecodeErr,1);
    }

    // ====获取目标ip地址=========================================================
    const string& GetDstIp()
    {
        return m_strDstIP;
    }

    int GetDstPort()
    {
        return m_iDstPort;
    }

    // ====spp微线程框架的执行函数============================================
	virtual int Process();

  private:
    //获取sockaddr_in地址；如果是l5请求则写l5的请求结构stQosReq，用户上报l5的状态
    int  GetSockAddr(sockaddr_in &dstAddr,QOSREQUEST &stQosReq);

    int  MallocUdpRecvBuf()
    {
        if(m_iRecvBufSize <= 0)
            m_iRecvBufSize = (int)BUF_LEN_8K;

        if(NULL == m_pRecvBuf && m_iRecvBufSize > 0)
        {
            void* tmp = malloc(m_iRecvBufSize);
            if(NULL == tmp)
            {
                SPP_ERROR_LOG("optBaseTask tName[%s]tKey[%s]: MallocUdpRecvBufErr",GetTaskName(GetTaskType()).c_str(),m_strTaskKey.c_str());
                return ERR_TASK_MALLOCBUF; // 申请内存失败
            }

            m_pRecvBuf = tmp;
        }

        return 0;
    }

  private:
    //发送buf
    char *m_pSendBuf;
    int   m_iSendBufSize;
    bool  m_bNeedFreeSendBuf;

    //接受buf
    void *m_pRecvBuf;
    int   m_iRecvBufSize;

    //spp中定义: typedef int (*MtFuncTcpMsgChecker)(void* buf, int len, bool closed, void* msg_ctx, bool &msg_len_detected);
    MtFuncTcpMsgChecker m_pFuncCheckPkt; //tcp检查包完整新的函数执行

    StSvrConnInfo m_stConnInfo;       //下层服务的连接信息
    StMcallInfo   m_stMcallInfo;      //模调监控信息
    StMoniInfo    m_stMoniInfo;       //monitor监控信息

    string m_strDstIP;           //目标ip
    int    m_iDstPort;           //目标port
    int    m_iOptCostTime_us;    //task网络请求处理耗时，用于模调和l5上报

    string m_strTaskKey;         //用于判别相同操作的task，(Task_Type用于判别不同操作的task)
    string m_strKeyExt1;         //额外的key，有些操作一个taskKey不能满足，额外增加key的存储和读取
    string m_strKeyExt2;         //额外的key，有些操作一个taskKey不能满足，额外增加key的存储和读取
};


#endif /*__TASK_BASE_H__*/
