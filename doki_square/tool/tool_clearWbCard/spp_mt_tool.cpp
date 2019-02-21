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
#include "json.h"

//封装的spp通用task操作
#include "task_operate_redis.h"
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
using namespace StarTimeline;

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
    g_oMgSvrConn.SetSvrConnInfo(TASK_REDIS_FEED_INFO, "sz9210.ft_new_index.ssdb.com", 200);

    SPP_DEBUG_LOG("ConnInfo:\n %s",g_oMgSvrConn.OutConnInfo().c_str());

    return ;
}

int GetRedisData(const string &strStarId, string &strFeedId, StStarCardEvent &stEvent)
{
    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    string strRedisKey = "ft_star_talk_" + strStarId;
    COperateRedis *pGetStarIdx = new COperateRedis(TASK_REDIS_STAR_IDX);
    pGetStarIdx->SetTaskKey(strRedisKey);
    pGetStarIdx->SetReq_SortSet_GetRange_Value_ByRank(strRedisKey,0,0);
    oTaskExec.PushBack(pGetStarIdx);

    strRedisKey = "star_card_hash_" + strStarId;
    string strHashField = "WeiIns";
    COperateRedis *pGetStarWb = new COperateRedis(TASK_REDIS_STAR_CARD);
    pGetStarWb->SetTaskKey(strRedisKey);
    pGetStarWb->SetReq_Hash_GetValue_ByField(strRedisKey,strHashField);
    oTaskExec.PushBack(pGetStarWb);

    iRet = oTaskExec.RunTaskList();
    if(0 != iRet)
    {
        SPP_ERROR_LOG("GetRedisData: RunTaskError iExecRet[%d] ", iRet);
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
        else if(TASK_REDIS_STAR_CARD == pTask->GetTaskType())
        {
            COperateRedis *pGetStarWb = (COperateRedis*)pTask;
            if(0 != pGetStarWb->GetResult())
            {
                SPP_ERROR_LOG("GetStarWb: Error TaskKey[%s] TaskRet[%d]",pGetStarWb->GetTaskKey().c_str(),pGetStarWb->GetResult());
                continue;
            }
            if(REDIS_DTYPE_STR != pGetStarWb->GetRspData().eRedisDataType || pGetStarWb->GetRspData().strResult.empty())
            {
                SPP_DEBUG_LOG("GetStarWb: TaskKey[%s] valueType[%d] DataIsEmpty"
                   , pGetStarWb->GetTaskKey().c_str(),pGetStarWb->GetRspData().eRedisDataType);
                continue;
            }

            iRet = CJceCode::DecodeJcePkg(stEvent,pGetStarWb->GetRspData().strResult,strErrMsg);
            if(0 != iRet)
            {
                SPP_ERROR_LOG("GetStarWb: Error TaskKey[%s] jceDecodeErr errMsg[%s]",pGetStarWb->GetTaskKey().c_str(),strErrMsg.c_str());
            }
            else
            {
                SPP_DEBUG_LOG("GetStarWb: OptSucc TaskKey[%s] pubTime[%lld]idtype[%d]picSize[%u]"
                    ,pGetStarWb->GetTaskKey().c_str(),stEvent.llStartTime,stEvent.iSubType,stEvent.vecPicUrl.size());
            }
        }
    }

    return 0;
}

int GetFeedSSDPic(const string &strFeedId, string &strFeedPic)
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

        string strPic;
        uiPos1 = strRetData.find("picIdx[0]: PicUrl[");
        if (string::npos != uiPos1)
        {
            uiPos1 = uiPos1 + sizeof("picIdx[0]: PicUrl[") - 1; //sizeof会计算字符串“\0”结尾的,需要减一
            size_t uiPos2 = strRetData.find("]", uiPos1);
            if (string::npos != uiPos2)
                strPic = strRetData.substr(uiPos1, uiPos2 - uiPos1);
        }

        string strIdentity;
        uiPos1 = strRetData.find("MgrInfo: identity[");
        if (string::npos != uiPos1)
        {
            uiPos1 = uiPos1 + sizeof("MgrInfo: identity[") - 1; //sizeof会计算字符串“\0”结尾的,需要减一
            size_t uiPos2 = strRetData.find("]", uiPos1);
            if (string::npos != uiPos2)
                strIdentity = strRetData.substr(uiPos1, uiPos2 - uiPos1);
        }

        if("2" == strUserType &&  !strPic.empty() && "11" != strIdentity)
        {
            strFeedPic = strPic;
        }
        else
        {
            SPP_DEBUG_LOG("GetFeedInfo: feedid[%s]userType[%s]Identity[%s]pic[%s]",strFeedId.c_str(),strUserType.c_str(),strIdentity.c_str(),strPic.c_str());
        }
    }

    return 0;
}

int GetFeedModifyPic(const string &strFeedId, string &strPicUrl)
{
    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    string strRedisKey  = "ft_messagecnt_" + strFeedId;
    string strHashField = "imagelist";

    //先读取之前的数据，判断时间，存储的时间较新，则不更新记录
    COperateRedis *pGetWbIns = new COperateRedis(TASK_REDIS_FEED_INFO);
    pGetWbIns->SetTaskKey(strRedisKey);
    pGetWbIns->SetReq_Hash_GetValue_ByField(strRedisKey,strHashField);
    oTaskExec.PushBack(pGetWbIns);

    iRet = oTaskExec.RunTaskList();
    if(0 != iRet)
    {
        SPP_ERROR_LOG("GetFeedInfo: RunTaskError iExecRet[%d] ", iRet);
        return iRet;
    }

    iRet = pGetWbIns->GetResult();
    if(0 != iRet)
    {
        SPP_ERROR_LOG("GetFeedInfo: Error TaskKey[%s] TaskRet[%d]",pGetWbIns->GetTaskKey().c_str(),iRet);
        return iRet;
    }
    if(REDIS_DTYPE_STR != pGetWbIns->GetRspData().eRedisDataType || pGetWbIns->GetRspData().strResult.empty())
    {
        SPP_DEBUG_LOG("GetFeedInfo: TaskKey[%s] valueType[%d] DataIsEmpty"
            , pGetWbIns->GetTaskKey().c_str(),pGetWbIns->GetRspData().eRedisDataType);
        return 0;
    }

    Json::Reader jsonReader;
    Json::Value  jsonStarInfo;
    if(jsonReader.parse(pGetWbIns->GetRspData().strResult,jsonStarInfo) && jsonStarInfo.isArray())
    {
        for(Json::Value::UInt uiIdx = 0; uiIdx < jsonStarInfo.size(); uiIdx++)
        {
            Json::Value &jsonValue = jsonStarInfo[uiIdx];

            DEF_GET_JSON_STR(strPicUrl,"url"); //当前帖子的修改，没有保存缩略图，只能取原图的url的，后续帖子的修改会支持保存缩略图的

            break;
        }
    }
    SPP_DEBUG_LOG("GetFeedInfo: key[%s]url[%s]",strRedisKey.c_str(),strPicUrl.c_str());

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

    return 0;
}

// ====mian函数==========================================================
int main(int argc, const char * argv[])
{
    //=====读取配置文件,初始化日志=====================================================
    InitInfo("./tool_clearWbCard.conf");//加载配置文件

    //=====读取命令行参数=============================================================

    //=====初始化微线程框架===========================================================
    bool bInitMt = mt_init_frame();
    if (!bInitMt)
    {
        SPP_ERROR_LOG("init micro thread frame failed.");
        return -1;
    }

    mt_sleep(0);//触发微线程切换

    //主体处理逻辑
    //1. 从文件中读取明星id
    //2. 从明星索引里面读取最新几条feedid
    //3. 读取帖子详情，判断是否是微博发表的，
    //4. 读取明星卡片的数据，判断是否需要更新数据

    //1. 从文件中读取明星id
    vector<string> vecStarId;
    ifstream inFile;
    inFile.open("./wb_starid");
    while(inFile)
    {
        string strLine;
        getline(inFile,strLine);
        if(strLine.empty())
            continue;

        vecStarId.push_back(strLine);
    }
    inFile.clear();
    inFile.close();
    SPP_DEBUG_LOG("GetReqStarid: size=%u",vecStarId.size());

    int iRet = 0;
    for(size_t uiIdx = 0; uiIdx < vecStarId.size(); uiIdx++)
    {
        const string &strStarId = vecStarId[uiIdx];

        //读取明星说索引
        //读取明星卡片数据
        string strFeedId;
        StStarCardEvent stEvent;
        iRet = GetRedisData(strStarId, strFeedId, stEvent);
        if(0 != iRet)
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], error[%d]",strStarId.c_str(),iRet);
            continue;
        }
        else if(strFeedId.empty())
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], StarTalkdIsEmpty",strStarId.c_str());
            continue;
        }
        else if(stEvent.strStarId.empty())
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], CardStarIdIsEmpty",strStarId.c_str());
            continue;
        }
        else if(stEvent.vecPicUrl.empty())
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], CardPicIsEmpty",strStarId.c_str());
            continue;
        }
        else if(11 ==  stEvent.iSubType) //11是ins，其他的认为是微博
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], CardIsNotWb",strStarId.c_str());
            continue;
        }

        string strWbCardPic = stEvent.vecPicUrl[0];
        if(strWbCardPic.empty())
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], strWbCardPicIsEmpty",strStarId.c_str());
            continue;
        }

        //读取帖子详情
        string strFeedPic;
        GetFeedSSDPic(strFeedId, strFeedPic);
        if(strFeedPic.empty())
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], FeedPicIsEmpty",strStarId.c_str());
            continue;
        }

        //相等的才换成缩略图,不相等的是遗漏的产品逻辑问题，需要重新洗数据
        bool bIsEqual = strWbCardPic==strFeedPic;
        if(!bIsEqual)
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], no_equal wbCardPic[%s]starIdxSSDPic[%s]starIdxFeedid[%s]"
                ,strStarId.c_str(),strWbCardPic.c_str(),strFeedPic.c_str(),strFeedId.c_str());
            continue;
        }

        //从redis扩展中读取修改过的帖子的图片数据
        string strRedisFeedPic;
        GetFeedModifyPic(strFeedId, strRedisFeedPic);
        if(strRedisFeedPic.empty())
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], RedisPicIsEmpty",strStarId.c_str());
            continue;
        }

        if(strWbCardPic==strRedisFeedPic) //卡片数据和redis数据是一样的，不用修改
        {
            SPP_FATAL_LOG("OptData: DoNot_starId[%s], equal wbCardPic[%s]redisPic[%s]starIdxFeedid[%s]"
                ,strStarId.c_str(),strWbCardPic.c_str(),strRedisFeedPic.c_str(),strFeedId.c_str());
            continue;
        }

        stEvent.llFeedId = atoll(strFeedId.c_str());
        stEvent.vecPicUrl.clear();
        stEvent.vecPicUrl.push_back(strRedisFeedPic);

        iRet = SetRedisWb(strStarId,stEvent);
        if(0 != iRet)
        {
            SPP_FATAL_LOG("OptData: DoErr_starId[%s], wbCardPic[%s]redisPic[%s]starIdxFeedid[%s]"
                ,strStarId.c_str(),strWbCardPic.c_str(),strRedisFeedPic.c_str(),strFeedId.c_str());
            continue;
        }

        SPP_FATAL_LOG("OptData: DoSucc_starId[%s], wbCardPic[%s]redisPic[%s]starIdxFeedid[%s]"
            ,strStarId.c_str(),strWbCardPic.c_str(),strRedisFeedPic.c_str(),strFeedId.c_str());

        printf("OptData: DoSucc_starId[%s], wbCardPic[%s]redisPic[%s]starIdxFeedid[%s]\n"
            ,strStarId.c_str(),strWbCardPic.c_str(),strRedisFeedPic.c_str(),strFeedId.c_str());
    }

    return 0;
}
