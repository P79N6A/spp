/*
 *  Filename: spp_framework.cpp
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 * 	Description: spp微线程框架入口函数
 *      1. 除了修改多个work进程路由外，基本不用修改代码
 *      2. 配置文件的初始化放在msg_init_root中执行，保证spp_framework不用做过多修改
 *
 */
#include "bossapi.h"        //模调监控
using namespace BossAPI;

#include "http_tool.h" 		//http打解包工具类 用于在spp_handle_input函数中解析http
#include "msg_init_root.h"  //主要处理读取配置文件、路由msg的操作


//全局变量
CServerBase        *g_pCServerBase = NULL;      //仅仅只是为了task打印日志的
StConfInfo          g_stConf;                   //程序配置文件
vapi::CommCacheApi  g_cacheApi;                 //共享内存cache


/**
 * @brief SPP框架接口函数：业务模块初始化插件接口（可选实现proxy,worker）
 * @param arg1 - 配置文件
 * @param arg2 - 服务器容器对象
 * @return 0 - 成功, 其它失败
 */
extern "C" int spp_handle_init(void* arg1, void* arg2)
{
    const char * etc  = (const char*)arg1;
    CServerBase* base = (CServerBase*)arg2;
    g_pCServerBase = base;

    if (base->servertype() == SERVER_TYPE_WORKER) //work init
    {
        SPP_TRACE_LOG("spp_handle_init: work init config[%s]",etc);

        int iRet = CSyncFrame::Instance()->InitFrame(base, 100000); //初始化框架
        if (iRet < 0)
        {
            SPP_ERROR_LOG("Sync framework init failed, iRet[%d]", iRet);
            return -1;
        }

        CConfigFile obj_conf(etc);//加载配置文件

        InitWorkInfo(obj_conf); //读取配置数据、设置下层接口配置、监控信息配置等

        //====配置中心agent初始化=================
        /*iRet = g_confApi.init(675);
        if(0 != iRet)
        {
            SPP_ERROR_LOG("ConfInitFail: confId[%d] initError[%d]", 675, iRet);
            return -2;
        }*/

        MODCALL_INIT();    //模调初始化
    }
    else
    {
        SPP_TRACE_LOG("spp_handle_init: proxy or ctrl int config[%s]",etc);
    }

    return 0;
}

/**
 * @brief SPP框架接口函数：业务模块检查报文合法性与分包接口(proxy)
 * @param flow - 请求包标志
 * @param arg1 - 数据块对象
 * @param arg2 - 服务器容器对象
 * @return ==0  数据包还未完整接收,继续等待
 *         > 0  数据包已经接收完整, 返回包长度
 *         < 0  数据包非法, 连接异常, 将断开TCP连接
 */
extern "C" int spp_handle_input(unsigned flow, void* arg1, void* arg2)
{
    blob_type *blob   = (blob_type *)arg1;//数据块对象，结构请参考tcommu.h
    CServerBase *base = (CServerBase *)arg2;//服务器容器对象

    int iLen = 0;
    char cFlag = *((char*)blob->data);

    if(0x26 == cFlag) //video packet
    {
        SPP_TRACE_LOG("it is packet from video packet");
        iLen = CVideoPacket::checkPacket((const char *)blob->data,blob->len);
        if(iLen < 0 )
            SPP_ERROR_LOG("CheckVideoPktError, cFlag[0x%x]blobLen[%d] iLen[%d]",cFlag,blob->len,iLen);
    }
    else if(0x0a == cFlag) //cs packet
    {
        SPP_TRACE_LOG("it is packet from cs packet");
        iLen = CCsPacket::checkPacket((const char *)blob->data,blob->len);
        if(iLen < 0 )
            SPP_ERROR_LOG("CheckCsPktError, cFlag[0x%x]blobLen[%d] iLen[%d]",cFlag,blob->len,iLen);
    }
    else if('G' == cFlag || 'P' == cFlag) //目前只收取GET和POST方式
    {
        SPP_TRACE_LOG("it is packet from http packet");
        iLen = COptHttpData::CheckHttpPkt(blob->data, blob->len);
        if(iLen < 0 )
            SPP_ERROR_LOG("CheckHttpPktError, cFlag[%c]blobLen[%d] iLen[%d]",cFlag,blob->len,iLen);
    }
    else
    {
        SPP_ERROR_LOG("it is unknow packet, cFlag[0x%x]cFlag[%c] blobLen[%d]",cFlag,cFlag,blob->len);
        iLen = -1;
    }

    SPP_TRACE_LOG("cFlag[0x%x]cFlag[%c] blobLen[%d]ilen[%d]",cFlag,cFlag,blob->len,iLen);
    return iLen;
}

/**
 * @brief SPP框架接口函数：业务模块报文按worker组分发接口(proxy)
 * @param flow - 请求包标志
 * @param arg1 - 数据块对象
 * @param arg2 - 服务器容器对象
 * @return 处理该报文的worker组id
 */
extern "C" int spp_handle_route(unsigned flow, void* arg1, void* arg2)
{
    int iGroupId = 1;

    blob_type *blob = (blob_type *)arg1;//数据块对象，结构请参考tcommu.h
    char cFlag = *((char*)blob->data);

    if(0x26 == cFlag) //video packet
    {
        iGroupId = 1;
        unsigned short usCmd = 0;
        {
            SPP_DEBUG_LOG("spp_handle_route: GetCmd DecodeVideopacket");

            CVideoPacket oVideoPacket;
            oVideoPacket.set_packet((uint8_t *)blob->data, blob->len);

            int iRet = oVideoPacket.decode();
            if (iRet != 0)
            {
                SPP_ERROR_LOG("spp_handle_route: GetCmd VideoPacketDecodeError, iRet[%d]", iRet);
                return iGroupId;
            }

            usCmd = oVideoPacket.getCommand();
        }

        switch (usCmd)
        {
        case CMD_CHANNEL: 
            iGroupId = 1;
            break;

        case CMD_SECOND_PAGE:
            iGroupId = 2;  
            break;

        default:
            iGroupId = 1;          // 写接口、雪球中转 - 4进程
            break;
        }
    }
    else if(0x0a == cFlag)         // cs packet
    {
        iGroupId = 2;
    }
    // else if('G' == cFlag || 'P' == cFlag) //目前只收取GET和POST方式
    // {
    //     iGroupId = 3;
    // }

	return iGroupId;
}

/**
 * @brief SPP框架接口函数：业务模块报文,worker组的处理接口(worker)
 * @param flow - 请求包标志
 * @param arg1 - 数据块对象
 * @param arg2 - 服务器容器对象
 * @return 0 - 成功,其它表示失败
 */
extern "C" int spp_handle_process(unsigned flow, void* arg1, void* arg2)
{
    blob_type   *blob  = (blob_type*)arg1;      //数据块对象，结构请参考tcommu.h
    CServerBase *base  = (CServerBase*)arg2;    //服务器容器对象
    CTCommu     *commu = (CTCommu*)blob->owner; //数据来源的通讯组件对象
    TConnExtInfo *connInfo = (TConnExtInfo*)blob->extdata; //数据来源的通讯组件对象

    CMsgOptBase *msg = GetNewCMsg(blob); //框架会做delete msg的操作
    if (!msg)
    {
        blob_type respblob;
        respblob.data  = NULL;
        respblob.len   = 0;
        commu->sendto(flow, &respblob, NULL);

        Attr_API(SPP_LOGIC_ERR, 1);
    	// SPP_ERROR_LOG("new_msg_error. flow:%u", flow);
        return -1;
    }

    //==========================队列式过载保护代码开始==========================
    // 检测从proxy收包，到当前的时间
    int64_t llCurTimeMs = __spp_get_now_ms();
    int64_t llRecvTimeMs = int64_t(connInfo->recvtime_) * 1000 + connInfo->tv_usec / 1000;
    SPP_TRACE_LOG("CheckProxyTimeOut: curT[%lld ms]recvT[%lld ms] curT-recvT[%lld ms] proxyConfT[%d ms]"
        , llCurTimeMs, llRecvTimeMs, llCurTimeMs - llRecvTimeMs, g_stConf.iProxyTimeout_ms);

    if (llCurTimeMs - llRecvTimeMs > g_stConf.iProxyTimeout_ms)
    {
        Attr_API(SPP_OVERLOAD, 1);

        // 写入过载错误码，给调用方回包
        msg->SetErrCode(CErrCodeProj::ERR_REQUEST_OVERLOAD);
        msg->SendRspData();

        // 过载保护，不执行后续操作了，需要自行删除msg的内存
        if (msg)
        {
            delete msg;
            msg = NULL;
            return 0;
        }

        SPP_ERROR_LOG("spp_handle_process, proxy timeout. take[%ld] timeout[%ld]"
            , llCurTimeMs - llRecvTimeMs, g_stConf.iProxyTimeout_ms);
    }
    //==========================队列式过载保护代码结束==========================

    //设置msg信息
    msg->SetServerBase(base);
    msg->SetTCommu(commu);
    msg->SetFlow(flow);
    msg->SetMsgTimeout(g_stConf.iMsgTimeout_ms);

    //V2版本之后使用命令字路由到不同的msg，在MsgRoute函数中数据已经拷贝到msg中了，这里不需要再拷贝整个报文了
    // msg->SetReqPkg(blob->data, blob->len); //微线程有独立空间,这里要拷贝一次报文

    CSyncFrame::Instance()->Process(msg); //框架会做delete msg的操作

    return 0;
}

/**
 * @brief SPP框架接口函数：业务服务终止接口函数(proxy/worker)
 * @param arg1 - 保留
 * @param arg2 - 服务器容器对象
 * @return 0 - 成功,其它表示失败
 */
extern "C" void spp_handle_fini(void* arg1, void* arg2)
{
    CServerBase* base = (CServerBase*)arg2;
    SPP_TRACE_LOG("spp_handle_fini");

    if ( base->servertype() == SERVER_TYPE_WORKER )
    {
        MODCALL_DESTORY(); //模调清理收回

        CSyncFrame::Instance()->Destroy();
    }
}
