/*
 *  Filename: spp_mt_tool.cpp
 *
 *  Created on: 2016-11-09
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2016 Tencent. All rights reserved.
 *
 *
 * 	Description: spp微线程框架,单独使用微线程写静态程序
 *
 */
#include <iostream>
#include "config.h"         //配置文件读取库
#include <fstream>
#include <unistd.h>
#include <stdint.h>

//封装的spp通用task操作
#include "task_operate_redis.h"
#include "task_operate_jcedata.h"
#include "task_exec.h"

//工具函数
#include "myUtils.h"
#include "myUtilsTVideo.h"
using namespace MyUtils;

//项目基本头文件
#include "task_commdef_proj.h"

//jce协议文件
#include "jce/Jce.h"		//jce协议库
#include "star_timeline.h"
#include "qqvideo_interface.h"
#include "qqvideo_message.h"
#include "qqvideo_fantuan.h"
using namespace StarTimeline;

#define CMD_GET_FEED_LINE             0xf085 //请求帖子详情(列表显示时使用，显示的是摘要信息,无回复评论数据)

void InitInfo(const char *etc)
{
    CConfigFile obj_conf(etc);//加载配置文件

    //初始化日志
    string strLogPathName  = obj_conf.GetConfValue<string> ("VLog","strLogPathName","");
    int    iLogLevel       = obj_conf.GetConfValue<int32_t>("VLog","iLogLevel","0");
    int    iLogFileMaxNum  = obj_conf.GetConfValue<int32_t>("VLog","iLogFileMaxNum","0");
    int    iLogFileMaxSize = obj_conf.GetConfValue<int32_t>("VLog","iLogFileMaxSize","0");
    int    iLogIsByLevel   = obj_conf.GetConfValue<int32_t>("VLog","iLogIsByLevel","0");

    g_vlog.setFileName(strLogPathName);
    g_vlog.setLevel(iLogLevel);
    g_vlog.setMaxNum(iLogFileMaxNum);
    g_vlog.setMaxSize(iLogFileMaxSize);
    g_vlog.setByLevel(iLogIsByLevel);
    SPP_DEBUG_LOG("logInitInfo: logName[%s]level[%d]maxNum[%d]mszSize[%d]logByLevel[%d]"
        ,strLogPathName.c_str(),iLogLevel,iLogFileMaxNum,iLogFileMaxSize,iLogIsByLevel);

    //初始化服务接口配置，task会自动根据taskType来设置目标链接信息

    g_oMgSvrConn.SetSvrConnInfo(TASK_REDIS_STAR_CARD, "sz1149.star_timeline_cardlist.redis.com", 200);
    g_oMgSvrConn.SetSvrConnInfo(TASK_REDIS_STAR_IDX, "sz1364.doki_star_talk_index.redis.com", 200);
    g_oMgSvrConn.SetSvrConnInfo(GET_FEED_LINE, 818753, 196608, 400);

    SPP_DEBUG_LOG("ConnInfo:\n %s",g_oMgSvrConn.OutConnInfo().c_str());

    return ;
}

int GetStarIdxFeedId(const string &strStarId, string &strFeedId)
{
    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    string strRedisKey = "ft_star_talk_" + strStarId;
    COperateRedis *pGetStarIdx = new COperateRedis(TASK_REDIS_STAR_IDX);
    pGetStarIdx->SetTaskKey(strRedisKey);
    pGetStarIdx->SetReq_SortSet_GetRange_Value_ByRank(strRedisKey,0,0);
    oTaskExec.PushBack(pGetStarIdx);

    iRet = oTaskExec.RunTaskList();
    if(0 != iRet)
    {
        SPP_ERROR_LOG("GetStarIdxFeedId: RunTaskError iExecRet[%d] ", iRet);
        return iRet;
    }

    for(size_t uiIdx = 0; uiIdx < oTaskExec.GetVecPtrTask().size(); ++uiIdx)
    {
        IMtTask *pTask = oTaskExec.GetVecPtrTask()[uiIdx];

        if(TASK_REDIS_STAR_IDX == pTask->GetTaskType())
        {
            COperateRedis *pGetStarIdx = (COperateRedis*)pTask;
            if(0 != pGetStarIdx->GetResult())
            {
                SPP_ERROR_LOG("GetStarIdx: Error TaskKey[%s] TaskRet[%d]",pGetStarIdx->GetTaskKey().c_str(),pGetStarIdx->GetResult());
                continue;
            }
            if(REDIS_DTYPE_VEC_STR != pGetStarIdx->GetRspData().eRedisDataType || pGetStarIdx->GetRspData().vecStrResult.empty())
            {
                SPP_DEBUG_LOG("GetStarIdx: TaskKey[%s] valueType[%d] DataIsEmpty"
                   , pGetStarIdx->GetTaskKey().c_str(),pGetStarIdx->GetRspData().eRedisDataType);
                continue;
            }

            strFeedId = pGetStarIdx->GetRspData().vecStrResult[0];
            SPP_DEBUG_LOG("GetStarIdx: starId[%s]strFeedId[%s]",strStarId.c_str(),strFeedId.c_str());
        }
    }

    return 0;
}

int GetFeedIdentity(const string &strFeedId, string &strIdentity)
{
    FILE *fp;
    string strCmd = "./gettweet get 333333 " + strFeedId;
    fp = popen(strCmd.c_str(), "r");
    if (NULL == fp)
    {
        SPP_ERROR_LOG("getstarid: optError cmd[%s] ", strCmd.c_str());
        return -1;
    }
    else
    {
        char buffer[2048] = {0};
        fread(buffer, 1, sizeof(buffer), fp);
        pclose(fp);

        string strRetData(buffer);

        string strUserType;
        size_t uiPos1 = strRetData.find("UserInfo: usertype[");
        if (string::npos != uiPos1)
        {
            uiPos1 = uiPos1 + sizeof("UserInfo: usertype[") - 1; //sizeof会计算字符串“\0”结尾的,需要减一
            size_t uiPos2 = strRetData.find("]", uiPos1);
            if (string::npos != uiPos2)
                strUserType = strRetData.substr(uiPos1, uiPos2 - uiPos1);
        }

        string strFeedIdentity;
        uiPos1 = strRetData.find("MgrInfo: identity[");
        if (string::npos != uiPos1)
        {
            uiPos1 = uiPos1 + sizeof("MgrInfo: identity[") - 1; //sizeof会计算字符串“\0”结尾的,需要减一
            size_t uiPos2 = strRetData.find("]", uiPos1);
            if (string::npos != uiPos2)
                strFeedIdentity = strRetData.substr(uiPos1, uiPos2 - uiPos1);
        }

        SPP_DEBUG_LOG("GetFeedInfo: feedid[%s]userType[%s]Identity[%s]",strFeedId.c_str(),strUserType.c_str(),strFeedIdentity.c_str());

        if("2" == strUserType) //只取微博和ins的数据
        {
            strIdentity = strFeedIdentity;
        }
    }

    return 0;
}

int GetFeedDetail(const string &strFeedId, jce::CirclePrimaryFeed &stFeed)
{
    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    jce::FeedDetailRequest stFeedReq;
    stFeedReq.dataKey = "id=" + strFeedId + "&scene=videocomment";

    CVideoPacket oVideoPacket;

    COperateJceData<jce::FeedDetailResponse> *pGetFeed = new COperateJceData<jce::FeedDetailResponse>(GET_FEED_LINE,BUF_LEN_8K);
    pGetFeed->SetTaskKey(strFeedId);
    pGetFeed->SetReqData(stFeedReq, CMD_GET_FEED_LINE, oVideoPacket);
    pGetFeed->SetRspJceRetCode((int*)&(pGetFeed->GetRspJceData().errCode));
    oTaskExec.PushBack(pGetFeed);

    iRet = oTaskExec.RunTaskList();
    if(0 != iRet)
    {
        SPP_ERROR_LOG("PidInfo_StarComment_FansData: RunTaskError iExecRet[%d] ", iRet);
        return iRet;
    }

    if(0 != pGetFeed->GetResult())
    {
        SPP_ERROR_LOG("GetStaCommentFeedDetail: taskOptRet[%d] taskKey[%s]",pGetFeed->GetResult(), pGetFeed->GetTaskKey().c_str());
        return iRet;
    }
    stFeed = pGetFeed->GetRspJceData().feedInfo;

    SPP_DEBUG_LOG("GetStaCommentFeedDetail: Succ, starid[%s]feedid[%s] text[%s]picSize[%u]"
        , pGetFeed->GetTaskKey().c_str(), stFeed.feedId.c_str(),stFeed.content.c_str(),stFeed.photos.size());

    return 0;
}

int SetRedisWb(const string &strStarId,const StStarCardEvent &stEvent)
{
    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    string strRedisKey  = "star_card_hash_" + strStarId;
    string strHashField = "WeiIns";

    string strRedisValue;
    iRet = CJceCode::EncodeJcePkg(strRedisValue,stEvent,strErrMsg);
    if(0 != iRet)
    {
        SPP_ERROR_LOG("WeiboIns_TransPub: JceEncodeError[%d],ErrMsg[%s]",iRet,strErrMsg.c_str());
        return iRet;
    }

    COperateRedis *pAddRedis = new COperateRedis(TASK_REDIS_STAR_CARD);
    pAddRedis->SetTaskKey(strRedisKey + "||" + strHashField);
    pAddRedis->SetReq_Hash_Add(strRedisKey,strHashField,strRedisValue);
    oTaskExec.PushBack(pAddRedis);

    oTaskExec.RunTaskList();

    SPP_DEBUG_LOG("WeiboIns_TransPub: oprRedisRet[%d] taskKey[%s]",pAddRedis->GetResult(),pAddRedis->GetTaskKey().c_str());
    return 0;
}

int PrintDemo()
{
    cout<<"demo:"<<endl;
    cout<<"./tool_updateWbCard starid"<<endl;

    return 0;
}

// ====mian函数==========================================================
int main(int argc, const char * argv[])
{
    //=====读取配置文件,初始化日志=====================================================
    InitInfo("./tool_updateWbCard.conf");//加载配置文件

    //=====读取命令行参数=============================================================
    if(argc < 2)
        return PrintDemo();

    string strStarId = string(argv[1]);

    //=====初始化微线程框架===========================================================
    bool bInitMt = mt_init_frame();
    if (!bInitMt)
    {
        SPP_ERROR_LOG("init micro thread frame failed.");
        return -1;
    }

    mt_sleep(0);//触发微线程切换

    //主体处理逻辑

    //读取明星说索引
    string strFeedId;
    int iRet = GetStarIdxFeedId(strStarId, strFeedId);
    if(0 != iRet)
    {
        printf("OptData: DoNot_starId[%s], error[%d]\n",strStarId.c_str(),iRet);
        return 0;
    }
    else if(strFeedId.empty())
    {
        printf("OptData: DoNot_starId[%s], StarTalkdIsEmpty\n",strStarId.c_str());
        return 0;
    }

    //判断帖子是否是wb数据
    string strIdentity;
    GetFeedIdentity(strFeedId, strIdentity);
    if(strIdentity.empty())
    {
        printf("OptData: DoNot_starId[%s], feedIdentityIsEmpty\n",strStarId.c_str());
        return 0;
    }

    //读取帖子详情，取文本内容和图片
    jce::CirclePrimaryFeed stFeed;
    GetFeedDetail(strFeedId, stFeed);
    if(stFeed.feedId.empty())
    {
        printf("OptData: DoNot_starId[%s], detailFeedIdIsEmpty\n",strStarId.c_str());
        return 0;
    }

    StarTimeline::StStarCardEvent stEvent;
    stEvent.strStarId   = strStarId;
    stEvent.strText     = stFeed.content;
    stEvent.llStartTime = stFeed.time;
    stEvent.llEndTime   = stEvent.llStartTime + 63072000; //730*86400, 生效时间时间为2年，产品jujubezhang制定的策略
    stEvent.iSubType    = atoi(strIdentity.c_str());
    stEvent.llFeedId    = atoll(strFeedId.c_str());

    string strPicUrl;
    for(size_t uiIdx = 0; uiIdx < stFeed.photos.size(); ++uiIdx)
    {
        jce::CircleMsgImageUrl &stPic = stFeed.photos[uiIdx];

        //优先使用小图，小图为空才使用大图
        if(!stPic.thumbUrl.empty())
            strPicUrl = stPic.thumbUrl;
        else
            strPicUrl  = stPic.url;

        stEvent.vecPicUrl.push_back(strPicUrl);

        break; //当前只需要一张图片，这里也只存储一张图片即可
    }

    iRet = SetRedisWb(strStarId,stEvent);
    if(0 != iRet)
    {
        printf("OptData: DoErr_starId[%s], starIdxFeedid[%s]\n",strStarId.c_str(),strFeedId.c_str());
        return 0;
    }

    SPP_FATAL_LOG("OptData: optSucc starId[%s], feedid[%s] time[%lld]content[%s]subtype[%d]cardFeed[%lld]pic[%s]"
        ,strStarId.c_str(),strFeedId.c_str(),stEvent.llStartTime,stEvent.strText.c_str(),stEvent.iSubType,stEvent.llFeedId,strPicUrl.c_str());

    printf("OptData: optSucc starId[%s], feedid[%s] time[%lld]content[%s]subtype[%d]cardFeed[%lld]pic[%s]\n"
        ,strStarId.c_str(),strFeedId.c_str(),stEvent.llStartTime,stEvent.strText.c_str(),stEvent.iSubType,stEvent.llFeedId,strPicUrl.c_str());

    return 0;
}
