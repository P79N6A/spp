/*
 *  Filename: msg_init_root.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description:
 *
 */

#ifndef __MSG_INIT_ROOT_H__
#define __MSG_INIT_ROOT_H__

#include <new>

//分命令字、类型处理
#include "msg_opt_base.h"
#include "msg_opt_square.h"

static void InitWorkInfo(CConfigFile &obj_conf)
{
    //全局配置文件
    g_stConf.iMsgTimeout_ms    = obj_conf.GetConfValue<int32_t>("Server", "iMsgTimeout_ms", "800");
    g_stConf.iIsTransCheckEnv  = obj_conf.GetConfValue<int32_t>("Server", "iIsTransCheckEnv", "0");
    g_stConf.iIsTestEnv        = obj_conf.GetConfValue<int32_t>("Server", "iIsTestEnv", "0");
    g_stConf.iProxyTimeout_ms = obj_conf.GetConfValue<int32_t>("Server", "iProxyTimeout_ms", "200"); // 队列等待超时
    SPP_DEBUG_LOG("SvrCommConf:\n %s",g_stConf.OutConfData().c_str());

    //接口配置文件，获取之后存储到g_oMgSvrConn中
    int iConfigAdminModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iConfigAdminModid", "0");
    int iConfigAdminCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iConfigAdminCmdid", "0");
    int iConfigAdminTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iConfigAdminTimeout_ms", "0");

    int iStarAdminModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iStarAdminModid", "0");
    int iStarAdminCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iStarAdminCmdid", "0");
    int iStarAdminTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iStarAdminTimeout_ms", "0");

    int iRightAdminModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iRightAdminModid", "0");
    int iRightAdminCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iRightAdminCmdid", "0");
    int iRightAdminTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iRightAdminTimeout_ms", "0");

    int iFansAdaptModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iFansAdaptModid", "0");
    int iFansAdaptCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iFansAdaptCmdid", "0");
    int iFansAdaptTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iFansAdaptTimeout_ms", "0");

    int iDokiInfoModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iDokiInfoModid", "0");
    int iDokiInfoCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iDokiInfoCmdid", "0");
    int iDokiInfoTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iDokiInfoTimeout_ms", "0");

    int iStarRankModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iStarRankModid", "0");
    int iStarRankCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iStarRankCmdid", "0");
    int iStarRankTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iStarRankTimeout_ms", "0");

    int iFansAdminCttModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iFansAdminCttModid", "0");
    int iFansAdminCttCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iFansAdminCttCmdid", "0");
    int iFansAdminCttTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iFansAdminCttTimeout_ms", "0");

    int iGetFeedDetailModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iGetFeedDetailModid", "0");
    int iGetFeedDetailCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iGetFeedDetailCmdid", "0");
    int iGetFeedDetailTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iGetFeedDetailTimeout_ms", "0");

    int iLiveCommentModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iLiveCommentModid", "0");
    int iLiveCommentCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iLiveCommentCmdid", "0");
    int iLiveCommentTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iLiveCommentTimeout_ms", "0");

    int iCompereModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iCompereModid", "0");
    int iCompereCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iCompereCmdid", "0");
    int iCompereTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iCompereTimeout_ms", "0");

    int iLivePollModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iLivePollModid", "0");
    int iLivePollCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iLivePollCmdid", "0");
    int iLivePollTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iLivePollTimeout_ms", "0");

    int iWelfareModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iWelfareModid", "0");
    int iWelfareCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iWelfareCmdid", "0");
    int iWelfareTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iWelfareTimeout_ms", "0");

    int iPromotionModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iPromotionModid", "0");
    int iPromotionCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iPromotionCmdid", "0");
    int iPromotionTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iPromotionTimeout_ms", "0");

    int iPidUnionModid = obj_conf.GetConfValue<int32_t>("Union", "iPidUnionModid", "0");
    int iPidUnionCmdid = obj_conf.GetConfValue<int32_t>("Union", "iPidUnionCmdid", "0");
    int iPidUnionTimeout_ms = obj_conf.GetConfValue<int32_t>("Union", "iPidUnionTimeout_ms", "0");

    int iUnionModid            = obj_conf.GetConfValue<int32_t>("Union","iUnionModid","0");
    int iUnionCmdid            = obj_conf.GetConfValue<int32_t>("Union","iUnionCmdid","0");
    int iUnionTimeout_ms       = obj_conf.GetConfValue<int32_t>("Union","iUnionTimeout_ms","0");

    string strRedisName        = obj_conf.GetConfValue<string>("Redis", "strRedisName", "");
    int iRedisTimeout_ms       = obj_conf.GetConfValue<int32_t>("Redis", "iRedisTimeout_ms", "");

    string strFtRedisName = obj_conf.GetConfValue<string>("Redis", "strFtRedisName", "");
    int iFtRedisTimeout_ms = obj_conf.GetConfValue<int32_t>("Redis", "iFtRedisTimeout_ms", "");

    int iUsrInfoModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iUsrInfoModid", "0");
    int iUsrInfoCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iUsrInfoCmdid", "0");
    int iUsrInfoTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iUsrInfoTimeout_ms", "0");

    int iGetRecommendModid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iGetRecommendModid", "0");
    int iGetRecommendCmdid = obj_conf.GetConfValue<int32_t>("ExternInterface", "iGetRecommendCmdid", "0");
    int iGetRecommendTimeout_ms = obj_conf.GetConfValue<int32_t>("ExternInterface", "iGetRecommendTimeout_ms", "0");

    string strRedisNameCms = obj_conf.GetConfValue<string>("Redis", "strRedisNameCms", "");
    int iRedisCmsTimeout_ms = obj_conf.GetConfValue<int32_t>("Redis", "iRedisCmsTimeout_ms", "");

    //初始化服务接口配置，task会自动根据taskType来设置目标链接信息
    g_oMgSvrConn.SetSvrConnInfo(UNION_GETVID_1006, iUnionModid   , iUnionCmdid   , iUnionTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(UNION_GETVID_2001, iUnionModid   , iUnionCmdid   , iUnionTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(UNION_GETSTAR, iUnionModid   , iUnionCmdid   , iUnionTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(UNION_GETDOKI, iUnionModid   , iUnionCmdid   , iUnionTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(UNION_GETPID , iPidUnionModid, iPidUnionCmdid, iPidUnionTimeout_ms);

    g_oMgSvrConn.SetSvrConnInfo(REDIS_SET_COUNT,strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_EXPIRE,strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_STR_SET , strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_SET_ADD , strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_SET_GET , strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_SSET_ADD, strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_SSET_DEL, strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_SSET_GET, strRedisName, iRedisTimeout_ms);

    g_oMgSvrConn.SetSvrConnInfo(REDIS_HASH_DEL, strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_HASH_ADD, strRedisName, iRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(REDIS_HASH_GET, strRedisName, iRedisTimeout_ms);

    g_oMgSvrConn.SetSvrConnInfo(GET_STAR_ADMIN,  iStarAdminModid, iStarAdminCmdid, iStarAdminTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_CONFIG_ADMIN, iConfigAdminModid, iConfigAdminCmdid, iConfigAdminTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_RIGHT_ADMIN, iRightAdminModid, iRightAdminCmdid, iRightAdminTimeout_ms);

    g_oMgSvrConn.SetSvrConnInfo(GET_DOKI_INFO,   iDokiInfoModid, iDokiInfoCmdid, iDokiInfoTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(FANS_ADAPT,      iFansAdaptModid, iFansAdaptCmdid, iFansAdaptTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_STAR_RANK,   iStarRankModid, iStarRankCmdid, iStarRankTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_USR_INFO,    iUsrInfoModid, iUsrInfoCmdid, iUsrInfoTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_FT_REDIS,  strFtRedisName, iFtRedisTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_RECOMMEND, iGetRecommendModid, iGetRecommendCmdid, iGetRecommendTimeout_ms);

    g_oMgSvrConn.SetSvrConnInfo(GET_CMS_SETTING, strRedisNameCms, iRedisCmsTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_FANS_ADMIN_CTT, iFansAdminCttModid, iFansAdminCttCmdid, iFansAdminCttTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_FEED_DEAIL, iGetFeedDetailModid, iGetFeedDetailCmdid, iGetFeedDetailTimeout_ms);

    g_oMgSvrConn.SetSvrConnInfo(LIVE_COMMENT, iLiveCommentModid, iLiveCommentCmdid, iLiveCommentTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(LIVE_COMPERE, iCompereModid, iCompereCmdid, iCompereTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(LIVE_POLL, iLivePollModid, iLivePollCmdid, iLivePollTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_WELFARE_WALLPAPER, iWelfareModid, iWelfareCmdid, iWelfareTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_WELFARE_EMOTICON, iWelfareModid, iWelfareCmdid, iWelfareTimeout_ms);
    g_oMgSvrConn.SetSvrConnInfo(GET_PROMOTION, iPromotionModid, iPromotionCmdid, iPromotionTimeout_ms);

    SPP_DEBUG_LOG("ConnInfo:\n %s", g_oMgSvrConn.OutConnInfo().c_str());

    //初始化monitor监控数据,task会自动上报monitor
    //g_oMgMoni.SetMonitorInfo(GET_RECOMMEND, GET_RECOMMEND_DATA_ALL, GET_RECOMMEND_DATA_FAIL, GET_RECOMMEND_DATA_RETERR, GET_RECOMMEND_DATA_SUCC, JCE_EN_ERR, JCE_DE_ERR);
    //g_oMgMoni.SetMonitorInfo(GET_FEED_DEAIL, GET_FEED_DETAIL_ALL, GET_FEED_DETAIL_FAIL, GET_FEED_DETAIL_RETERR, GET_FEED_DETAIL_SUCC, JCE_EN_ERR, JCE_DE_ERR);
    //g_oMgMoni.SetMonitorInfo(GET_PHOTO_QUALITY, GET_PHOTO_QUALITY_ALL, GET_PHOTO_QUALITY_FAIL, GET_PHOTO_QUALITY_RETERR, GET_PHOTO_QUALITY_SUCC, JCE_EN_ERR, JCE_DE_ERR);
    g_oMgMoni.SetMonitorInfo(UNION_GETSTAR, GET_DOKI_INFO_ALL, GET_DOKI_INFO_FAIL, GET_DOKI_INFO_RETERR, GET_DOKI_INFO_SUCC, JCE_EN_ERR, JCE_DE_ERR);
    //g_oMgMoni.SetMonitorInfo(UNION_GETPID , UNION_PID_ALL , UNION_PID_FAIL , 0, UNION_PID_SUCC , UNION_EN_ERR, UNION_DE_ERR);

    //初始化模调监控数据,task会自动上报模调

    return ;
}

/**
 * @brief
 * @note
 * @param  *pData:
 * @param  uiDataLen:
 * @retval
 */
static CMsgOptBase* CmdRouteVPacket(char *pData, uint32_t uiDataLen)
{
    int iRet = 0;

    // ====根据cmd路由对应的msg(新版本的videopacket可以不解包直接获取cmd)===================================
    // ====这里也可以根据app的版本或者平台路由，需要解析一次videopaket包，需要的话，业务自行处理===============

    //获取命令字
    unsigned short usCmd = 0;
    usCmd = CVideoPacket::getCommand((uint8_t *)pData, uiDataLen);
    if (0 == usCmd) //老版本的videopacket版本，没法快捷获取到cmd，只能解析一次请求包了
    {
        SPP_DEBUG_LOG("cmdRouteVPacket: GetCmd DecodeVideopacket");

        CVideoPacket oVideoPacket;
        oVideoPacket.set_packet((uint8_t *)pData, uiDataLen);
        iRet = oVideoPacket.decode();
        if (iRet != 0)
        {
            SPP_ERROR_LOG("cmdRouteVPacket: GetCmd VideoPacketDecodeError, iRet[%d]", iRet);
            return NULL;
        }
        usCmd = oVideoPacket.getCommand();
    }

    CMsgOptBase *msg = NULL;
    SPP_DEBUG_LOG("cmdRouteVPacket: GetCmd[%x]", usCmd);

    // ====根据cmd路由对应的msg===========================================================================
    switch (usCmd)
    {
        case CMD_CHANNEL:
        case CMD_SECOND_PAGE:
        {
            msg = new (nothrow)CMsgOptSquare();
            break;
        }

        default:
        {
            break;
        }
    }
    // ==================================================================================================

    if (!msg) //非本服务的命令字、或者new失败了
    {
        SPP_ERROR_LOG("cmdRouteVPacket: NoNeedCmd[0x%x] Or NewMsgError", usCmd);
        return NULL;
    }

    //拷贝有效数据包到msg中（避免videopacket的浅拷贝，这里将buf传递给msg，以重新解析videopacket包）
    iRet =  msg->SetMsgReqData(pData, uiDataLen);
    if (iRet != 0)
    {
        SPP_ERROR_LOG("cmdRouteVPacket: SetMsgVideoPacket DecodeError, iRet[%d]", iRet);
        return NULL;
    }

    return msg;
}

/**
 * @brief
 * @note
 * @param  *pData:
 * @param  uiDataLen:
 * @retval
 */
static CMsgOptBase* CmdRouteHttp(char *pData, uint32_t uiDataLen)
{
    int iRet = 0;

    //可以解析首行，根据cgi来路由不同的msg
    // string strReqCgi =  COptHttpData::GetCgiFromBuf(pData);
    //可以解析域名，根据域名来路由不同的msg
    // string strReqHost = COptHttpData::GetHeadFromBuf(pData,"Host");

    CMsgOptBase *msg = NULL;

    // ====路由对应的msg============================================================
    // msg = new (nothrow) CMsgOptHttp();
    // ============================================================================

    if (!msg) //new失败了
    {
        SPP_ERROR_LOG("cmdRouteHttp: newMsgError");
        return NULL;
    }

    //拷贝有效数据包到msg中
    iRet = msg->SetMsgReqData(pData, uiDataLen);
    if (iRet != 0)
    {
        SPP_ERROR_LOG("cmdRouteVPacket: SetMsgVideoPacket DecodeError, iRet[%d]", iRet);
        return NULL;
    }

    return msg;
}

/**
 * @brief
 * @note
 * @param  *blob:
 * @retval
 */
static CMsgOptBase* GetNewCMsg(blob_type *blob)
{
    int iRet = 0;

    char cFlag = *(blob->data); //获取包标志
    if (0x0a == cFlag) //是微博中转包的话，先解cspacket,再解videopacket
    {
        //解析cspacket包
        CCsPacket oCsPkg;
        oCsPkg.set_packet((uint8_t *)blob->data, blob->len);
        iRet = oCsPkg.decode();
        if (iRet != 0)
        {
            SPP_ERROR_LOG("getNewMsg: CsPacketDecodeError, iRet[%d]", iRet);
            return NULL;
        }

        return CmdRouteVPacket((char *)oCsPkg.getBody(), oCsPkg.getBodyLen());
    }
    else if (0x26 == cFlag) //视频通用包，videopacket包
    {
        return CmdRouteVPacket(blob->data, blob->len);
    }
    else if ('G' == cFlag || 'P' == cFlag) //http包
    {
        return CmdRouteHttp(blob->data, blob->len);
    }
    else
    {
        SPP_ERROR_LOG("getNewMsg: packetFlagInvalid,cFlag[0x%x]", (int)cFlag);
        return NULL;
    }

    SPP_ERROR_LOG("getNewMsg: logicOptError");
    return NULL;
}

#endif	// __MSG_INIT_ROOT_H__
