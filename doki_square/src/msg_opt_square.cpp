/*
 *  Filename: msg_opt_starcard.cpp
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: iphone和aphone平台600之后版本、ipad570之后版本的明星卡片服务
 *      注意： 这里的逻辑本来很纯粹的，产品给101做了一堆的特殊处理，同时ipad和phone端在101活动上实现不一致，导致代码里面很多业务逻辑的判断。
 *            增加了业务复杂性，同时也导致cache逻辑需要做兼容。
 *
 */

#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <algorithm>

#include "msg_opt_square.h"
#include "json.h"

// ====继承类需要实现以下几个虚函数，定义各自的设置请求数据、解包校验操作、处理操作、回包操作=========================
int CMsgOptSquare::SetMsgReqData(char *pData, uint32_t uiLen)
{
    m_stVideoPacket.set_packet((uint8_t *)pData, uiLen);
    int iRet = m_stVideoPacket.decode();
    if (iRet != 0)
    {
        SPP_ERROR_LOG("setMsgReqData: VideoPacketDecodeError, iRet[%d]", iRet);
        return -1;
    }

    m_uiCmd = m_stVideoPacket.getCommand();
    return 0;
}

int CMsgOptSquare::EncodeAndCheckReqData()
{
    int iRet = 0;
    string strErrMsg;
    m_lVuid = m_stVideoPacket.getLoginUserid();

    // 不需解包的基础数据
    {
        m_strReqKey = CUserLoginInfo::GetUserKeyFromVPacket(m_stVideoPacket);
        m_iPlatform = m_stVideoPacket.getQUAInfo().platform;

        // ABTest配置信息：http://tapd.oa.com/VideoServerDev/markdown_wikis/view/#1010095981008175541
        m_iBucketId = m_stVideoPacket.getQUAInfo().extentData.bucketInfo.bucketId;
        string strPolicyId = m_stVideoPacket.getQUAInfo().extentData.bucketInfo.strategyId;
        //m_iPolicyId = qqvideo::api::BucketApi::GetStrategyId(iBucketId, 48);
        m_iPolicyId = atoi(strPolicyId.c_str());

        string strAppVer = m_stVideoPacket.getQUAInfo().versionName;
        m_bIsPhone = (m_iPlatform == PLATFORM_APHONE) || (m_iPlatform == PLATFORM_IPHONE);

        SPP_MSG_DEBUG_LOG("plat[%d] sppver[%s]", m_iPlatform, strAppVer.c_str());
        SPP_MSG_DEBUG_LOG("pack iBucketId[%d] pack policyId[%s] fromBucketAPI policyId[%d]", m_iBucketId, strPolicyId.c_str(), m_iPolicyId);

        if (m_bIsPhone && CJudgeAppVerBy3Slice::IsBiggerAndEqual(strAppVer, "6.6.8"))
        {
            // 668以下的变量都要 设置
            m_bIsVerGe668 = true;
            m_bIsVerGe658 = true;
        }
        else if (m_bIsPhone && CJudgeAppVerBy3Slice::IsBiggerAndEqual(strAppVer, "6.5.8"))
        {
            m_bIsVerGe658 = true;
        }
        else {
            // 默认值false
        }
    }

    switch (m_uiCmd)
    {
        case CMD_CHANNEL:
        {
            Attr_API(CHANNEL_REQ_ALL, 1);

            iRet = CJceCode::DecodeJcePkg(m_jceChannelReq, m_stVideoPacket.getBody(), strErrMsg);
            SPP_MSG_DEBUG_LOG("channelId[%s] dataKey[%s] pageContext[%s]", 
                m_jceChannelReq.channelItemId.c_str(), m_jceChannelReq.dataKey.c_str(), m_jceChannelReq.pageContext.c_str());

            if (m_jceChannelReq.channelItemId != "100224")
            {
                SPP_MSG_ERROR_LOG("invalid channel id");
                m_iError = CErrCodeProj::ERR_REQUEST_INVALID_PARAM;
                return -1;
            }

            if (!m_bIsPhone || !m_bIsVerGe658)
            {
                SPP_MSG_ERROR_LOG("invalid param, m_bIsPhone[%d] m_bIsVerGe658[%d]", m_bIsPhone, m_bIsVerGe658);
                m_iError = CErrCodeProj::ERR_REQUEST_INVALID_PARAM;
                return -1;
            }

            m_strChannelId = m_jceChannelReq.channelItemId;
            break;
        }

        case CMD_SECOND_PAGE:
        {
            Attr_API(SECOND_PAGE_REQ_ALL, 1);

            iRet = CJceCode::DecodeJcePkg(m_jceSecondPageReq, m_stVideoPacket.getBody(), strErrMsg);
            SPP_MSG_DEBUG_LOG("datakey[%s] pageContext[%s]",
                m_jceSecondPageReq.dataKey.c_str(), m_jceSecondPageReq.pageContext.c_str());

            if (m_jceSecondPageReq.dataKey.empty())
            {
                SPP_MSG_ERROR_LOG("ERR_REQUEST_INVALID_PARAM");
                m_iError = CErrCodeProj::ERR_REQUEST_INVALID_PARAM;
                return -1;
            }

            break;
        }

        default:
        {
            SPP_MSG_ERROR_LOG("reqDataError: cmd[%x] Error",m_uiCmd);
            m_iError = CErrCodeProj::ERR_REQUEST_CMD_INVALID;
            return m_iError;
        }
    }

    // 用于日志打印
    m_bLoginUser = (m_strReqKey.empty() || m_strReqKey.find("GUID") != string::npos) ? false : true;

    if (0 != iRet)
    {
        Attr_API(JCE_DE_ERR, 1);
        SPP_MSG_ERROR_LOG("reqDataError: jceDecodeError, iRet[%d] strErrMsg[%s]", iRet, strErrMsg.c_str());
        m_iError = CErrCodeProj::ERR_REQUEST_JCE_DECODE;
        return -1;
    }

    return 0;
}

void CMsgOptSquare::SendRspData()
{
    switch(m_uiCmd)
    {
        case CMD_CHANNEL:
        {
            if (m_iError != 0) {
                m_jceChannelRsp.errCode = m_iError;
            }
            else {
                Attr_API(CHANNEL_REQ_SUCC, 1);
            }

            if (m_jceChannelRsp.data.empty())
                Attr_API(CHANNEL_REQ_EMPTY, 1);

            SendRspDataVPacket(m_jceChannelRsp, m_stVideoPacket);
            break;
        }

        case CMD_SECOND_PAGE:
        {
            if (m_iError != 0) {
                m_jceSecondPageRsp.errorCode = m_iError;
            }
            else{
                Attr_API(SECOND_PAGE_SUCC, 1);
            }

            if (m_jceSecondPageRsp.uiData.empty())
                Attr_API(SECOND_PAGE_EMPTY, 1);

            SendRspDataVPacket(m_jceSecondPageRsp, m_stVideoPacket);
            break;
        }

        default:
        {
            SendRspDataEmpty();
            break;
        }
    }

    return ;
}

int CMsgOptSquare::DealProcess()
{
    switch (m_uiCmd)
    {
        case CMD_CHANNEL:
        {
           return HandleChannelPage();
        }

        case CMD_SECOND_PAGE:
        {
           return HandleSecondPage();
        }
    }

    return 0;
}

// =====业务逻辑处理相关函数=====================================================================================
int CMsgOptSquare::HandleChannelPage()
{
    int iRet = 0;
    struct timeval  tmStart;
    struct timeval  tmEnd;

    gettimeofday(&tmStart, NULL);
    UniRecInterface::RecReq stReq;

    // 解析翻页参数
    ParseDatakey(m_jceChannelReq.pageContext, m_stDatakey);

    // 1\ 构建推荐请求包
    iRet = ConstructReqPack(stReq);
    if (iRet != 0)
    {
        SPP_MSG_ERROR_LOG("return iRet[%d] ", iRet);
        return 0;
    }

    // 2\ 获取推荐数据
    iRet = GetRecommend(stReq, m_stRecRsp);
    if (iRet != 0)
    {
        SPP_MSG_ERROR_LOG("getRecommend failed. iRet[%d] rec_modules.size[%d]", iRet, m_stRecRsp.rec_modules.size());

        m_iError = CErrCodeProj::ERR_REQ_RECOMMEND_FAIL;
        return 0;
    }

    // 整理推荐数据 -- 纯计算
    ProcessRecData(m_stRecRsp);

    // 3\ 读取更多模块数据，譬如十佳帖数据、焦点图、DOKI信息
    Channel_PrepareData_1();

    // 4\ 读取更多模块数据，譬如帖子详情信息、pid、vid信息
    Channel_PrepareData_2();

    // 5\ 读取视频union数据
    Channel_PrepareData_3();

    // 6\ 构造回包数据
    ConstructChannelData();

    gettimeofday(&tmEnd, NULL);
    MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_MOD_ID, MCALL_IF_CHANNEL, m_iError);
    return 0;
}

int CMsgOptSquare::HandleSecondPage()
{
    int iRet = 0;
    struct timeval  tmStart;
    struct timeval  tmEnd;

    gettimeofday(&tmStart, NULL);
    UniRecInterface::RecReq stReq;

    // 解析翻页参数
    ParseDatakey(m_jceSecondPageReq.pageContext, m_stDatakey);
    ParseDatakey(m_jceSecondPageReq.dataKey, m_stDatakey);

    // 请求参数检查
    if (m_stDatakey.modid < E_MODID_FEED ||
        m_stDatakey.modid > E_MODID_IP_GROUP)
    {
        SPP_MSG_ERROR_LOG("modid[%d], cmsid[%s], datakey[%s]", m_stDatakey.modid, m_stDatakey.strCMSId.c_str(), m_jceSecondPageReq.dataKey.c_str());

        m_iError = CErrCodeProj::ERR_REQUEST_INVALID_PARAM;
        return -1;
    }

    // 1\ 构建推荐请求包
    iRet = ConstructReqPack(stReq);
    if (iRet != 0)
    {
        SPP_MSG_ERROR_LOG("return iRet[%d] ", iRet);
        return 0;
    }

    // 2\ 获取推荐数据
    iRet = GetRecommend(stReq, m_stRecRsp);
    if (iRet != 0)
    {
        SPP_MSG_ERROR_LOG("getRecommend failed. iRet[%d] rec_modules.size[%d]", iRet, m_stRecRsp.rec_modules.size());

        m_iError = CErrCodeProj::ERR_REQ_RECOMMEND_FAIL;
        return 0;
    }

    // 整理推荐数据 -- 纯计算
    ProcessRecData(m_stRecRsp);

    // 复用频道页函数， 如有不一致，则另起一个函数
    // 3\ 读取更多模块数据，譬如十佳帖数据、焦点图、DOKI信息
    Channel_PrepareData_1();

    // 4\ 读取更多模块数据，譬如帖子详情信息、pid、vid信息
    Channel_PrepareData_2();

    // 5\ 读取视频union数据
    Channel_PrepareData_3();

    // 6\ 构造回包数据
    ConstructSecondPage();

    gettimeofday(&tmEnd, NULL);
    MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_MOD_ID, MCALL_IF_SECOND_PAGE, m_iError);

    return 0;
}

int CMsgOptSquare::GetRecommend(const UniRecInterface::RecReq& stReq, UniRecInterface::RecRsp& stRsp)
{
    int iRet = 0, iMCallIf = MCALL_IF_RECOMMEND_CHANNEL;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    struct timeval  tmStart;
    struct timeval  tmEnd;
    gettimeofday(&tmStart, NULL);
    Attr_API(GET_RECOMMEND_REQ_ALL, 1);

    if (IsSecondPageView())
    {
        iMCallIf = MCALL_IF_RECOMMEND_SECOND_PAGE;
    }

    COperateJceData<UniRecInterface::RecRsp> *pGetRecommend = new COperateJceData<UniRecInterface::RecRsp>(GET_RECOMMEND, BUF_LEN_128K);
    pGetRecommend->SetReqData(stReq, 0xf7aa, m_stVideoPacket);
    pGetRecommend->SetRspJceRetCode((int*)&(pGetRecommend->GetRspJceData().status.rsp_code));
    oTaskExec.PushBack(pGetRecommend);

    iRet = oTaskExec.RunTaskList();
    if (0 != iRet)
    {
        SPP_MSG_ERROR_LOG("GetData_1: RunTaskError iExecRet[%d] ", iRet);
        AppendBossKVData("recTaskErr=" + CNum2Str::Num2Str(iRet));

        Attr_API(GET_RECOMMEND_REQ_FAIL, 1);
        gettimeofday(&tmEnd, NULL);
        MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_MOD_ID, iMCallIf, iRet);

        return iRet;
    }

    for (size_t uiIdx = 0; uiIdx < oTaskExec.GetVecPtrTask().size(); ++uiIdx)
    {
        IMtTask *pTask = oTaskExec.GetVecPtrTask()[uiIdx];

        if (GET_RECOMMEND == pTask->GetTaskType())
        {
            COperateJceData<UniRecInterface::RecRsp> *pJceRsp = (COperateJceData<UniRecInterface::RecRsp>*)pTask;
            if (0 != pJceRsp->GetResult())
            {
                if (pJceRsp->GetResult() < 0)
                {
                    Attr_API(GET_RECOMMEND_REQ_RPC_FAIL, 1);
                }
                else if (pJceRsp->GetResult() == 1104) // 推荐空数据
                {
                    // 空数据
                    Attr_API(GET_RECOMMEND_REQ_EMPTY, 1);

                    // 无推荐数据， 不报错给APP，但记录错误用来设置下一页标记位
                    m_iErrInner = CErrCodeProj::ERR_REQ_RECOMMEND_EMPTY;
                    AppendBossKVData("recRet=empty");

                    gettimeofday(&tmEnd, NULL);
                    MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_MOD_ID, iMCallIf, pJceRsp->GetResult());
                    return 0;
                }
                else {
                    Attr_API(GET_RECOMMEND_REQ_FAIL, 1);
                }

                SPP_MSG_ERROR_LOG("GetData_1: taskOptRet[%d] taskKey[%s]", pJceRsp->GetResult(), pJceRsp->GetTaskKey().c_str());
                AppendBossKVData("recRetErr=" + CNum2Str::Num2Str(pJceRsp->GetResult()));

                gettimeofday(&tmEnd, NULL);
                MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_MOD_ID, iMCallIf, pJceRsp->GetResult());
                return pJceRsp->GetResult();
            }

            // 判空和错误
            stRsp = pJceRsp->GetRspJceData();
            if (stRsp.rec_modules.empty() || stRsp.status.rsp_code == 1104)
            {
                Attr_API(GET_RECOMMEND_REQ_EMPTY, 1);
                AppendBossKVData("recRet=empty");

                SPP_MSG_DEBUG_LOG("no data. return");

                gettimeofday(&tmEnd, NULL);
                MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_MOD_ID, iMCallIf, stRsp.status.rsp_code);
                return 0;
            }

            AppendBossKVData("recRet=" + CNum2Str::Num2Str(stRsp.rec_modules.size()));
            SPP_MSG_DEBUG_LOG("get recommend succ rsp.module.size(%d)", stRsp.rec_modules.size());
        }
    }

    gettimeofday(&tmEnd, NULL);
    MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_MOD_ID, iMCallIf, iRet);
    return 0;
}

int CMsgOptSquare::GetDokiInfo(vector<string>& vecDokiid, map<string, StStarInfo>& mpStarInfo)
{
    if (vecDokiid.empty())
    {
        SPP_MSG_DEBUG_LOG("vecDokiid is empty");
        return 0;
    }

    struct timeval  tmStart;
    struct timeval  tmEnd;
    gettimeofday(&tmStart, NULL);

    // 做个兜底
    if (vecDokiid.size() > 30)
    {
        SPP_MSG_ERROR_LOG("doki more than expected, vecDokiid.size(%d)", vecDokiid.size());
        vecDokiid.resize(30);
    }

    SPP_MSG_DEBUG_LOG("vecDokiid.size(%d)", vecDokiid.size());

    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源


    COperateUnionData *pGetUnionData = new COperateUnionData(UNION_GETSTAR);
    pGetUnionData->SetReqUnionId(2039);
    pGetUnionData->SetReqVideoIds(vecDokiid);
    pGetUnionData->SetReqFields("c_star_id,c_title,c_type,c_smallPic");
    oTaskExec.PushBack(pGetUnionData);

    //union，查询2039表信息
    iRet = oTaskExec.RunTaskList();
    if (0 != iRet || 0 != pGetUnionData->GetResult())
    {
        SPP_MSG_ERROR_LOG("GetStarId4Union: iExecRet[%d] TaskRet[%d] ", iRet, pGetUnionData->GetResult());

        gettimeofday(&tmEnd, NULL);
        //MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_STAR_NEWS_ID, MCALL_STAR_NEWS_GET_DOKI_INFO, iRet);
        return CErrCodeProj::ERR_DOKI_BASIC_INFO_2039;
    }

    //解析明星id
    map<string, string> mpStarIdToDokiid;
    vector<string> vecStarId;

    for (int k = 0; k < pGetUnionData->GetUnionRspData().results_size(); k++)
    {
        const tv::DataSet &oDataSet = pGetUnionData->GetUnionRspData().results(k);
        string strKeyId = oDataSet.id();
        StStarInfo stDokiInfo;

        for (int i = 0; i < oDataSet.fields_size(); i++)
        {
            const tv::KeyValue &kv = oDataSet.fields(i);
            if (kv.key() == "c_smallPic")
            {
                std::string pic = COperateUnionData::UnionValueToString(kv.value());
                //SPP_MSG_TRACE_LOG("dokiid pic|%s|%s", strKeyId.c_str(), pic.c_str());

                if (!pic.empty() && pic != "null")
                {
                    stDokiInfo.strStarHead = pic;
                }
            }
            else if (kv.key() == "c_title")
            {
                std::string name = COperateUnionData::UnionValueToString(kv.value());
                // SPP_MSG_TRACE_LOG("dokiid name|%s|%s", strKeyId.c_str(), name.c_str());

                if (!name.empty() && name != "null")
                {
                    stDokiInfo.strStarName = name;
                }
            }
            else if (kv.key() == "c_star_id")
            {
                std::string strNameId = COperateUnionData::UnionValueToString(kv.value());
                //SPP_MSG_TRACE_LOG("dokiid[%s], star id[%s]", strKeyId.c_str(), strNameId.c_str());

                if (!strNameId.empty() && strNameId != "null")
                {
                    stDokiInfo.strStarId = strNameId;
                }
            }
            else if (kv.key() == "c_type")
            {
                std::string value = COperateUnionData::UnionValueToString(kv.value());

                int type = 255;
                if (value.empty() || value == "null")
                {
                    // 明星默认填写 type = 0， 因为union未设置数值
                    type = 0;
                }
                else
                {
                    type = atoi(value.c_str());
                }

                // SPP_MSG_TRACE_LOG("dokiid[%s], type[%d]", strKeyId.c_str(), type);
                if (type >= 0)
                {
                    stDokiInfo.iDokiType = type;
                }
                else
                {
                    SPP_MSG_ERROR_LOG("dokiid[%s], type[%d]", strKeyId.c_str(), type);
                    continue;
                }
            }
        }

        // 数据过滤
        if (stDokiInfo.iDokiType != 0 && (stDokiInfo.strStarName.empty() || stDokiInfo.strStarHead.empty()))
        {
            SPP_MSG_ERROR_LOG("dokiid[%s], type[%d]", strKeyId.c_str(), stDokiInfo.iDokiType);
            continue;
        }

        // 过滤非DOKI数据
        if (stDokiInfo.iDokiType == 6 || stDokiInfo.iDokiType == 7 || stDokiInfo.iDokiType == 8)
        {
            continue;
        }

        if (stDokiInfo.iDokiType == 0 && !stDokiInfo.strStarId.empty())
        {
            vecStarId.push_back(stDokiInfo.strStarId);
            mpStarIdToDokiid[stDokiInfo.strStarId] = strKeyId;
            continue;
        }

        if (stDokiInfo.iDokiType != 0)
        {
            m_mpDokiidToDetail[strKeyId] = stDokiInfo;
            continue;
        }
    }

    if (vecStarId.empty())
    {
        SPP_MSG_DEBUG_LOG("vecStarId is empty");
        return 0;
    }

    oTaskExec.ClearAllTask();
    pGetUnionData = NULL;
    pGetUnionData = new COperateUnionData(UNION_GETSTAR);
    pGetUnionData->SetReqUnionId(2006);
    pGetUnionData->SetReqVideoIds(vecStarId);
    pGetUnionData->SetReqFields("uin,name,pic200x200,fansid,is_visit");
    oTaskExec.PushBack(pGetUnionData);

    iRet = oTaskExec.RunTaskList();
    if (0 != iRet || 0 != pGetUnionData->GetResult())
    {
        SPP_MSG_ERROR_LOG("GetStar4Union: iExecRet[%d] TaskRet[%d] ", iRet, pGetUnionData->GetResult());

        gettimeofday(&tmEnd, NULL);
        //MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_STAR_NEWS_ID, MCALL_STAR_NEWS_GET_DOKI_INFO, iRet);
        return CErrCodeProj::ERR_DOKI_BASIC_INFO_2006;
    }

    //解析明星id
    set<string> setFilter;
    for (int k = 0; k < pGetUnionData->GetUnionRspData().results_size(); k++)
    {
        const tv::DataSet &oDataSet = pGetUnionData->GetUnionRspData().results(k);
        const string &strStarId = oDataSet.id();

        string     strVisit;
        string     strUin;

        StStarInfo stStarInfo;
        stStarInfo.strStarId = strStarId;

        for (int i = 0; i < oDataSet.fields_size(); i++)
        {
            const string    &strField = oDataSet.fields(i).key();
            const tv::Value &stValue = oDataSet.fields(i).value();

            if ("uin" == strField)
                strUin = COperateUnionData::UnionValueToString(stValue);
            else if ("name" == strField)
                stStarInfo.strStarName = COperateUnionData::UnionValueToString(stValue);
            else if ("pic200x200" == strField)
                stStarInfo.strStarHead = COperateUnionData::UnionValueToString(stValue);
            else if ("fansid" == strField)
                stStarInfo.strFansId = COperateUnionData::UnionValueToString(stValue);
            else if ("is_visit" == strField)
                strVisit = COperateUnionData::UnionValueToString(stValue);
        }

        if (strVisit != "是")
        {
            SPP_MSG_DEBUG_LOG("GetDokiInfo: DoNotShow starid[%s]name[%s] visit[%s]"
                , stStarInfo.strStarId.c_str(), stStarInfo.strStarName.c_str(), strVisit.c_str()); \
                continue;
        }

        if (stStarInfo.strStarName.empty())
        {
            SPP_MSG_DEBUG_LOG("GetDokiInfo: DoNotShow starid[%s]name[%s] visit[%s]"
                , stStarInfo.strStarId.c_str(), stStarInfo.strStarName.c_str(), strVisit.c_str()); \
                continue;
        }

        // 不信任2006表的fansid字段
        if (mpStarIdToDokiid.find(strStarId) != mpStarIdToDokiid.end())
        {
            m_mpDokiidToDetail[mpStarIdToDokiid[strStarId]] = stStarInfo;
        }

        //SPP_MSG_DEBUG_LOG("GetDokiInfo: Show starid[%s]name[%s]uin[%s]", stStarInfo.strStarId.c_str(), stStarInfo.strStarName.c_str(), strUin.c_str());
    }

    gettimeofday(&tmEnd, NULL);
    //MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_STAR_NEWS_ID, MCALL_STAR_NEWS_GET_DOKI_INFO, iRet);
}

int CMsgOptSquare::GetFeedInfo(const vector<string>& vecFeedid, map<string, jce::CirclePrimaryFeed>& mpFeedInfo)
{

}

int CMsgOptSquare::GetWelfareData(const WelfareClubInner::StWelfareNewDataReq& stReq, 
    WelfareClubInner::StWelfareNewDataRsp& stRsp)
{

}

int CMsgOptSquare::GetFocusPicture()
{

}

//   频道页回包数据
int CMsgOptSquare::Channel_PrepareData_1()
{
    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    struct timeval  tmStart;
    struct timeval  tmEnd;

    gettimeofday(&tmStart, NULL);

    // 获取焦点图 -- 读取REDIS单独处理
    for (size_t i = 0; i < m_vecMidMoreWork.size(); i++)
    {
        EModuleId& mid = m_vecMidMoreWork[i];
        if (mid == E_MODID_FOCUS_PIC)
        {
            // 获取焦点图数据
            GetCmsModuleRedisKey(m_vecModItemKey);
            GetCmsModuleConfDetail(m_vecModItemKey, m_vecFocusPic);

            SPP_MSG_DEBUG_LOG("m_vecModItemKey[%d] m_vecFocusPic[%d]", m_vecModItemKey.size(), m_vecFocusPic.size());
            break;
        }
    }

    // 以下是RPC调用
    // 首页获取PR配置
    if (IsSquareFirstPage())
    {
        AdvertisingPromotion::QueryAdPromotionReq stReq;

        stReq.stLocInfo.iPageLocId = AdvertisingPromotion::EnumPageDoki;//频道页
        stReq.stLocInfo.iModLocId = 0;  //这个填写什么？

        COperateJceData<AdvertisingPromotion::QueryAdPromotionRsp> *pGetJceData = new COperateJceData<AdvertisingPromotion::QueryAdPromotionRsp>(GET_PROMOTION, BUF_LEN_64K);
        pGetJceData->SetRspJceRetCode((int*)&(pGetJceData->GetRspJceData().stCommRes.iRet));
        pGetJceData->SetReqData(stReq, AdvertisingPromotion::EnumCmdQueryAdTasks, m_stVideoPacket);
        oTaskExec.PushBack(pGetJceData);
    }

    // 十佳帖、壁纸、表情包，并发处理
    for (size_t i = 0; i < m_vecMidMoreWork.size(); i++)
    {
        EModuleId& mid = m_vecMidMoreWork[i];

        // 样式A需读取第一个帖子封面图，样式B按插件下发即可
        if (mid == E_MODID_TOPTEN && IsSquareViewA()) 
        {
            // 十佳帖数据
            CStarAdminPro::SquareGoodFeedsRequest stReq;
            COperateJceData<CStarAdminPro::SquareGoodFeedsResp> *pGetJceData = new COperateJceData<CStarAdminPro::SquareGoodFeedsResp>(GET_FANS_ADMIN_CTT, BUF_LEN_64K);
            pGetJceData->SetTaskKey("fans_admin_config");
            pGetJceData->SetRspJceRetCode((int*)&(pGetJceData->GetRspJceData().errCode));
            pGetJceData->SetReqData(stReq, 0xfa17, m_stVideoPacket);
            oTaskExec.PushBack(pGetJceData);
            break;
        }
    }

    // 壁纸 -- 获取推荐doki
    for (size_t i = 0; i < m_vecMidMoreWork.size(); i++)
    {
        EModuleId& mid = m_vecMidMoreWork[i];
        if (mid == E_MODID_WALLPAPER)
        {
            UniRecInterface::RecReq stReq;
            static long seqnum = 10000;

            seqnum++;
            stReq.header.src_key = "110697#doki_rec";
            stReq.header.seq_num = m_stVideoPacket.getGuid() + "_" + CNum2Str::Num2Str(time(NULL)) + "_" + CNum2Str::Num2Str(seqnum);
            SPP_MSG_TRACE_LOG("seqnum[%s], src_key[%s]", stReq.header.seq_num.c_str(), stReq.header.src_key.c_str());

            stReq.rec_ctx.rec_scene = 2;
            stReq.rec_ctx.ctx_paras["module_num"] = "15";
            stReq.rec_ctx.ctx_paras["module_item_num"] = "1";

            COperateJceData<UniRecInterface::RecRsp> *pGetRecommend = new COperateJceData<UniRecInterface::RecRsp>(GET_RECOMMEND, BUF_LEN_16K);
            pGetRecommend->SetReqData(stReq, 0xf7aa, m_stVideoPacket);
            pGetRecommend->SetTaskKey("wallpaper");
            pGetRecommend->SetRspJceRetCode((int*)&(pGetRecommend->GetRspJceData().status.rsp_code));
            oTaskExec.PushBack(pGetRecommend);
            break;
        }
    }

    // 表情包 -- 获取推荐doki
    for (size_t i = 0; i < m_vecMidMoreWork.size(); i++)
    {
        EModuleId& mid = m_vecMidMoreWork[i];
        if (mid == E_MODID_EMOTICON)
        {
            UniRecInterface::RecReq stReq;
            static long seqnum = 10000;

            seqnum++;
            stReq.header.src_key = "110697#doki_rec";
            stReq.header.seq_num = m_stVideoPacket.getGuid() + "_" + CNum2Str::Num2Str(time(NULL)) + "_" + CNum2Str::Num2Str(seqnum);
            SPP_MSG_TRACE_LOG("seqnum[%s], src_key[%s]", stReq.header.seq_num.c_str(), stReq.header.src_key.c_str());

            stReq.rec_ctx.rec_scene = 2;
            stReq.rec_ctx.ctx_paras["module_num"] = "15";
            stReq.rec_ctx.ctx_paras["module_item_num"] = "1";

            COperateJceData<UniRecInterface::RecRsp> *pGetRecommend = new COperateJceData<UniRecInterface::RecRsp>(GET_RECOMMEND, BUF_LEN_16K);
            pGetRecommend->SetReqData(stReq, 0xf7aa, m_stVideoPacket);
            pGetRecommend->SetTaskKey("emoticon");
            pGetRecommend->SetRspJceRetCode((int*)&(pGetRecommend->GetRspJceData().status.rsp_code));
            oTaskExec.PushBack(pGetRecommend);
            break;
        }
    }

    // 读取PID信息
    if (!m_vecPid.empty())
    {
        for (size_t i = 0; i < m_vecPid.size(); i++)
        {
            const string &strPid = m_vecPid[i];

            COperateUnionData *pGetPid = new COperateUnionData(UNION_GETPID);
            pGetPid->SetTaskKey(strPid);
            pGetPid->SetReqUnionId(2010);
            pGetPid->SetReqAddVideoId(strPid);
            pGetPid->SetReqFields("title,playing_status,live_type,start_time,end_time,play_cnt,targetid,rose_live_id,stream_ratio");
            oTaskExec.PushBack(pGetPid);
        }
    }

    // 读取DOKI基本信息单独处理
    if (!m_vecDokiid.empty())
    {
        iRet = GetDokiInfo(m_vecDokiid, m_mpDokiidToDetail);
        if (m_mpDokiidToDetail.size() != m_vecDokiid.size())
        {
            SPP_MSG_ERROR_LOG("req doki[%d] get doki[%d]", m_vecDokiid.size(), m_mpDokiidToDetail.size());
        }
    }

    iRet = oTaskExec.RunTaskList();
    if (0 != iRet)
    {
        SPP_MSG_ERROR_LOG("RunTaskError iExecRet[%d] ", iRet);
        Attr_API(GET_PREPARE_DATA_1_FAIL, 1);

        AppendBossKVData("prepare1TaskErr=" + CNum2Str::Num2Str(iRet));
        return iRet;
    }

    // 处理结果
    for (size_t uiIdx = 0; uiIdx < oTaskExec.GetVecPtrTask().size(); ++uiIdx)
    {
        IMtTask *pTask = oTaskExec.GetVecPtrTask()[uiIdx];

        if (GET_PROMOTION == pTask->GetTaskType())
        {
            COperateJceData<AdvertisingPromotion::QueryAdPromotionRsp> *pJceTask = (COperateJceData<AdvertisingPromotion::QueryAdPromotionRsp>*)pTask;
            if (0 != pJceTask->GetResult())
            {
                AppendBossKVData("prTaskErr=" + CNum2Str::Num2Str(pJceTask->GetResult()));
                SPP_MSG_ERROR_LOG("QueryAdPromotionRsp: optRet[%d]", pJceTask->GetResult());
                continue;
            }

            string strErrMsg;
            iRet = CJceCode::DecodeJcePkg(m_stPRConfig, pJceTask->GetRspJceData().stAdTempletItem.strData, strErrMsg);
            if (0 != iRet)
            {
                AppendBossKVData("prDecodeErr=" + CNum2Str::Num2Str(iRet));

                SPP_MSG_ERROR_LOG("QueryAdPromotionRsp, decode err. iRet[%d]", iRet);
                continue;
            }

            // 判空和错误
            SPP_MSG_DEBUG_LOG("QueryAdPromotionRsp succ.");
        }
        // 壁纸和表情包icon推荐
        else if (GET_RECOMMEND == pTask->GetTaskType())
        {
            COperateJceData<UniRecInterface::RecRsp> *pJceRsp = (COperateJceData<UniRecInterface::RecRsp>*)pTask;
            if (0 != pJceRsp->GetResult())
            {
                //Attr_API(FEEDLINE_FANSCLUB_ERR, 1);
                SPP_MSG_ERROR_LOG("taskOptRet[%d] taskKey[%s]", pJceRsp->GetResult(), pJceRsp->GetTaskKey().c_str());
                AppendBossKVData("recTaskWallPaper|EmoticonErr=" + CNum2Str::Num2Str(pJceRsp->GetResult()));

                gettimeofday(&tmEnd, NULL);
                //MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_STAR_NEWS_ID, MCALL_STAR_NEWS_GET_RECOMMEND_DATA, iRet);
                continue;
            }

            // 判空和错误
            const UniRecInterface::RecRsp& stRsp = pJceRsp->GetRspJceData();
            if (stRsp.rec_modules.empty())
            {
                AppendBossKVData("recWallPaper|Emoticon=empty");
                SPP_MSG_ERROR_LOG("GetData_1, return empty");
                //continue;
            }

            int iTotalDoki = stRsp.rec_modules.size();
            const string& strTaskKey = pJceRsp->GetTaskKey();

            bool bWallPaper = (strTaskKey == "wallpaper");
            bool bEmoticon  = (strTaskKey == "emoticon");
 
            for (int iIdx = 0; iIdx < iTotalDoki; iIdx++)
            {
                const UniRecStruct::RecModule& stMoudle = stRsp.rec_modules[iIdx];
                if (stMoudle.module_id.empty())
                {
                    SPP_MSG_ERROR_LOG("GetDokiInfo, module_id is empty");
                    continue;
                }

                if (bWallPaper)
                    m_vecWallPaper.push_back(stMoudle.module_id);
                else if (bEmoticon)
                    m_vecEmoticon.push_back(stMoudle.module_id);

                SPP_MSG_DEBUG_LOG("%s: get a new doki, dokiid[%s]", 
                    strTaskKey.c_str(), stMoudle.module_id.c_str());
            }

            SPP_MSG_TRACE_LOG("GetData_1, vecRecommendData.size(%d)", stRsp.rec_modules.size());
        }
        else if (GET_FANS_ADMIN_CTT == pTask->GetTaskType())
        {
            // 语音和团长的信息
            COperateJceData<CStarAdminPro::SquareGoodFeedsResp> *pJceTask = (COperateJceData<CStarAdminPro::SquareGoodFeedsResp>*)pTask;
            if (0 != pJceTask->GetResult())
            {
                AppendBossKVData("toptenTaskErr=" + CNum2Str::Num2Str(pJceTask->GetResult()));

                SPP_MSG_ERROR_LOG("m_stToptenRsp: optRet[%d]", pJceTask->GetResult());
                continue;
            }

            m_stToptenRsp = pJceTask->GetRspJceData();

            // 获取十佳帖数据
            // 十佳饭帖
            for (int iIdx = 0; iIdx < m_stToptenRsp.feeds.size(); iIdx++)
            {
                CStarAdminPro::GoodFeedInfo& stFeedCfg = m_stToptenRsp.feeds[iIdx];
                //CStarAdminPro::GoodFeedInfo& stFeedCfg = m_jceTopTenFeedRsp.feeds[vecPos[iIdx]];

                if (stFeedCfg.feedid.empty())
                {
                    SPP_MSG_DEBUG_LOG("feedid empty, continue");
                    continue;
                }

                m_vecFeedid.push_back(stFeedCfg.feedid);
                SPP_MSG_DEBUG_LOG("topten feed[%s]", stFeedCfg.feedid.c_str());

                // A样式 - 只取一张封面图
                break;
            }

            // 判空和错误
            SPP_MSG_DEBUG_LOG("m_stToptenRsp.feeds.size(%d), nextPageContext[%s]",
                m_stToptenRsp.feeds.size(), m_stToptenRsp.nextPageContext.c_str());
        }
        // pid数据
        else if (UNION_GETPID == pTask->GetTaskType())
        {
            COperateUnionData *pGetPidInfo = (COperateUnionData*)pTask;
            if (0 != pGetPidInfo->GetResult())
            {
                AppendBossKVData("getPidTaskErr=" + CNum2Str::Num2Str(pGetPidInfo->GetResult()));

                SPP_MSG_ERROR_LOG("GetPidInfo4Union: TaskRet[%d] ", pGetPidInfo->GetResult());
                continue;
            }

            string strPid = pGetPidInfo->GetTaskKey();
            vector<string>& vecDokiid = m_mpPidToDokiid[strPid];

            if (strPid.empty() || vecDokiid.empty())
            {
                SPP_MSG_ERROR_LOG("strPid[%s] vecDokiid[%d]", strPid.c_str(), vecDokiid.size());
                continue;
            }

            for (int k = 0; k < pGetPidInfo->GetUnionRspData().results_size(); k++)
            {
                const tv::DataSet &oDataSet = pGetPidInfo->GetUnionRspData().results(k);

                StNewPidInfo stPidInfo;
                stPidInfo.strPid = strPid;

                for (int i = 0; i < oDataSet.fields_size(); i++)
                {
                    const tv::KeyValue &kv = oDataSet.fields(i);

                    if (kv.key() == "title" && kv.value().type() == tv::Value::VT_STRING)
                        stPidInfo.strTitle = kv.value().strval();
                    else if (kv.key() == "playing_status" && kv.value().type() == tv::Value::VT_INT)
                        stPidInfo.iPlayStatus = kv.value().intval();
                    else if (kv.key() == "live_type" && kv.value().type() == tv::Value::VT_STRING)
                        stPidInfo.strLiveType = kv.value().strval();
                    else if (kv.key() == "start_time" && kv.value().type() == tv::Value::VT_STRING)
                        stPidInfo.strBeginTime = kv.value().strval();
                    else if (kv.key() == "end_time" && kv.value().type() == tv::Value::VT_STRING)
                        stPidInfo.strEndTime = kv.value().strval();
                    else if (kv.key() == "play_cnt" && kv.value().type() == tv::Value::VT_INT)
                        stPidInfo.llPlayCountUnion = kv.value().intval();
                    else if (kv.key() == "targetid" && kv.value().type() == tv::Value::VT_INT)
                        stPidInfo.strTargetId = CNum2Str::Num2Str(kv.value().intval());
                    else if (kv.key() == "rose_live_id" && kv.value().type() == tv::Value::VT_INT)
                        stPidInfo.strRoseId = CNum2Str::Num2Str(kv.value().intval());
                    else if (kv.key() == "stream_ratio" && kv.value().type() == tv::Value::VT_STRING)
                        stPidInfo.strStreamRatio = kv.value().strval();
                }

                if ("fans" == stPidInfo.strLiveType) //fans直播
                {
                    //3 预设直播; 1 直播中; 4 直播结束点播未成功;5 直播结束点播成功;2 下架
                    if (3 == stPidInfo.iPlayStatus)
                        stPidInfo.iLiveStatus = STATUS_PID_BEF;
                    else if (1 == stPidInfo.iPlayStatus)
                        stPidInfo.iLiveStatus = STATUS_PID_ING;
                    else if (4 == stPidInfo.iPlayStatus || 5 == stPidInfo.iPlayStatus)
                        stPidInfo.iLiveStatus = STATUS_PID_END;
                }
                else //ving直播
                {
                    // 3-直播前，1-直播中，2-直播结束
                    if (3 == stPidInfo.iPlayStatus)
                        stPidInfo.iLiveStatus = STATUS_PID_BEF;
                    else if (1 == stPidInfo.iPlayStatus)
                        stPidInfo.iLiveStatus = STATUS_PID_ING;
                    else if (2 == stPidInfo.iPlayStatus)
                        stPidInfo.iLiveStatus = STATUS_PID_END;
                }

                if (STATUS_PID_BEF == stPidInfo.iLiveStatus
                    || STATUS_PID_ING == stPidInfo.iLiveStatus
                    || STATUS_PID_END == stPidInfo.iLiveStatus) //直播后，精选页保留24小时内的
                {
                    m_mpPidToPidInfo[strPid] = stPidInfo;

                    SPP_MSG_DEBUG_LOG("GetPid4Union: pid[%s]title[%s]playStatus[%d]liveType[%s]beginTime[%s]endTime[%s]playCnt[%lld]targetid[%s]roseid[%s]streamRation[%s] iLiveType[%d]"
                        , stPidInfo.strPid.c_str(), stPidInfo.strTitle.c_str(), stPidInfo.iPlayStatus, stPidInfo.strLiveType.c_str()
                        , stPidInfo.strBeginTime.c_str(), stPidInfo.strEndTime.c_str(), stPidInfo.llPlayCountUnion, stPidInfo.strTargetId.c_str(), stPidInfo.strRoseId.c_str()
                        , stPidInfo.strStreamRatio.c_str(), stPidInfo.iLiveStatus);
                }
                else //其他情况的（下架等） 直接删除
                {
                    SPP_MSG_DEBUG_LOG("GetPid4Union: filter pid[%s] iLiveStatus[%d]", stPidInfo.strPid.c_str(), stPidInfo.iLiveStatus);
                    continue;
                }
            }
        }
    }
}

//   频道页回包数据
int CMsgOptSquare::Channel_PrepareData_2()
{
    // 获取帖子详情
    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    struct timeval  tmStart;
    struct timeval  tmEnd;
    gettimeofday(&tmStart, NULL);

    // 设定一个上限
    // 避免逻辑错误导致feed过多，下游模块被压垮
    const int MAX_FEED_NUM = 50;
    string strScene = "fantuanexcl";

    //PRINT_VECTOR(m_vecFeedid);

    // 聚合feed id -- 
    if (m_vecFeedid.size() > MAX_FEED_NUM)
    {
        SPP_MSG_ERROR_LOG("feed num more than expected[%d], resize[%d]", m_vecFeedid.size(), MAX_FEED_NUM);
        m_vecFeedid.resize(MAX_FEED_NUM);
    }

    // SPP_MSG_DEBUG_LOG("get feed detail. feedid[%s]", m_vecFeedid[iFeedIdx].c_str());

    // 获取帖子数据
    for (int iFeedIdx = 0; iFeedIdx < m_vecFeedid.size(); iFeedIdx++)
    {
        string& strFeedid = m_vecFeedid[iFeedIdx];

        Attr_API(GET_FEED_DETAIL_REQ, 1);
        if (strFeedid.empty())
            continue;

        jce::FeedDetailRequest stReq;
        stReq.dataKey = string("id=") + m_vecFeedid[iFeedIdx] + "&scene=" + strScene;
        stReq.reportData = "reportKey=";

        if (IsSecondPageView())
        {
            stReq.reportData += "server_second_page_mod_" + CNum2Str::Num2Str(m_mpFeedidToModid[strFeedid]);
        }
        else {
            stReq.reportData += "server_page_channel_" + m_jceChannelReq.channelItemId;
        }

        COperateJceData<jce::FeedDetailResponse> *pGetJceData = new COperateJceData<jce::FeedDetailResponse>(GET_FEED_DEAIL, BUF_LEN_64K);
        pGetJceData->SetTaskKey(m_vecFeedid[iFeedIdx]);
        pGetJceData->SetRspJceRetCode((int*)&(pGetJceData->GetRspJceData().errCode));
        pGetJceData->SetReqData(stReq, 0xf085, m_stVideoPacket);
        oTaskExec.PushBack(pGetJceData);
    }

    // 壁纸
    for (size_t i = 0; i < m_vecWallPaper.size(); i++)
    {
        string& strDokiid = m_vecWallPaper[i];
        jce::WallPaperListRequest stReq;
        stReq.dataKey = "ftid=" + strDokiid + "&dst=square";

        Attr_API(GET_WALLPAPER_REQ, 1);

        COperateJceData<jce::WallPaperListResponse> *pGetJceData = new COperateJceData<jce::WallPaperListResponse>(
            GET_WELFARE_WALLPAPER, BUF_LEN_64K);
        pGetJceData->SetTaskKey("wallpaper");
        pGetJceData->SetKeyExt1(strDokiid);
        pGetJceData->SetRspJceRetCode((int*)&(pGetJceData->GetRspJceData().errCode));
        pGetJceData->SetReqData(stReq, 0xfa04, m_stVideoPacket);

        oTaskExec.PushBack(pGetJceData);
    }

    // 表情包
    for (size_t i = 0; i < m_vecEmoticon.size(); i++)
    {
        string& strDokiid = m_vecEmoticon[i];
        jce::DokiEmoticonGroupPageRequest stReq;

        stReq.dataKey = "ftid=" + strDokiid + "&dst=square";
        Attr_API(GET_EMOTICON_REQ, 1);

        COperateJceData<jce::DokiEmoticonGroupPageResponse> *pGetJceData = new COperateJceData<jce::DokiEmoticonGroupPageResponse>(
            GET_WELFARE_EMOTICON, BUF_LEN_64K);
        pGetJceData->SetTaskKey("emoticon");
        pGetJceData->SetKeyExt1(strDokiid);
        pGetJceData->SetRspJceRetCode((int*)&(pGetJceData->GetRspJceData().errCode));
        pGetJceData->SetReqData(stReq, 0xfc16, m_stVideoPacket);

        oTaskExec.PushBack(pGetJceData);
    }

    // 读取直播数据
    if (!m_vecPid.empty() && !m_mpPidToPidInfo.empty())
    {
        //直播前, 从livepoll获取关注数
        //直播中，ving直播取主持人最新一条feed，个人直播取当前的一条弹幕数据; 从livepoll取直播互动人数
        for (map<string, StNewPidInfo>::iterator itMap = m_mpPidToPidInfo.begin(); itMap != m_mpPidToPidInfo.end(); ++itMap)
        {
            const string &strPid = itMap->first;
            StNewPidInfo &stPidInfo = itMap->second;

            //直播前、直播中请求livepoll
            if (STATUS_PID_BEF == stPidInfo.iLiveStatus || STATUS_PID_ING == stPidInfo.iLiveStatus)
            {
                jce::LivePollRequest jceGetLivePollReq;
                jceGetLivePollReq.pollDataKey = "pid=" + stPidInfo.strPid;

                COperateJceData<jce::LivePollResponse> *pGetLivePoll = new COperateJceData<jce::LivePollResponse>(LIVE_POLL, BUF_LEN_8K);
                pGetLivePoll->SetTaskKey(strPid);
                pGetLivePoll->SetRspJceRetCode((int*)&(pGetLivePoll->GetRspJceData().errCode));
                pGetLivePoll->SetReqData(jceGetLivePollReq, CMD_LIVE_POLL, m_stVideoPacket);
                oTaskExec.PushBack(pGetLivePoll);
            }

            //直播中请求主持人、弹幕
            if (STATUS_PID_ING == stPidInfo.iLiveStatus || STATUS_PID_END == stPidInfo.iLiveStatus)
            {
                if ("fans" == stPidInfo.strLiveType)
                {
                    // 个人直播改为直接展示标题，不在取弹幕数据了
                }
                else
                {
                    jce::GetCommentListRequest jceGetCommentListReq;
                    jceGetCommentListReq.commentKey = GetCommentKey(stPidInfo);
                    jceGetCommentListReq.scene = 1;

                    COperateJceData<jce::GetCommentListResponse> *pGetLiveCompere = new COperateJceData<jce::GetCommentListResponse>(LIVE_COMPERE, BUF_LEN_8K);
                    pGetLiveCompere->SetTaskKey(strPid);
                    pGetLiveCompere->SetRspJceRetCode((int*)&(pGetLiveCompere->GetRspJceData().errCode));
                    pGetLiveCompere->SetReqData(jceGetCommentListReq, CMD_LIVE_COMPERE, m_stVideoPacket);
                    oTaskExec.PushBack(pGetLiveCompere);
                }
            }
        }
    }

    iRet = oTaskExec.RunTaskList();
    if (0 != iRet)
    {
        SPP_MSG_ERROR_LOG("GetFeedDetail: RunTaskError iExecRet[%d] ", iRet);
        AppendBossKVData("prepare2TaskErr=" + CNum2Str::Num2Str(iRet));

        Attr_API(GET_PREPARE_DATA_2_FAIL, 1);
        gettimeofday(&tmEnd, NULL);
        //MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_STAR_NEWS_ID, MCALL_STAR_NEWS_GET_FEED_DETAIL, iRet);
        return iRet;
    }

    for (size_t uiIdx = 0; uiIdx < oTaskExec.GetVecPtrTask().size(); ++uiIdx)
    {
        IMtTask *pTask = oTaskExec.GetVecPtrTask()[uiIdx];

        if (GET_WELFARE_WALLPAPER == pTask->GetTaskType())
        {
            // 团长列表信息
            COperateJceData<jce::WallPaperListResponse> *pJceTask = (COperateJceData<jce::WallPaperListResponse>*)pTask;
            if (0 != pJceTask->GetResult())
            {
                Attr_API(GET_WALLPAPER_REQ_FAIL, 1);
                AppendBossKVData("wallPaperTaskErr=" + CNum2Str::Num2Str(pJceTask->GetResult()));

                SPP_MSG_ERROR_LOG("WallPaperListResponse: optRet[%d]", pJceTask->GetResult());
                continue;
            }

            // 壁纸
            const string& strDokiid = pJceTask->GetKeyExt1();
            if (!(pJceTask->GetRspJceData().wallpaperList.empty()))
            {
                m_mpWallPaper[strDokiid] = pJceTask->GetRspJceData().wallpaperList[0];
            }
            else {
                AppendBossKVData("wallPaperRet=empty");
            }

            Attr_API(GET_WALLPAPER_REQ_SUCC, 1);
            SPP_MSG_DEBUG_LOG("get wall paper succ, dokiid[%s] count[%ld]",
                    strDokiid.c_str(), pJceTask->GetRspJceData().wallpaperList.size());
        }
        else if (GET_WELFARE_EMOTICON == pTask->GetTaskType())
        {
            // 团长列表信息
            COperateJceData<jce::DokiEmoticonGroupPageResponse> *pJceTask = (COperateJceData<jce::DokiEmoticonGroupPageResponse>*)pTask;
            if (0 != pJceTask->GetResult())
            {
                Attr_API(GET_EMOTICON_FAIL, 1);
                AppendBossKVData("emoticonTaskErr=" + CNum2Str::Num2Str(pJceTask->GetResult()));

                SPP_MSG_ERROR_LOG("DokiEmoticonGroupPageResponse: optRet[%d]", pJceTask->GetResult());
                continue;
            }

            // 表情包
            const string& strDokiid = pJceTask->GetKeyExt1();
            if (!(pJceTask->GetRspJceData().uiData.empty()))
            {
                for (size_t i = 0; i < pJceTask->GetRspJceData().uiData.size(); i++)
                {
                    const jce::TempletLine& stLine = pJceTask->GetRspJceData().uiData[i];
                    if (stLine.item.itemType == jce::EnumONADokiEmoticonItem)
                    {
                        try
                        {
                            string strBinData(stLine.item.data.begin(), stLine.item.data.end());

                            jce::ONADokiEmoticonItem stIcon;
                            iRet = CJceCode::DecodeJcePkg(stIcon, strBinData, strErrMsg);
                            
                            // 解码成功
                            if (iRet == 0)
                            {
                                m_mpEmoticon[strDokiid] = stIcon;
                                SPP_MSG_DEBUG_LOG("get emoticon succ. dokiid[%s]", strDokiid.c_str());
                                break;
                            }
                        }
                        catch (std::exception* e)
                        {
                            SPP_MSG_ERROR_LOG("decode ONADokiEmoticonItem err");
                        }
                    }
                }

                Attr_API(GET_EMOTICON_SUCC, 1);
                SPP_MSG_DEBUG_LOG("get emoticon, dokiid[%s] count[%ld]",
                    strDokiid.c_str(), pJceTask->GetRspJceData().uiData.size());
            }
            else
            {
                AppendBossKVData("emoticonRet=empty");
            }
        }
        else if (GET_FEED_DEAIL == pTask->GetTaskType())
        {
            COperateJceData<jce::FeedDetailResponse> *pGetJceData = (COperateJceData<jce::FeedDetailResponse>*)pTask;
            if (0 != pGetJceData->GetResult())
            {
                Attr_API(GET_FEED_DETAIL_FAIL, 1);
                AppendBossKVData("feedTaskErr=" + CNum2Str::Num2Str(pGetJceData->GetResult()));

                SPP_MSG_ERROR_LOG("FeedDetailResponse: optRet[%d], feedId[%s]", pGetJceData->GetResult(), pGetJceData->GetTaskKey().c_str());
                continue;
            }

            // 审核态过滤
            const jce::FeedDetailResponse &stRsp = pGetJceData->GetRspJceData();
            if (stRsp.feedInfo.status != 0)
            {
                Attr_API(GET_FEED_STAT_ERR, 1);
                SPP_MSG_DEBUG_LOG("FeedDetailResponse, wrong status: feedId[%s], status[%d]",
                    pGetJceData->GetTaskKey().c_str(), stRsp.feedInfo.status);

                AppendBossKVData("feedStatErr=" + pGetJceData->GetTaskKey());
                continue;
            }

            // 保存数据
            m_mpFeedidToDetail[pGetJceData->GetTaskKey()] = stRsp.feedInfo;

            Attr_API(GET_FEED_DETAIL_SUCC, 1);
            SPP_MSG_DEBUG_LOG("FeedDetailResponse succ: feedId[%s] title[%s]",
                pGetJceData->GetTaskKey().c_str(),
                stRsp.feedInfo.feedTitle.c_str());
        }
        else if (LIVE_POLL == pTask->GetTaskType())
        {
            COperateJceData<jce::LivePollResponse> *pGetLivePoll = (COperateJceData<jce::LivePollResponse>*)pTask;
            if (0 != pGetLivePoll->GetResult())
            {
                AppendBossKVData("livePollTaskErr=" + CNum2Str::Num2Str(pGetLivePoll->GetResult()));

                SPP_MSG_ERROR_LOG("GetLivePoll: optRet[%d]", pGetLivePoll->GetResult());
                continue;
            }
            const jce::LivePollResponse &stRsp = pGetLivePoll->GetRspJceData();

            map<string, StNewPidInfo>::iterator itMap = m_mpPidToPidInfo.find(pGetLivePoll->GetTaskKey());
            if (itMap == m_mpPidToPidInfo.end())
            {
                //Attr_API(NEWCARD_MAPFINDSTAR_ERR, 1);
                SPP_MSG_ERROR_LOG("GetLivePoll: cannot find pid[%s] info", pGetLivePoll->GetTaskKey().c_str());
                continue;
            }

            StNewPidInfo &stPidInfo = itMap->second;

            stPidInfo.llPlayCount = stRsp.playCount;
            stPidInfo.llAttentCount = stRsp.attentNumber;

            SPP_MSG_ERROR_LOG("GetLivePoll: Succ, pid[%s] joinNum[%lld] attentNum[%lld]"
                , stPidInfo.strPid.c_str(), stPidInfo.llPlayCount, stPidInfo.llAttentCount);
        }
        else if (LIVE_COMPERE == pTask->GetTaskType())
        {
            COperateJceData<jce::GetCommentListResponse> *pGetLiveCompere = (COperateJceData<jce::GetCommentListResponse>*)pTask;
            if (0 != pGetLiveCompere->GetResult())
            {
                AppendBossKVData("liveCompereTaskErr=" + CNum2Str::Num2Str(pGetLiveCompere->GetResult()));

                SPP_MSG_ERROR_LOG("GetLiveCompere: optRedisRet[%d]", pGetLiveCompere->GetResult());
                continue;
            }
            const jce::GetCommentListResponse &stRsp = pGetLiveCompere->GetRspJceData();

            map<string, StNewPidInfo>::iterator itMap = m_mpPidToPidInfo.find(pGetLiveCompere->GetTaskKey());
            if (itMap == m_mpPidToPidInfo.end())
            {
                //Attr_API(NEWCARD_MAPFINDSTAR_ERR, 1);
                SPP_MSG_ERROR_LOG("GetLiveCompere: cannot find starid[%s]", pGetLiveCompere->GetTaskKey().c_str());
                continue;
            }

            StNewPidInfo &stPidInfo = itMap->second;

            //遍历数据找到符合要求的一条主帖数据
            bool bHaveValidData = false;
            for (size_t uiIdx = 0; uiIdx < stRsp.commentList.size(); ++uiIdx)
            {
                const jce::CommentItem &stParentItem = stRsp.commentList[uiIdx];
                SPP_MSG_DEBUG_LOG("GetLiveCompere: commentId[%s]parentId[%s]richType[%d]"
                    , stParentItem.commentId.c_str(), stParentItem.parentId.c_str(), stParentItem.richType);

                if (stParentItem.commentId.empty()) //无帖子信息
                {
                    SPP_MSG_DEBUG_LOG("GetLiveCompere: starid[%s] commentidIsEmpty", pGetLiveCompere->GetTaskKey().c_str());
                    continue;
                }

                if (!stParentItem.parentId.empty() && "0" != stParentItem.parentId) //不是主帖
                {
                    SPP_MSG_DEBUG_LOG("GetLiveCompere: cannot find starid[%s] isMainFeed", pGetLiveCompere->GetTaskKey().c_str());
                    continue;
                }

                // 不是需要的数据
                if (RICHTYPE_NORMAL != stParentItem.richType && RICHTYPE_PIC != stParentItem.richType
                    && RICHTYPE_AUDIO != stParentItem.richType && RICHTYPE_VIDEO != stParentItem.richType)
                {
                    SPP_MSG_DEBUG_LOG("GetLiveCompere: pid[%s] type[%d] NoNeed", pGetLiveCompere->GetTaskKey().c_str(), stParentItem.richType);
                    continue;
                }

                // 非明星发表数据
                if (stParentItem.userInfo.userType != 1)
                {
                    SPP_MSG_DEBUG_LOG("GetLiveCompere: pid[%s] userType[%d], not star. filtered. ", 
                        pGetLiveCompere->GetTaskKey().c_str(),  stParentItem.userInfo.userType);
                    continue;
                }

                // 图片和视频列表
                for (size_t ui = 0; ui < stParentItem.imageList.size(); ++ui)
                {
                    const jce::VideoImage &stVideoImage = stParentItem.imageList[ui];

                    if (!stVideoImage.imageUrl.empty())
                    {
                        jce::CircleMsgImageUrl stPic;
                        stPic.url = "";
                        stPic.thumbUrl = stVideoImage.imageUrl;

                        bHaveValidData = true;
                        stPidInfo.stContentInfo.photos.push_back(stPic);
                    }

                    // 视频信息
                    if (!stVideoImage.video.vid.empty()){
                        bHaveValidData = true;
                        stPidInfo.stContentInfo.videos.push_back(stVideoImage.video);
                    }
                }

                // 语音信息
                if (!stParentItem.voiceData.voiceId.empty())
                {
                    bHaveValidData = true;
                    stPidInfo.stContentInfo.voices.push_back(stParentItem.voiceData);
                }

                // 文本信息
                if (!stParentItem.content.empty())
                {
                    bHaveValidData = true;
                }

                stPidInfo.llFeedTime = stParentItem.time;

                // 找到合法数据
                if (bHaveValidData)
                {
                    SPP_MSG_DEBUG_LOG("GetLiveCompere: pid[%s] comment[%s] name[%s]",
                        pGetLiveCompere->GetTaskKey().c_str(), stParentItem.content.c_str(),
                        stParentItem.userInfo.actorName.c_str());

                    stPidInfo.stContentInfo.text = stParentItem.userInfo.actorName + ":" +  stParentItem.content;
                    break;
                }
            }

            SPP_MSG_DEBUG_LOG("GetLiveCompere: Succ, pid[%s] content[%s]"
                , stPidInfo.strPid.c_str(), stPidInfo.stContentInfo.text.c_str());
        }
    }

    // 判空和错误
    SPP_MSG_DEBUG_LOG("m_mpFeedidToDetail.size(%d)", m_mpFeedidToDetail.size());

    gettimeofday(&tmEnd, NULL);
    //MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_STAR_NEWS_ID, MCALL_STAR_NEWS_GET_FEED_DETAIL, iRet);
    return iRet;
}

//   频道页回包数据
int CMsgOptSquare::Channel_PrepareData_3()
{
    // 获取帖子详情
    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    struct timeval  tmStart;
    struct timeval  tmEnd;
    gettimeofday(&tmStart, NULL);

    map< string, map<string, string> > mapUnionRsp_vid_2001;
    map< string, map<string, string> > mapUnionRsp_vid_1006;

    // 读取视频贴的cid/vid数据
    map<string, jce::CirclePrimaryFeed>::iterator mpFeedItor;
    mpFeedItor = m_mpFeedidToDetail.begin();

    while (mpFeedItor != m_mpFeedidToDetail.end())
    {
        if (IsVideoFeed(mpFeedItor->second))
        {
            StVideoInfo video;
            GetVideoInfo(mpFeedItor->second, video);

            if (video.IsCid() && !video.GetId().empty())
            {
                m_vecCid.push_back(video.GetId());
                SPP_MSG_DEBUG_LOG("feed[%s], cid[%s]", mpFeedItor->first.c_str(), video.GetId().c_str());
            }
            else if (video.IsVid() && !video.GetId().empty())
            {
                m_vecVid.push_back(video.GetId());
                SPP_MSG_DEBUG_LOG("feed[%s], vid[%s]", mpFeedItor->first.c_str(), video.GetId().c_str());
            }
        }

        mpFeedItor++;
    }

    // 读取宽高比数据
    if (!m_vecVid.empty())
    {
        COperateUnionData *pGetUnion = new COperateUnionData(UNION_GETVID_2001);
        pGetUnion->SetReqUnionId(2001);
        pGetUnion->SetReqVideoIds(m_vecVid);
        pGetUnion->SetReqFields("title,duration,aspect"); //标题，时长（秒），视频宽高比（小于1表示竖屏）
        oTaskExec.PushBack(pGetUnion);

        pGetUnion = new COperateUnionData(UNION_GETVID_1006);
        pGetUnion->SetReqUnionId(1006);
        pGetUnion->SetReqVideoIds(m_vecVid);
        pGetUnion->SetReqFields("video_title,video_duration,aspect"); //标题，时长（秒），视频宽高比（小于1表示竖屏）
        oTaskExec.PushBack(pGetUnion);
    }

    iRet = oTaskExec.RunTaskList();
    if (0 != iRet)
    {
        SPP_MSG_ERROR_LOG("RunTaskError iExecRet[%d] ", iRet);
        AppendBossKVData("prepare3TaskErr=" + CNum2Str::Num2Str(iRet));

        //Attr_API(GET_PREPARE_DATA_2_FAIL, 1);
        gettimeofday(&tmEnd, NULL);
        //MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_STAR_NEWS_ID, MCALL_STAR_NEWS_GET_FEED_DETAIL, iRet);
        return iRet;
    }

    for (size_t uiIdx = 0; uiIdx < oTaskExec.GetVecPtrTask().size(); ++uiIdx)
    {
        IMtTask *pTask = oTaskExec.GetVecPtrTask()[uiIdx];

        // 同样的操作。不同task方便上报
        if (UNION_GETVID_1006 == pTask->GetTaskType() || UNION_GETVID_2001 == pTask->GetTaskType())
        {
            COperateUnionData *pUnion = (COperateUnionData *)pTask;
            if (0 != pUnion->GetResult())
            {
                //Attr_API(GET_WALLPAPER_REQ_FAIL, 1);
                AppendBossKVData("getVidUnionTaskErr=" + CNum2Str::Num2Str(pUnion->GetResult()));

                SPP_MSG_ERROR_LOG("getVidUnionTaskErr: optRet[%d]", pUnion->GetResult());
                continue;
            }

            if (UNION_GETVID_2001 == pTask->GetTaskType())
                pUnion->GetMapRspData(mapUnionRsp_vid_2001);
            else
                pUnion->GetMapRspData(mapUnionRsp_vid_1006);

            //Attr_API(GET_WALLPAPER_REQ_SUCC, 1);
        }
    }

    // 判空和错误
    SPP_MSG_DEBUG_LOG("mapUnionRsp_vid_2001.size(%d), mapUnionRsp_vid_1006.size(%d)", mapUnionRsp_vid_2001.size(), mapUnionRsp_vid_1006.size());

    // 处理数据结果
    for (size_t i = 0; i < m_vecVid.size(); i++)
    {
        string& strVid = m_vecVid[i];
        StVideoInfo& video = m_mpVidToDetail[strVid];

        // 优先使用精品库数据
        if (mapUnionRsp_vid_2001.find(strVid) != mapUnionRsp_vid_2001.end())
        {
            map<string, string>& mpKeyVal = mapUnionRsp_vid_2001[strVid]; 

            string& duration = mpKeyVal["duration"];
            string& aspect = mpKeyVal["aspect"];

            if (!duration.empty()) video.lDuration = atol(duration.c_str());
            if (!aspect.empty()) video.fWHRatio = atof(aspect.c_str());

            video.SetVid(strVid);
            //SPP_MSG_DEBUG_LOG("get 2001_vid(%s), duration(%d), aspect(%f)", strVid.c_str(), video.lDuration, video.fWHRatio);
        }
        
        // 如果2001时长或宽高比 未取到，继续查找1006获取数据
        if ((video.lDuration == 0 || video.fWHRatio <= 0.0001)
            &&
            mapUnionRsp_vid_1006.find(strVid) != mapUnionRsp_vid_1006.end())
        {
            map<string, string>& mpKeyVal = mapUnionRsp_vid_1006[strVid];

            string& duration = mpKeyVal["video_duration"];
            string& aspect = mpKeyVal["aspect"];

            if (video.lDuration == 0 && !duration.empty()) video.lDuration = atol(duration.c_str());
            if (video.fWHRatio <= 0.0001 && !aspect.empty()) video.fWHRatio = atof(aspect.c_str());

            video.SetVid(strVid);
            //SPP_MSG_DEBUG_LOG("get 1006_vid(%s), duration(%d), aspect(%f)", strVid.c_str(), video.lDuration, video.fWHRatio);
        }
        else {
            //SPP_MSG_DEBUG_LOG("vid(%s) info not found.", strVid.c_str());
        }
    }


    gettimeofday(&tmEnd, NULL);
    //MCALL_REPORT_FUN(tmStart, tmEnd, MCALL_STAR_NEWS_ID, MCALL_STAR_NEWS_GET_FEED_DETAIL, iRet);
    return iRet;
}


int CMsgOptSquare::ConstructChannelData()
{
    // A样式 -- 双瀑布流feeds形态; 焦点图、话题模块等仍旧保留，feeds模块从“热门话题”模块下开始 
    // B样式 -- 每日十佳饭帖、明星说、饭制视频（影视cut、混剪、娱评等）、今天娱乐圈发生了什么（文章类资讯）、爱豆饭拍、榜单播报、爱豆美图、、爱豆表情、自有IP模块、无限刷feeds流
    // TAPD: http://tapd.oa.com/doki/prong/stories/view/1020357852063515463

    // 设置回包
    m_jceChannelRsp.hasNextPage = (m_iErrInner == CErrCodeProj::ERR_REQ_RECOMMEND_EMPTY) ? false : true;
    m_jceChannelRsp.pageContext = "pagenum=" + CNum2Str::Num2Str(m_stDatakey.pagenum + 1);

    string strPubKey = "targetid=2259922167&chid=" + m_jceChannelReq.channelItemId;
    m_jceChannelRsp.channnelExtraInfo.extraInfo["postDataKey"] = strPubKey; //发表用的datakey

    // PR配置数据
    if (IsSquareFirstPage())
    {
        m_jceChannelRsp.promotionInfo = m_stPRConfig.promotionInfo;
    }
    
    // 创建onaview
    taf::JceOutputStream<taf::BufferWriter> stJceWriter;

    for (size_t i = 0; i < m_stRecRsp.rec_modules.size(); i++)
    {
        UniRecStruct::RecModule& stRecModule = m_stRecRsp.rec_modules[i];
        SPP_MSG_DEBUG_LOG("To add module(%s)", GetModuleName(stRecModule.module_type).c_str());

        switch (stRecModule.module_type)
        {
        case E_MODID_FOCUS_PIC:           // 焦点图
            {
                CreateOnaFocPic(m_vecFocusPic, m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_DOKI_RENQI_UPDATE:   // 榜单播报，插件变更，UI变更
            {
                AppendPluginLine(m_jceChannelReq.channelItemId, 100046, "", "renqi_change_up", m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_HOT_TOPIC:           // 热门话题   -- 无需变更，插件实现
            {
                AppendPluginLine(m_jceChannelReq.channelItemId, 100062, "", "hot_topic", m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_DAIYLY_REPORT:       // 日报      -- 无需变更，插件实现
            {
                AppendPluginLine(m_jceChannelReq.channelItemId, 100061, "", "daily_report", m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_RENQI_MODCHAIN:      // 榜单、福利社入口  -- 无需变更，插件实现
            {
                AppendPluginLine(m_jceChannelReq.channelItemId, 100060, "", "reqi_modchain", m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_FEED:
            {
                CreateOnaFeed(stRecModule, m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_STAR_TALK:
            {
                CreateOnaStarTalk(stRecModule, m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_TOPTEN:
            {
                CreateOnaTopten(stRecModule, m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_PGC_VIDEO:  //饭制视频
        case E_MODID_UGC_VIDEO:
            {
                CreateOnaVideo(stRecModule, m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_WALLPAPER: // 推荐在此给或额外拉取接口获取，待定
        case E_MODID_EMOTICON:  // 
            {
                CreateOnaWelfare(stRecModule, m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_IP_GROUP:     // 宫格聚合
            {
                CreateOnaIPGroup(stRecModule, m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_ENTERTAINMENT_NEWS:
            {
                CreateOnaEntNews(stRecModule, m_jceChannelRsp.data);
                continue;
            }

        case E_MODID_DOKI_UPDATE_USR_FOLLOW: // 无此模块 
        case E_MODID_DOKI_UPDATE_RECOMMEND:  // 无此模块
        case E_MODID_VIDEO_TOPIC:            // AB样式均无此模块
        case E_MODID_DOKI_PARTY:             // 无此模块
            continue;

        default:
            SPP_MSG_ERROR_LOG("invalid modid: %d", stRecModule.module_type);
            break;
        }
    }

    SPP_MSG_DEBUG_LOG("ret module(%d)", m_jceChannelRsp.data.size());
}

//   模块二级页 -- 单机模块跳转进入
int CMsgOptSquare::SecondPage_PrepareData_1()
{
    
}

int CMsgOptSquare::ConstructSecondPage()
{
    // A样式 -- 双瀑布流feeds形态; 焦点图、话题模块等仍旧保留，feeds模块从“热门话题”模块下开始 
    // B样式 -- 每日十佳饭帖、明星说、饭制视频（影视cut、混剪、娱评等）、今天娱乐圈发生了什么（文章类资讯）、爱豆饭拍、榜单播报、爱豆美图、、爱豆表情、自有IP模块、无限刷feeds流
    // TAPD: http://tapd.oa.com/doki/prong/stories/view/1020357852063515463

    // 设置回包
    SPP_MSG_DEBUG_LOG("m_iErrInner(%d), m_stRecRsp.rec_modules[%d], pagenum[%d], pageContext[%s]", 
        m_iErrInner, m_stRecRsp.rec_modules.size(), m_stDatakey.pagenum, m_jceSecondPageReq.pageContext.c_str());

    m_jceSecondPageRsp.hasNextPage = (m_iErrInner == CErrCodeProj::ERR_REQ_RECOMMEND_EMPTY) ? false : true;
    m_jceSecondPageRsp.pageContext = "pagenum=" + CNum2Str::Num2Str(m_stDatakey.pagenum + 1);

    if (!m_stRecRsp.rec_modules.empty())
    m_jceSecondPageRsp.title = GetCMSTitle(m_stRecRsp.rec_modules[0]);

    // 创建onaview
    taf::JceOutputStream<taf::BufferWriter> stJceWriter;

    // 明星说、饭拍、饭制、宫格、娱乐资讯
    for (size_t i = 0; i < m_stRecRsp.rec_modules.size(); i++)
    {
        UniRecStruct::RecModule& stRecModule = m_stRecRsp.rec_modules[i];
        SPP_MSG_DEBUG_LOG("To add module(%d)", stRecModule.module_type);

        switch (stRecModule.module_type)
        {
        case E_MODID_STAR_TALK:
            {
                CreateOnaStarTalk(stRecModule, m_jceSecondPageRsp.uiData);
                continue;
            }

        case E_MODID_PGC_VIDEO:  //饭制视频
        case E_MODID_UGC_VIDEO:
            {
                CreateOnaVideo(stRecModule, m_jceSecondPageRsp.uiData);
                continue;
            }

        case E_MODID_ENTERTAINMENT_NEWS:
            {
                CreateOnaEntNews(stRecModule, m_jceSecondPageRsp.uiData);
                continue;
            }

        case E_MODID_IP_GROUP:     // 宫格聚合
            {
                CreateOnaIPGroup(stRecModule, m_jceSecondPageRsp.uiData);
                continue;
            }

        default:
            SPP_MSG_ERROR_LOG("invalid modid: %d", stRecModule.module_type);
            break;
        }
    }

    SPP_MSG_DEBUG_LOG("ret module(%d)", m_jceSecondPageRsp.uiData.size());
}

int  CMsgOptSquare::CreateOnaFocPic( 
    const vector<StCmsItem> &vecFocusPic,
    vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    // 待处理： 读取直播节目信息
    if (vecFocusPic.empty())
    {
        SPP_MSG_DEBUG_LOG("no focus pics");
        return 0;
    }

    // 
    char formatBuff[512] = {0};

    // 单个模块至多展示20张海报
    int iMaxPosters = 20;
    jce::ONAGalleryPoster stONAPicContainer;
    stONAPicContainer.uiType = 1;

    // 设置数据 -- 横图模式
    /*string strParams = "&modtitle=" + stModule.ZT_leaf_head;
    jce::ONAAttentPosterList stOnaviewDokiParty;
    stOnaviewDokiParty.reportKey = GetModRepKey();
    stOnaviewDokiParty.leftPadding = 12;     //固定间距
    stOnaviewDokiParty.maxLine = 2;

    //3是横图，2是竖图
    int iPicType = atoi(stModule.ZT_leaf_pic_type.c_str());
    bool bHorizPic = (iPicType == 3);
    stOnaviewDokiParty.uiType = (iPicType == 3) ? 0 : 1;*/

    for (int iIdx = 0; iIdx < vecFocusPic.size(); iIdx++)
    {
        jce::Poster stPoster;
        const StCmsItem &stCmsItem = vecFocusPic[iIdx];

        stPoster.imageUrl = stCmsItem.pic_1080x404;
        stPoster.imageUiType = 0;
       
        if (stCmsItem.strActionUrl.empty())
        {
            continue;
        }

        stPoster.action.url = stCmsItem.strActionUrl;
        stPoster.action.reportKey = DOKI_CHANNEL_FOCPIC_REPORT_KEY;

        snprintf(formatBuff, sizeof(formatBuff)-1, "plat_bucketid=%d&ztid=100224&mod_idx=1&ZT_leaf_type=ft_square_focus_pic_click&ZT_leaf_more_type=4&video_idx=%lu",
            m_stVideoPacket.getQUAInfo().extentData.bucketInfo.bucketId, iIdx);
        stPoster.action.reportParams = string(formatBuff);

        snprintf(formatBuff, sizeof(formatBuff)-1, "plat_bucketid=%d&ztid=100224&mod_idx=1&ZT_leaf_type=ft_square_focus_pic_exposure&ZT_leaf_more_type=&video_idx=%lu",
            m_stVideoPacket.getQUAInfo().extentData.bucketInfo.bucketId, iIdx);

        stPoster.reportKey = DOKI_CHANNEL_FOCPIC_REPORT_KEY;
        stPoster.reportParams = string(formatBuff);

        stONAPicContainer.posterList.push_back(stPoster);
    }

    // 构造标题onaview
    jce::TempletLine stPicLine;
    stPicLine.lineId = "ftfocuspic";
    stPicLine.groupId = "ftfocuspic";
    stPicLine.increaseId = 0;

    stPicLine.item.itemType = jce::EnumONAGalleryPoster;
    stONAPicContainer.writeTo(stJceWriter);
    stPicLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stPicLine);
    
    SPP_MSG_DEBUG_LOG("create focus pic succ. items[%d]", vecFocusPic.size());
}

int  CMsgOptSquare::CreateOnaDailyNews(vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
}

int  CMsgOptSquare::CreateOnaRankChain(vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
}

int  CMsgOptSquare::CreateOnaHotTopic(vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
}

int  CMsgOptSquare::CreateOnaTopten( 
    UniRecStruct::RecModule& stRecModule,
    vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    if (IsSquareViewA())
    {
        if (m_stToptenRsp.feeds.empty())
        {
            SPP_MSG_ERROR_LOG("no topten feed");
            return 0;
        }

        // 读取十佳帖数据
        jce::ONADokiTopTenGroupCard stOnaContainer;

        // 主标题
        const string& strCMSTitle = GetCMSTitle(stRecModule);
        const string& strCMSSubTitle = GetCMSSubTitle(stRecModule);
        const string& strCMSPosterUrl = GetCMSPoster(stRecModule);  //封面图

        jce::CircleMsgImageUrl stImg;
        string strFeedid = m_stToptenRsp.feeds.empty() ? "" : m_stToptenRsp.feeds[0].feedid;

        map<string, jce::CirclePrimaryFeed>::const_iterator mpItor = m_mpFeedidToDetail.find(strFeedid);
        if (strFeedid.empty() || mpItor == m_mpFeedidToDetail.end() || mpItor->second.status != 0)
        {
            SPP_MSG_ERROR_LOG("Feedid[%s], feedInfoFound[%d], feed status[%d]", 
                strFeedid.c_str(), mpItor == m_mpFeedidToDetail.end(), mpItor->second.status);
            return 0;
        }

        jce::CirclePrimaryFeed& stFeedInfo = m_mpFeedidToDetail[strFeedid];
        GetToptenFeedImage(stFeedInfo, m_stToptenRsp.feeds[0], stImg);

        // 检查边界
        if (strCMSPosterUrl.empty() && stImg.url.empty())
        {
            SPP_MSG_ERROR_LOG("cms poster and feed poster all empty");
            return 0;
        }

        stOnaContainer.cardInfo.title = strCMSTitle.empty() ? "十佳饭帖" : strCMSTitle;
        stOnaContainer.cardInfo.subTitle = strCMSSubTitle.empty() ? "发掘doki团高质帖" : strCMSSubTitle;
        stOnaContainer.cardInfo.cardAction.url = "txvideo://v.qq.com/DokiTopTenFeedListActivity?dataKey=";
        stOnaContainer.cardInfo.cardAction.url += CNewUrlCode::UrlEncode(m_stToptenRsp.nextPageContext);

        // 优先展示CMS配置封面
        if (!strCMSPosterUrl.empty())
        {
            stImg.url = strCMSPosterUrl;
        }

        stOnaContainer.cardInfo.images.push_back(stImg);
        stOnaContainer.cardInfo.userInfo.dokiName = stFeedInfo.user.actorName;
        stOnaContainer.cardInfo.userInfo.dokiImgUrl = stFeedInfo.user.faceImageUrl;

        // 数据上报
        SetReportParamViewA(E_MODID_TOPTEN, stOnaContainer);

        // 打包二进制数据
        jce::TempletLine stDataLine;
        stDataLine.lineId = "topten";
        stDataLine.groupId = "topten";
        stDataLine.increaseId = 0;

        stDataLine.item.itemType = jce::EnumONADokiTopTenGroupCard;
        stOnaContainer.writeTo(stJceWriter);
        stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
        vecLineData.push_back(stDataLine);

        //SPP_MSG_DEBUG_LOG("create topten succ. ");
    }
    // 样式B UI无变更，直接下发插件即可
    else if (IsSquareViewB())
    {
        AppendPluginLine(m_jceChannelReq.channelItemId, 100066, "", "topten", vecLineData);
    }
}

int  CMsgOptSquare::CreateOnaRenqiChangePlugin(vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
}

int  CMsgOptSquare::CreateOnaStarTalk(UniRecStruct::RecModule& stRecModule,vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    if (stRecModule.rec_items.empty())
    {
        SPP_MSG_ERROR_LOG("empty rec_items");
        return 0;
    }

    // 样式A，只取dokiid即可
    if (IsSquareViewA())
    {
        // 获取所有明星
        StStarTalkFeed stTalkFeed;
        stTalkFeed.eType = EREC_TYPE_DOKI_LIST;

        for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
        {
            const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
            map<string, string>::const_iterator mpItor = stRecItem.item_paras.find("item_rec_reason");

            if (mpItor == stRecItem.item_paras.end())
            {
                SPP_MSG_ERROR_LOG("star talk. item_id[%s], item_rec_reason\\dokiid not found.", stRecItem.item_id.c_str());
                continue;
            }

            vector<string> tmpDokiid;
            CStrUnits::SplitToVec(mpItor->second, "&", tmpDokiid);

            if (!tmpDokiid.empty())
            {
                stTalkFeed.vecDokiid.insert(stTalkFeed.vecDokiid.end(), tmpDokiid.begin(), tmpDokiid.end());
                SPP_MSG_DEBUG_LOG("star talk. get dokiid[%s].", tmpDokiid[0].c_str());
            }
        }

        CreateOnaStarTalkAView(stRecModule, stTalkFeed, vecLineData);
    }
    // 样式B，取明星说数据
    else if (IsSquareViewB())
    {
        vector<jce::TempletLine> vecTmpData;

        for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
        {
            const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
            if (stRecItem.item_type != EREC_TYPE_FEED && stRecItem.item_type != EREC_TYPE_PID)
            {
                SPP_MSG_ERROR_LOG("star talk. invalid item type[%d]", stRecItem.item_type);
                continue;
            }

            if (stRecItem.item_id.empty())
            {
                SPP_MSG_ERROR_LOG("star talk. invalid feedid or pid");
                continue;
            }

            // 明星说
            StStarTalkFeed stStarTalkPidFeed;
            if (stRecItem.item_type == EREC_TYPE_FEED)
            {
                stStarTalkPidFeed.eType = EREC_TYPE_FEED;
                stStarTalkPidFeed.strFeedid = stRecItem.item_id;

                CreateOnaStarTalkFeed(stRecModule, stStarTalkPidFeed, vecTmpData);
                SPP_MSG_DEBUG_LOG("star talk. feed id[%s]", stRecItem.item_id.c_str());
            }
            else {
                stStarTalkPidFeed.eType = EREC_TYPE_PID;
                stStarTalkPidFeed.strPid = stRecItem.item_id;

                CreateOnaStarTalkPid(stRecModule, stStarTalkPidFeed, vecTmpData);
                SPP_MSG_DEBUG_LOG("star talk. pid[%s]", stRecItem.item_id.c_str());
            }
        }

        ResizeLines(E_MODID_STAR_TALK, vecTmpData);

        // 是否存在
        if (!vecTmpData.empty())
        {
            // 下发标题
            string strCMSTitle = GetCMSTitle(stRecModule);;
            if (strCMSTitle.empty()) strCMSTitle = "明星说";

            // 标题groupid必须不一样 
            AppendTitle(vecLineData,   strCMSTitle, GetModGroupId(E_MODID_STAR_TALK) + "Title");
            vecLineData.insert(vecLineData.end(), vecTmpData.begin(), vecTmpData.end());

            // 查看更多
            string strActUrl = DOKI_SECOND_PAGE_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));
            AppendMore(vecLineData,   "查看更多明星说", GetModGroupId(E_MODID_STAR_TALK), strActUrl, stRecModule);
        }
    }
    // 和样式B采用同样的onaview，逻辑略有差别
    else if (IsSecondPageView())
    {
        for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
        {
            const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
            if (stRecItem.item_type != EREC_TYPE_FEED && stRecItem.item_type != EREC_TYPE_PID)
            {
                SPP_MSG_ERROR_LOG("star talk. invalid item type[%d]", stRecItem.item_type);
                continue;
            }

            if (stRecItem.item_id.empty())
            {
                SPP_MSG_ERROR_LOG("star talk. invalid feedid or pid");
                continue;
            }

            // 明星说
            StStarTalkFeed stStarTalkPidFeed;
            if (stRecItem.item_type == EREC_TYPE_FEED)
            {
                stStarTalkPidFeed.eType = EREC_TYPE_FEED;
                stStarTalkPidFeed.strFeedid = stRecItem.item_id;

                CreateOnaStarTalkFeed(stRecModule, stStarTalkPidFeed, vecLineData);
                SPP_MSG_DEBUG_LOG("star talk. feed id[%s]", stRecItem.item_id.c_str());
            }
            else {
                stStarTalkPidFeed.eType = EREC_TYPE_PID;
                stStarTalkPidFeed.strPid = stRecItem.item_id;

                CreateOnaStarTalkPid(stRecModule, stStarTalkPidFeed, vecLineData);
                SPP_MSG_DEBUG_LOG("star talk. pid[%s]", stRecItem.item_id.c_str());
            }
        }
    }
}

//   明星说样式 - 明星驾到
int  CMsgOptSquare::CreateOnaStarTalkPid( 
    UniRecStruct::RecModule& stRecModule, const StStarTalkFeed& stPidFeed, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    // 
    const string& strPid = stPidFeed.strPid;
    const string& strFeedid = stPidFeed.strFeedid;

    if (strPid.empty() || stPidFeed.eType != EREC_TYPE_PID)
    {
        SPP_MSG_ERROR_LOG("invalid. empty pid");
        return 0;
    }

    // 直播明星驾到
    vector<string>& vecDokiid = m_mpPidToDokiid[strPid];
    map<string, StNewPidInfo>::iterator mpItor = m_mpPidToPidInfo.find(strPid);

    // 
    if (vecDokiid.empty() || mpItor == m_mpPidToPidInfo.end())
    {
        SPP_MSG_ERROR_LOG("vecDokiid[%d], pid[%s], mpItor == m_mpPidToPidInfo.end[%d]", 
            vecDokiid.size(), strPid.c_str(), mpItor == m_mpPidToPidInfo.end());
        return 0;
    }

    // 设计稿：http://tc-svn.tencent.com/video/video_videodzd_rep/iPhone_proj/document/ui_design/iPhone6.5.8/doki%E5%B9%BF%E5%9C%BAfeeds%E6%B5%81/preview/
    jce::ONADokiStarLiveCard stOnaContainer;
    StNewPidInfo&            stPidInfo = mpItor->second;

    stOnaContainer.liveStatus = stPidInfo.iLiveStatus;
    stOnaContainer.attentItem.attentKey = "attent_type=4&rid=" + stPidInfo.strPid;
    stOnaContainer.pid = stPidInfo.strPid;

        // 头像
    for (size_t i = 0; i < vecDokiid.size() && i < 2; i++)
    {
        const string& strDokiid = vecDokiid[i];
        if (m_mpDokiidToDetail.find(strDokiid) == m_mpDokiidToDetail.end())
        {
            SPP_MSG_ERROR_LOG("dokiid[%d] info not found", strDokiid.c_str());
            continue;
        }

        jce::DokiBaseLiteInfo stAppDokiInfo;
        StStarInfo&           stInnerDokiInfo = m_mpDokiidToDetail[strDokiid];

        stAppDokiInfo.dokiName = stInnerDokiInfo.strStarName;
        stAppDokiInfo.dokiImgUrl = stInnerDokiInfo.strStarHead;
        stAppDokiInfo.badgeInfo.iconUrl = DOKI_VIP_ICON;// 大V角标

        stOnaContainer.actors.push_back(stAppDokiInfo);
    }

    stOnaContainer.lottieUrl = (stOnaContainer.actors.size() == 1) ?  
        "http://i.gtimg.cn/qqlive/images/20181213/onedata.json.zip" :
        "http://i.gtimg.cn/qqlive/images/20181213/twodata.json.zip";

    //  设置与直播状态相关 数据
    if (stPidInfo.iLiveStatus == STATUS_PID_BEF)
    {
        //开始时间，精确到分钟，格式为2017-07-14 14:34 ，若跨年则显示年份，不跨年则不显示年份。
        int64_t llStartTimeStamp = CStrUnits::GetTimeStampFromStrTime(stPidInfo.strBeginTime, "%Y-%m-%d %H:%M:%S");
        stOnaContainer.topDescText = "开始时间: " + CVideoCommUI::GetFeedDateTime(llStartTimeStamp);

        stOnaContainer.attentItem.attentKey = "attent_type=4&rid=" + stPidInfo.strPid;
        stOnaContainer.bottomLeftDescText = CVideoCommUI::GetUserNum(stPidInfo.llAttentCount, "人预约");;
        stOnaContainer.bottomRightAction.text = "立即预约";
        stOnaContainer.content = stPidInfo.strTitle;

        string strActionUrl = CVideoCommUI::GetTVideoActionUrlToPidDetail(stPidInfo.strPid, stPidInfo.strLiveType, stPidInfo.strStreamRatio);;
        stOnaContainer.cardAction.url = strActionUrl;
    }
    else if (stPidInfo.iLiveStatus == STATUS_PID_ING)
    {
        stOnaContainer.topDescText = CVideoCommUI::GetUserNum(stPidInfo.llPlayCount, "人参与");
        if (stOnaContainer.topDescText.empty())
        {
            stOnaContainer.topDescText = "正在直播";
        }

        stOnaContainer.bottomLeftDescText = "明星聊天室";
        stOnaContainer.bottomRightAction.text = "立即参与";

        string strActionUrl = CVideoCommUI::GetTVideoActionUrlToPidDetail(stPidInfo.strPid, stPidInfo.strLiveType, stPidInfo.strStreamRatio);;
        stOnaContainer.bottomRightAction.action.url = strActionUrl;
        stOnaContainer.cardAction.url = strActionUrl;

        // 直播中，展示主持人最新一条feed信息
        // 若无则展示 标题信息
        if (stPidInfo.stContentInfo.text.empty())
            stOnaContainer.content = stPidInfo.strTitle;
        else
            stOnaContainer.content = stPidInfo.stContentInfo.text;

        // 优先展示视频信息
        string strVidPic; //视频封面图
        // 语音
        if (!stPidInfo.stContentInfo.videos.empty())
        {
            jce::CircleMsgImageUrl stImage;
            stImage.url = stPidInfo.stContentInfo.videos[0].imageUrl;
            stImage.maskType = 2;

            stOnaContainer.images.push_back(stImage);
        }
        // 图片
        else if (!stPidInfo.stContentInfo.photos.empty())
        {
            stOnaContainer.images = stPidInfo.stContentInfo.photos;
        }
        else if (!stPidInfo.stContentInfo.voices.empty())
        {
            stOnaContainer.voiceData = stPidInfo.stContentInfo.voices[0];
        }
        else {
            SPP_MSG_DEBUG_LOG("pid[%s] no video/photo/voices", stPidInfo.strPid.c_str());;
        }
    }
    else if (stPidInfo.iLiveStatus == STATUS_PID_END)
    {
        int64_t llStartTimeStamp = CStrUnits::GetTimeStampFromStrTime(stPidInfo.strBeginTime, "%Y-%m-%d %H:%M:%S");
        stOnaContainer.topDescText = CVideoCommUI::GetFeedDateTime(llStartTimeStamp);
        stOnaContainer.topDescText += " " + CVideoCommUI::GetUserNum(stPidInfo.llPlayCountUnion, "人参与");;

        string strActionUrl = CVideoCommUI::GetTVideoActionUrlToPidDetail(stPidInfo.strPid, stPidInfo.strLiveType, stPidInfo.strStreamRatio);;
        stOnaContainer.bottomRightAction.action.url = strActionUrl;
        stOnaContainer.cardAction.url = strActionUrl;

        stOnaContainer.bottomLeftDescText = "";
        stOnaContainer.bottomRightAction.text = "查看更多";

        stOnaContainer.content = stPidInfo.strTitle;
    }
    else {
        SPP_MSG_ERROR_LOG("invalid pid status[%d]", stPidInfo.iLiveStatus);
        return 0;
    }

    // 数据上报
    {
        string strExt = "content_id=" + stPidInfo.strPid + "&live_status=" + CNum2Str::Num2Str(stPidInfo.iLiveStatus);

        AppendRecParam(stRecModule, strExt);
        SetReportParamViewB_WithoutCardBase(E_MODID_STAR_TALK, stOnaContainer, strExt);
        
        stOnaContainer.bottomRightAction.action.reportEventId = "common_button_item_click";
        stOnaContainer.bottomRightAction.action.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;
        
        stOnaContainer.bottomRightAction.action.reportParams = "data_type=content&sub_mod_id=pic&mod_id=10010&strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
        stOnaContainer.bottomRightAction.action.reportParams += "&" + strExt;
    }

    // 打包二进制数据
    jce::TempletLine stDataLine;

    // 1、广场B样式首页：同一个模块groupId必须一致，APP根据groupID把
    //    首页数据带进二级页，并置顶展示
    // 2、明星发表的普通feed，复用明星说view展示
    if (IsSecondPageView())
    {
        stDataLine.lineId = GetModGroupId(E_MODID_STAR_TALK, strPid);
        stDataLine.groupId = GetModGroupId(E_MODID_STAR_TALK, strPid);
    }
    else {
        stDataLine.lineId = GetModGroupId(E_MODID_STAR_TALK);
        stDataLine.groupId = GetModGroupId(E_MODID_STAR_TALK);
    }

    stDataLine.increaseId = 0;
    stDataLine.item.itemType = jce::EnumONADokiStarLiveCard;

    stOnaContainer.writeTo(stJceWriter);
    stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stDataLine);

    SPP_MSG_DEBUG_LOG("star talk. add pid[%s] title[%s]", stPidInfo.strPid.c_str(), stPidInfo.strTitle.c_str());
    return 0;
}

//   明星说样式 - 明星说普通feed
int  CMsgOptSquare::CreateOnaStarTalkFeed( 
    UniRecStruct::RecModule& stRecModule, const StStarTalkFeed& stPidFeed, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    // 帖子信息
    const string& strPid = stPidFeed.strPid;
    const string& strFeedid = stPidFeed.strFeedid;

    if (strFeedid.empty() || stPidFeed.eType != EREC_TYPE_FEED)
    {
        SPP_MSG_ERROR_LOG("invalid. empty feedid");
        return 0;
    }

    // 普通帖子数据
    // 设计稿：http://tc-svn.tencent.com/video/video_videodzd_rep/iPhone_proj/document/ui_design/iPhone6.5.8/doki%E5%B9%BF%E5%9C%BAfeeds%E6%B5%81/preview/
    map<string, jce::CirclePrimaryFeed>::iterator mpItor = m_mpFeedidToDetail.find(strFeedid);
    if (mpItor == m_mpFeedidToDetail.end())
    {
        SPP_MSG_ERROR_LOG("feed info not found. mpItor == m_mpPidToPidInfo.end[%d]", mpItor == m_mpFeedidToDetail.end());
        return 0;
    }

    jce::ONADokiStarFeedCard stOnaContainer;
    jce::CirclePrimaryFeed&  stFeedInfo = mpItor->second;

    FillFeedCard(stFeedInfo, stOnaContainer.cardInfo);
    GetFeedVoice(stFeedInfo, stOnaContainer.voiceData);
    
    // 明星说来源
    stOnaContainer.source = stFeedInfo.contentSource;

    // 查看更多
    //stOnaContainer.cardInfo.cardAction.url = stFeedInfo.feedAction.url;
    stOnaContainer.moreAction.text = "查看更多";
    stOnaContainer.moreAction.action.url = stFeedInfo.feedAction.url;

    // 数据上报: 如普通feed复用明星说样式（譬如明星发表的普通feed）
    // 按照明星说样式上报
    string strExt = "feed_id=" + strFeedid;
    strExt += "&userType=" + CNum2Str::Num2Str(int(stFeedInfo.user.userType));
    AppendRecParam(stRecModule, strExt);

    // 打包二进制数据
    jce::TempletLine stDataLine;

    // 1、广场B样式首页：同一个模块groupId必须一致，APP根据groupID把
    //    首页数据带进二级页，并置顶展示
    // 2、明星发表的普通feed，复用明星说view展示
    if (stRecModule.module_type == E_MODID_FEED)
    {
        stDataLine.lineId = GetModGroupId(E_MODID_FEED, strFeedid);
        stDataLine.groupId = GetModGroupId(E_MODID_FEED, strFeedid);

        SetReportParamViewB(E_MODID_FEED, stOnaContainer, strExt);
    }
    else if (IsSecondPageView())
    {
        stDataLine.lineId = GetModGroupId(E_MODID_STAR_TALK, strFeedid);
        stDataLine.groupId = GetModGroupId(E_MODID_STAR_TALK, strFeedid);
        SetReportParamViewB(E_MODID_STAR_TALK, stOnaContainer, strExt);
    }
    else {
        stDataLine.lineId = GetModGroupId(E_MODID_STAR_TALK);
        stDataLine.groupId = GetModGroupId(E_MODID_STAR_TALK);

        SetReportParamViewB(E_MODID_STAR_TALK, stOnaContainer, strExt);
    }

    stDataLine.increaseId = 0;
    stDataLine.item.itemType = jce::EnumONADokiStarFeedCard;

    stOnaContainer.writeTo(stJceWriter);
    stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stDataLine);

    SPP_MSG_DEBUG_LOG("star talk. add feedid[%s] content[%s]", strFeedid.c_str(), stFeedInfo.content.c_str());
    return 0;
}

//   明星说样式 - DOKI列表
int  CMsgOptSquare::CreateOnaStarTalkAView(UniRecStruct::RecModule& stRecModule, 
    const StStarTalkFeed& stPidFeed, vector<jce::TempletLine>& vecLineData)
{
    // 明星为空
    const vector<string>& vecDokiid = stPidFeed.vecDokiid;
    if (stPidFeed.vecDokiid.empty())
    {
        SPP_MSG_ERROR_LOG("star talk. no dokiid found.");
        return 0;
    }

    // 处理所有明星
    jce::ONADokiStarGroupCard stOnaContainer;

    // 主标题
    const string& strCMSTitle = GetCMSTitle(stRecModule);
    const string& strCMSSubTitle = GetCMSSubTitle(stRecModule);
    const string& strCMSPosterUrl = GetCMSPoster(stRecModule);  //封面图
    set<string> setUniqueDoki;

    for (size_t i = 0; i < vecDokiid.size(); i++)
    {
        const string& strDokiid = vecDokiid[i];

        if (m_mpDokiidToDetail.find(strDokiid) != m_mpDokiidToDetail.end()
            && setUniqueDoki.find(strDokiid) == setUniqueDoki.end())
        {
            StStarInfo& stStarInfo = m_mpDokiidToDetail[strDokiid];

            jce::DokiBaseLiteInfo stAppDokiInfo;
            stAppDokiInfo.dokiName = stStarInfo.strStarName;
            stAppDokiInfo.dokiImgUrl = stStarInfo.strStarHead;
            stAppDokiInfo.badgeInfo.iconUrl = DOKI_VIP_ICON;// 大V角标

            stOnaContainer.dokiList.push_back(stAppDokiInfo);
            setUniqueDoki.insert(strDokiid);
        }
        else 
        {
            SPP_MSG_ERROR_LOG("star talk. dokiid[%s] info not found. or duplicate", strDokiid.c_str());
        }
    }

    // 明星为空
    if (stOnaContainer.dokiList.empty())
    {
        SPP_MSG_ERROR_LOG("star talk. dokiList empty.");
        return 0;
    }

    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;
    stOnaContainer.title = strCMSTitle.empty() ? "明星说" : strCMSTitle;
    stOnaContainer.subTitle = strCMSSubTitle.empty() ? "爱豆原创内容都在这里" : strCMSSubTitle;

    // 卡片整体跳转二级页
    stOnaContainer.cardAction.url =  DOKI_SECOND_PAGE_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));

    string strExt;
    AppendRecParam(stRecModule, strExt);
    SetReportParamViewA_WithoutCardBase(E_MODID_STAR_TALK, stOnaContainer, strExt);

    // 打包二进制数据
    jce::TempletLine stDataLine;
    stDataLine.lineId = "star talk";
    stDataLine.groupId = "star talk";
    stDataLine.increaseId = 0;

    stDataLine.item.itemType = jce::EnumONADokiStarGroupCard;
    stOnaContainer.writeTo(stJceWriter);
    stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stDataLine);

    SPP_MSG_DEBUG_LOG("star talk. doki list count[%d]", stOnaContainer.dokiList.size());
}


int  CMsgOptSquare::CreateOnaFeed(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    string strFeedid;
    for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
    {
        const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
        if (stRecItem.item_type != EREC_TYPE_FEED && stRecItem.item_type != EREC_TYPE_CMS_SETTING)
        {
            SPP_MSG_ERROR_LOG("invalid item type[%d]", stRecItem.item_type);
            continue;
        }

        if (stRecItem.item_id.empty())
        {
            SPP_MSG_ERROR_LOG("invalid feedid or pid");
            continue;
        }

        // 
        strFeedid = stRecItem.item_id;
        SPP_MSG_DEBUG_LOG("strFeedid[%s]", strFeedid.c_str());
        break;
    }

    map<string, jce::CirclePrimaryFeed>::iterator mpItor = m_mpFeedidToDetail.find(strFeedid);
    if (strFeedid.empty() || mpItor == m_mpFeedidToDetail.end())
    {
        SPP_MSG_ERROR_LOG("invalid feed. feedid[%s], mpItor == m_mpFeedidToDetail[%d]", strFeedid.c_str(), mpItor == m_mpFeedidToDetail.end());
        return 0;
    }

    jce::CirclePrimaryFeed&  stFeedInfo = mpItor->second;

    // 如果是明星发表的，按照明星说样式展示
    if (stFeedInfo.user.userType == 1)
    {
        StStarTalkFeed stTalkFeed;
        stTalkFeed.strFeedid = strFeedid;
        stTalkFeed.eType = EREC_TYPE_FEED;

        CreateOnaStarTalkFeed(stRecModule, stTalkFeed, vecLineData);
    }
    // 普通样式
    else
    {
        jce::ONADokiCommonFeedCard stOnaContainer;
        FillFeedCard(stFeedInfo, stOnaContainer.cardInfo);

        string strExt = "feed_id=" + strFeedid;
        if (IsVideoFeed(stFeedInfo)){
            strExt += "&jump_type=video";
        }
        else if (IsImageFeed(stFeedInfo)){
            strExt += "&jump_type=pic";
        }

        AppendRecParam(stRecModule, strExt);
        SetReportParamViewB(E_MODID_FEED, stOnaContainer, strExt);

        // 打包二进制数据
        jce::TempletLine stDataLine;
        stDataLine.lineId = GetModGroupId(E_MODID_FEED, strFeedid);
        stDataLine.groupId = GetModGroupId(E_MODID_FEED, strFeedid);
        stDataLine.increaseId = 0;

        stDataLine.item.itemType = jce::EnumONADokiCommonFeedCard;
        stOnaContainer.writeTo(stJceWriter);
        stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
        vecLineData.push_back(stDataLine);
    }
}

// 饭拍饭制
int  CMsgOptSquare::CreateOnaVideo( 
    UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    if (stRecModule.module_type != E_MODID_PGC_VIDEO 
        && stRecModule.module_type != E_MODID_UGC_VIDEO)
    {
        SPP_MSG_ERROR_LOG("invalid modid[%d]", stRecModule.module_type);
        return 0;
    }
    
    // 
    if (IsSquareViewA())
    {
        vector<string> vecFeedid;
        for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
        {
            const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
            if (stRecItem.item_type != EREC_TYPE_FEED && stRecItem.item_type != EREC_TYPE_CMS_SETTING)
            {
                SPP_MSG_ERROR_LOG("invalid item type[%d]", stRecItem.item_type);
                continue;
            }

            if (stRecItem.item_id.empty())
            {
                SPP_MSG_ERROR_LOG("invalid feedid or pid");
                continue;
            }

            // 
            vecFeedid.push_back(stRecItem.item_id);
            SPP_MSG_DEBUG_LOG("strFeedid[%s]", stRecItem.item_id.c_str());
        }

        CreateOnaVideoAView(stRecModule, vecFeedid, vecLineData);
    }
    else if (IsSquareViewB())
    {
        vector<jce::TempletLine> vecTmpData;

        CreateOnaVideoBView(stRecModule, vecTmpData);
        ResizeLines(stRecModule.module_type, vecTmpData);

        // 是否存在
        if (!vecTmpData.empty())
        {
            // 下发标题
            string strCMSTitle = GetCMSTitle(stRecModule);

            // 标题groupid必须不一样 
            AppendTitle(vecLineData,   strCMSTitle, GetModGroupId(stRecModule.module_type) + "Title");
            vecLineData.insert(vecLineData.end(), vecTmpData.begin(), vecTmpData.end());

            string strActUrl = DOKI_SECOND_PAGE_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));

            // 查看更多
            if (stRecModule.module_type == E_MODID_PGC_VIDEO)
            {
                if (strCMSTitle.empty()) strCMSTitle = "饭制视频";
                AppendMore(vecLineData,   "查看更多饭制视频", GetModGroupId(stRecModule.module_type), strActUrl, stRecModule);
            }
            else{
                if (strCMSTitle.empty()) strCMSTitle = "爱豆饭拍";
                AppendMore(vecLineData,   "查看更多爱豆饭拍", GetModGroupId(stRecModule.module_type), strActUrl, stRecModule);
            }
        }
    }
    // 二级页和B样式采用同样onaview，逻辑略有差异
    else if (IsSecondPageView())
    {
        // 扩展参数 -- 当前只用于广场B样式ugc饭拍视频模块; 广场页的饭拍沉浸式播放，复用了 饭拍模块二级页的协议（包括命令字）
        // extid=feedid 表示自广场页点击进入沉浸式播放，读取老样式feed流，并把feedid放在第一个feed下发；用于给app支持视频沉浸式播放和左右横滑下一个视频
        // extid=空     表示自广场页点击进入模块二级页，出卡片化的自拍视频feed流（658定义结构体）。空为默认值。
        if (stRecModule.module_type == E_MODID_UGC_VIDEO && !m_stDatakey.extid.empty())
        {
            // 直接在广场页横滑移动 -- 广场页帖子置顶
            CreateOnaUGCSecondPage(stRecModule, m_stDatakey.extid, vecLineData);
            CreateOnaVideoBView(stRecModule, vecLineData, false); //出老样式feed

            SPP_MSG_DEBUG_LOG("%s feed from square[%s]", GetModuleName(E_MODID_UGC_VIDEO).c_str(), m_stDatakey.extid.c_str());
        }
        else {
            // 默认出卡片样式feed
             CreateOnaVideoBView(stRecModule, vecLineData);
        }
    }
}

// 饭拍饭制 -- 帖子形态 -- B样式
int  CMsgOptSquare::CreateOnaVideoBView(UniRecStruct::RecModule& stRecModule, 
    vector<jce::TempletLine>& vecLineData, bool bNewFeedView)
{
    // 普通帖子数据
    // 设计稿：http://tc-svn.tencent.com/video/video_videodzd_rep/iPhone_proj/document/ui_design/iPhone6.5.8/doki%E5%B9%BF%E5%9C%BAfeeds%E6%B5%81/preview/

    for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
    {
        UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];

        // 既不是feed，也不是配置的视频数据
        if (!IsRecItemFeed(stRecItem) && !IsRecItemCidVideo(stRecItem) && !IsRecItemVidVideo(stRecItem))
        {
            SPP_MSG_ERROR_LOG("invalid item type[%d] id[%s]", stRecItem.item_type, stRecItem.item_id.c_str());
            continue;
        }

        if (IsRecItemFeed(stRecItem))
        {
            string& strFeedid = stRecItem.item_id;
            map<string, jce::CirclePrimaryFeed>::iterator mpItor = m_mpFeedidToDetail.find(strFeedid);

            if (strFeedid.empty() || mpItor == m_mpFeedidToDetail.end())
            {
                SPP_MSG_ERROR_LOG("invalid feed. feedid[%s], mpItor == m_mpFeedidToDetail[%d]", strFeedid.c_str(), mpItor == m_mpFeedidToDetail.end());
                continue;
            }

            jce::CirclePrimaryFeed&  stFeedInfo = mpItor->second;
            if (bNewFeedView)  // 卡片化feed
            {
                jce::ONADokiFansVideoCard stOnaContainer;
                FillFeedCard(stFeedInfo, stOnaContainer.cardInfo);

                // 饭拍ugc-0，饭制pgc-1
                stOnaContainer.uiType = (stRecModule.module_type == E_MODID_PGC_VIDEO) ? 1 : 0;

                // UGC饭拍视频点击 进入视频沉浸式播放，不跳转帖子详情
                if (stRecModule.module_type == E_MODID_UGC_VIDEO)
                {
                    // 扩展参数 -- 只用于广场B样式ugc饭拍视频模块
                    // extid=feedid 表示读取老样式feed流，并把feedid放在第一个feed下发；用于给app支持视频沉浸式播放和左右横滑下一个视频
                    // extid=空     表示点击饭拍视频模块进入模块二级页，出卡片化的自拍视频feed流（658定义结构体）。空为默认值。
                    string datakey = "modid=" + CNum2Str::Num2Str(stRecModule.module_type) + "&extid=" + strFeedid;
                    stOnaContainer.cardInfo.cardAction.url = "txvideo://v.qq.com/VerticalStreamListActivity?dataType=video_fans&dataKey=" + CNewUrlCode::UrlEncode(datakey);
                }

                // 帖子类型上报
                string strExt = "feed_id=" + strFeedid;
                if (IsVideoFeed(stFeedInfo)){
                    strExt += "&jump_type=video";
                }
                else if (IsImageFeed(stFeedInfo)){
                    strExt += "&jump_type=pic";
                }

                AppendRecParam(stRecItem, strExt);
                SetReportParamViewB(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

                string strGroupId;
                if (IsSecondPageView())
                {
                    strGroupId = GetModGroupId(stRecModule.module_type, strFeedid);
                }
                else
                {
                    // 广场页模块，同个模块需设置 一致的groupID，APP依此统一跳转
                    strGroupId = GetModGroupId(stRecModule.module_type);
                }

                // 打包二进制数据
                AppendTempleteLine(vecLineData, stOnaContainer, jce::EnumONADokiFansVideoCard, strGroupId, strGroupId);
                SPP_MSG_DEBUG_LOG("add feedid[%s] content[%s]", strFeedid.c_str(), stFeedInfo.content.c_str());
            }
            else 
            {
                jce::ONAPrimaryFeed stOnaContainer;
                stOnaContainer.feedInfo = stFeedInfo;
                string strGroupId = GetModGroupId(stRecModule.module_type, strFeedid);

                // 推荐上报
                string strExt;
                AppendRecParam(stRecItem, strExt);
                stOnaContainer.feedInfo.selfVideo.videoData.playReportInfo.autoPlayReportParams += strExt;
                stOnaContainer.feedInfo.bulletinBoardV2.videoData.playReportInfo.autoPlayReportParams += strExt;

                // 打包二进制数据
                AppendTempleteLine(vecLineData, stOnaContainer, jce::EnumONAPrimaryFeed, strGroupId, strGroupId);
                SPP_MSG_DEBUG_LOG("add feedid[%s] content[%s]", strFeedid.c_str(), stFeedInfo.content.c_str());
            }
        }
        // 二级页才出视频配置数据
        else if(IsSecondPageView()) 
        {
            string& strVid = stRecItem.item_id;

            jce::ONADokiVideoCoverCard stOnaContainer;
            FillVideoCard(stRecItem, stOnaContainer.cardInfo);

            string strExt = "sub_mod_id=" + stRecModule.module_id;
            strExt += "&modtitle=" + stOnaContainer.cardInfo.title;

            if (IsRecItemCidVideo(stRecItem))
                strExt += "&cid=" + strVid;
            else 
                strExt += "&vid=" + strVid;

            AppendRecParam(stRecItem, strExt);
            SetReportParamViewB(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

            // 极其不合理！ APP同学制定的上报 一会按照功能，一会按照视图，
            // 甚至同一个功能 既按照功能、也按照view类型上报，混乱不堪！找peterzkli
            stOnaContainer.reportEventId = "video_jce_poster_exposure";
            stOnaContainer.cardInfo.cardAction.reportEventId = "video_jce_action_click";

            // 打包二进制数据
            AppendTempleteLine(vecLineData, stOnaContainer, jce::EnumONADokiVideoCoverCard, GetModGroupId(E_MODID_FEED, strVid), GetModGroupId(E_MODID_FEED, strVid));
            SPP_MSG_DEBUG_LOG("add cms video[%s]", strVid.c_str());
        }
    }

    return 0;
}

//   饭拍视频 -- 二级页有两个形态
//   在广场页读取，依照普通帖子feed返回
//   在二级页读取，依照ONADokiFansVideoCard返回
int  CMsgOptSquare::CreateOnaUGCSecondPage(UniRecStruct::RecModule& stRecModule, const string& strFeedid, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter> stJceWriter;

    // 帖子信息
    if (strFeedid.empty())
    {
        SPP_MSG_ERROR_LOG("invalid. empty feedid");
        return 0;
    }

    // 普通帖子数据
    // 设计稿：http://tc-svn.tencent.com/video/video_videodzd_rep/iPhone_proj/document/ui_design/iPhone6.5.8/doki%E5%B9%BF%E5%9C%BAfeeds%E6%B5%81/preview/
    map<string, jce::CirclePrimaryFeed>::iterator mpItor = m_mpFeedidToDetail.find(strFeedid);
    if (mpItor == m_mpFeedidToDetail.end())
    {
        SPP_MSG_ERROR_LOG("feed info not found. mpItor == m_mpPidToPidInfo.end[%d]", mpItor == m_mpFeedidToDetail.end());
        return 0;
    }

    jce::CirclePrimaryFeed&  stFeedInfo = mpItor->second;

    // 饭拍二级页
    if (m_stDatakey.extid.empty())
    {
        jce::ONADokiFansVideoCard stOnaContainer;
        FillFeedCard(stFeedInfo, stOnaContainer.cardInfo);

        // 饭拍ugc-0，饭制pgc-1
        stOnaContainer.uiType = (stRecModule.module_type == E_MODID_PGC_VIDEO) ? 1 : 0;

        // 数据上报
        string strExt = "feed_id=" + strFeedid;
        AppendRecParam(stRecModule, strExt);
        SetReportParamViewB(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

        // 不再跳转帖子详情页
        // 扩展参数 -- 只用于广场B样式ugc饭拍视频模块
        // extid=feedid 表示读取老样式feed流，并把feedid放在第一个feed下发；用于给app支持视频沉浸式播放和左右横滑下一个视频
        // extid=空     表示点击饭拍视频模块进入模块二级页，出卡片化的自拍视频feed流（658定义结构体）。空为默认值。
        string datakey = "modid=" + CNum2Str::Num2Str(stRecModule.module_type) + "&extid=" + strFeedid;
        stOnaContainer.cardInfo.cardAction.url = "txvideo://v.qq.com/VerticalStreamListActivity?dataType=video_fans&dataKey=" + CNewUrlCode::UrlEncode(datakey);

        // 打包二进制数据
        jce::TempletLine stDataLine;

        // 二级页复用B样式onaview
        stDataLine.lineId = GetModGroupId(stRecModule.module_type, strFeedid);
        stDataLine.groupId = GetModGroupId(stRecModule.module_type, strFeedid);

        stDataLine.increaseId = 0;
        stDataLine.item.itemType = jce::EnumONADokiFansVideoCard;

        stOnaContainer.writeTo(stJceWriter);
        stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
        vecLineData.push_back(stDataLine);
    }
    // 广场页点击饭拍视频，左右滑动拉取视频流，此种情形采用普通帖子视图下发
    else{
        jce::ONAPrimaryFeed stOnaContainer;
        stOnaContainer.feedInfo = stFeedInfo;

        // 打包二进制数据
        jce::TempletLine stDataLine;

        // 二级页复用B样式onaview
        stDataLine.lineId = GetModGroupId(stRecModule.module_type, strFeedid);
        stDataLine.groupId = GetModGroupId(stRecModule.module_type, strFeedid);

        stDataLine.increaseId = 0;
        stDataLine.item.itemType = jce::EnumONAPrimaryFeed;

        stOnaContainer.writeTo(stJceWriter);
        stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
        vecLineData.push_back(stDataLine);
    }

    SPP_MSG_DEBUG_LOG("add feedid[%s] content[%s]", strFeedid.c_str(), stFeedInfo.content.c_str());
    return 0;
}


int  CMsgOptSquare::CreateOnaVideoAView( 
    UniRecStruct::RecModule& stRecModule, const vector<string>& vecFeedid, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    // 帖子信息
    if (vecFeedid.empty())
    {
        SPP_MSG_ERROR_LOG("invalid. empty vecFeedid");
        return 0;
    }

    jce::ONADokiGalleryGroupCard stOnaContainer;

    // 普通帖子数据
    // 设计稿：http://tc-svn.tencent.com/video/video_videodzd_rep/iPhone_proj/document/ui_design/iPhone6.5.8/doki%E5%B9%BF%E5%9C%BAfeeds%E6%B5%81/preview/
    for (size_t i = 0; i < vecFeedid.size(); i++)
    {
        const string& strFeedid = vecFeedid[i];
        map<string, jce::CirclePrimaryFeed>::iterator mpItor = m_mpFeedidToDetail.find(strFeedid);

        if (mpItor == m_mpFeedidToDetail.end())
        {
            SPP_MSG_ERROR_LOG("feed info not found. mpItor == m_mpPidToPidInfo.end[%d]", mpItor == m_mpFeedidToDetail.end());
            continue;
        }

        jce::CircleMsgImageUrl stImage;
        string strImgUrl;
        
        // 是否视频
        if (IsVideoFeed(mpItor->second, strImgUrl))
        {
            stImage.url = strImgUrl;
            stImage.maskType = 0;
            stOnaContainer.imageList.push_back(stImage);
        }
    }

    // 不足9张如何展示
    stOnaContainer.title = GetCMSTitle(stRecModule);
    stOnaContainer.subTitle= GetCMSSubTitle(stRecModule);

    // 饭制视频：正方图，饭拍：长方形图
    // 卡片整体跳转二级页
    stOnaContainer.uiType = (stRecModule.module_type == E_MODID_PGC_VIDEO) ? 1 : 0;  
    stOnaContainer.cardAction.url = DOKI_SECOND_PAGE_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));

    string strExt;
    AppendRecParam(stRecModule, strExt);
    SetReportParamViewA_WithoutCardBase(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

    // 打包二进制数据
    jce::TempletLine stDataLine;

    stDataLine.lineId = GetModGroupId(stRecModule.module_type);
    stDataLine.groupId = GetModGroupId(stRecModule.module_type);

    stDataLine.increaseId = 0;
    stDataLine.item.itemType = jce::EnumONADokiGalleryGroupCard;

    stOnaContainer.writeTo(stJceWriter);
    stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stDataLine);

    SPP_MSG_DEBUG_LOG("stOnaContainer.imageList[%d]", stOnaContainer.imageList.size());
    return 0;
}

int  CMsgOptSquare::CreateOnaWelfare(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    // 
    if (IsSquareViewA())
    {
        // 表情包、美图两个模块复用
        CreateOnaWallPaperAView(stRecModule, vecLineData);
    }
    else if (IsSquareViewB())
    {
        vector<jce::TempletLine> vecTmpData;
        if (stRecModule.module_type == E_MODID_WALLPAPER)
        {
            // 数据从成员变量取
            CreateOnaWallPaperBView(stRecModule, vecTmpData);
        }
        else if (stRecModule.module_type == E_MODID_EMOTICON)
        {
            CreateOnaEmoticonBView(stRecModule, vecTmpData);
        }
        
        // 是否存在
        if (!vecTmpData.empty())
        {
            // 下发标题
            string strCMSTitle = GetCMSTitle(stRecModule);

            // 标题groupid必须不一样 
            AppendTitle(vecLineData,   strCMSTitle, GetModGroupId(stRecModule.module_type) + "Title");
            vecLineData.insert(vecLineData.end(), vecTmpData.begin(), vecTmpData.end());

            string strActUrl = "txvideo://v.qq.com/DokiWelfareActivity?dataKey=pfrom%3D0ftid%3D0%26type%3D0&targetTabId=0";

            // 查看更多
            if (stRecModule.module_type == E_MODID_WALLPAPER)
            {
                if (strCMSTitle.empty()) 
                    strCMSTitle = "爱豆美图";

                AppendMore(vecLineData,   "查看更多爱豆美图", GetModGroupId(stRecModule.module_type), strActUrl, stRecModule);
            }
            else{
                if (strCMSTitle.empty()) 
                    strCMSTitle = "爱豆表情";

                AppendMore(vecLineData,   "查看更多爱豆表情", GetModGroupId(stRecModule.module_type), strActUrl, stRecModule);
            }

            SPP_MSG_DEBUG_LOG("%s vecTmpData[%d]", GetModuleName(stRecModule.module_type).c_str(), vecTmpData.size());
        }
    }
}

int  CMsgOptSquare::CreateOnaWallPaperAView( 
    UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;
    jce::ONADokiGalleryGroupCard stOnaContainer;

    if (stRecModule.module_type == E_MODID_WALLPAPER)
    {
        for (size_t i = 0; i < m_vecWallPaper.size(); i++)
        {
            string& strDokiid = m_vecWallPaper[i];
            map<string, jce::ONADokiWallPaperItem>::iterator mpItor = m_mpWallPaper.find(strDokiid);

            if (mpItor != m_mpWallPaper.end())
            {
                jce::CircleMsgImageUrl stImage;
                if (GetWallPaperImg(mpItor->second.wallpaper, stImage.url))
                {
                    stOnaContainer.imageList.push_back(stImage);
                }
                
                SPP_MSG_DEBUG_LOG("found wall paper. dokiid[%s]", strDokiid.c_str());
            }
        }
    }
    else if (stRecModule.module_type == E_MODID_EMOTICON)
    {
        for (size_t i = 0; i < m_vecEmoticon.size(); i++)
        {
            string& strDokiid = m_vecEmoticon[i];
             map<string, jce::ONADokiEmoticonItem>::iterator mpItor = m_mpEmoticon.find(strDokiid);

            if (mpItor != m_mpEmoticon.end())
            {
                jce::CircleMsgImageUrl stImage;
                if (GetEmoticonImg(mpItor->second, stImage.url))
                {
                    stOnaContainer.imageList.push_back(stImage);
                }

                SPP_MSG_DEBUG_LOG("found emoticon. dokiid[%s]", strDokiid.c_str());
            }
        }
    }

    if (stOnaContainer.imageList.empty())
    {
        SPP_MSG_ERROR_LOG("no image found. module: %s", GetModuleName(stRecModule.module_type).c_str());
        return 0;
    }

    // 不足9张如何展示
    stOnaContainer.title = GetCMSTitle(stRecModule);
    stOnaContainer.subTitle = GetCMSSubTitle(stRecModule);

    // 表情：正方图，壁纸：长方形图
    // 卡片整体跳转二级页
    if (stRecModule.module_type == E_MODID_EMOTICON)
    {
        stOnaContainer.uiType =  1;
        stOnaContainer.cardAction.url = DOKI_EMOTICON_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));

    }
    else {
        stOnaContainer.uiType = 0;
        stOnaContainer.cardAction.url = DOKI_WALLPAPER_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));
    }

    string strExt;
    AppendRecParam(stRecModule, strExt);
    SetReportParamViewA_WithoutCardBase(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

    // 打包二进制数据
    jce::TempletLine stDataLine;

    stDataLine.lineId = GetModGroupId(stRecModule.module_type);
    stDataLine.groupId = GetModGroupId(stRecModule.module_type);

    stDataLine.increaseId = 0;
    stDataLine.item.itemType = jce::EnumONADokiGalleryGroupCard;

    stOnaContainer.writeTo(stJceWriter);
    stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stDataLine);

    SPP_MSG_DEBUG_LOG("stOnaContainer.imageList[%d]", stOnaContainer.imageList.size());
}

//   B样式 -- 美图
int  CMsgOptSquare::CreateOnaWallPaperBView(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");

    //map<string, jce::ONADokiWallPaperScrollableList> mpWallPaper;
    jce::ONADokiWallPaperScrollableList stOnaContainer1;
    jce::ONADokiWallPaperScrollableList stOnaContainer2;

    for (size_t i = 0; i < m_vecWallPaper.size(); i++)
    {
        string& strDokiid = m_vecWallPaper[i];
        map<string, jce::ONADokiWallPaperItem>::iterator mpItor = m_mpWallPaper.find(strDokiid);

        if (mpItor != m_mpWallPaper.end())
        {
            mpItor->second.backgroundType = 1; //APP 动效需要
            mpItor->second.showUserInfo = false; 

            if (stOnaContainer1.wallpaperList.size() < 3)
                stOnaContainer1.wallpaperList.push_back(mpItor->second);
            else 
                stOnaContainer2.wallpaperList.push_back(mpItor->second);

            SPP_MSG_DEBUG_LOG("found wall paper. dokiid[%s]", strDokiid.c_str());
        }
    }

    ResizeLines(stRecModule.module_type, stOnaContainer1.wallpaperList);
    ResizeLines(stRecModule.module_type, stOnaContainer2.wallpaperList);

    if (stOnaContainer1.wallpaperList.empty() && stOnaContainer2.wallpaperList.empty())
    {
        SPP_MSG_ERROR_LOG("no wall paper found.");
        return 0;
    }

    if (!stOnaContainer1.wallpaperList.empty())
    {
        // 打包二进制数据
        taf::JceOutputStream<taf::BufferWriter>  stJceWriter;
        jce::TempletLine stDataLine;

        stDataLine.lineId = GetModGroupId(stRecModule.module_type);
        stDataLine.groupId = GetModGroupId(stRecModule.module_type);

        stDataLine.increaseId = 0;
        stDataLine.item.itemType = jce::EnumONADokiWallPaperScrollableList;
        stOnaContainer1.writeTo(stJceWriter);

        string strData((const char *)stJceWriter.getBuffer(), (size_t)stJceWriter.getLength());
        stDataLine.item.data.assign(strData.begin(), strData.end());

        vecLineData.push_back(stDataLine);
        SPP_MSG_DEBUG_LOG("stOnaContainer.wallpaperList[%d]", stOnaContainer1.wallpaperList.size());
    }

    if (!stOnaContainer2.wallpaperList.empty())
    {
        // 打包二进制数据
        taf::JceOutputStream<taf::BufferWriter>  stJceWriter;
        jce::TempletLine stDataLine;

        stDataLine.lineId = GetModGroupId(stRecModule.module_type);
        stDataLine.groupId = GetModGroupId(stRecModule.module_type);

        stDataLine.increaseId = 0;
        stDataLine.item.itemType = jce::EnumONADokiWallPaperScrollableList;
        stOnaContainer2.writeTo(stJceWriter);

        string strData((const char *)stJceWriter.getBuffer(), (size_t)stJceWriter.getLength());
        stDataLine.item.data.assign(strData.begin(), strData.end());

        vecLineData.push_back(stDataLine);
        SPP_MSG_DEBUG_LOG("stOnaContainer.wallpaperList[%d]", stOnaContainer2.wallpaperList.size());
    }
    return 0;
}

//   B样式 -- 表情包
int  CMsgOptSquare::CreateOnaEmoticonBView(
    UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    SPP_MSG_DEBUG_LOG("--");

    jce::ONADokiEmoticonList stOnaContainer1;
    jce::ONADokiEmoticonList stOnaContainer2;

    for (size_t i = 0; i < m_vecEmoticon.size(); i++)
    {
        string& strDokiid = m_vecEmoticon[i];
        map<string, jce::ONADokiEmoticonItem>::iterator mpItor = m_mpEmoticon.find(strDokiid);

        if (mpItor != m_mpEmoticon.end()) 
        {
            mpItor->second.backgroundType = 1;

            if (stOnaContainer1.emoticonList.size() < 3)
                stOnaContainer1.emoticonList.push_back(mpItor->second);
            else
                stOnaContainer2.emoticonList.push_back(mpItor->second);

            SPP_MSG_DEBUG_LOG("found emoticon. dokiid[%s]", strDokiid.c_str());
        }
    }

    ResizeLines(stRecModule.module_type, stOnaContainer1.emoticonList);
    ResizeLines(stRecModule.module_type, stOnaContainer2.emoticonList);

    if (stOnaContainer1.emoticonList.empty() && stOnaContainer2.emoticonList.empty())
    {
        SPP_MSG_ERROR_LOG("no wall paper found.");
        return 0;
    }

    // 打包二进制数据
    if (!stOnaContainer1.emoticonList.empty())
    {
        taf::JceOutputStream<taf::BufferWriter>  stJceWriter;
        jce::TempletLine stDataLine;

        stDataLine.lineId = GetModGroupId(stRecModule.module_type);
        stDataLine.groupId = GetModGroupId(stRecModule.module_type);

        stDataLine.increaseId = 0;
        stDataLine.item.itemType = jce::EnumONADokiEmoticonList;

        stOnaContainer1.writeTo(stJceWriter);
        stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
        vecLineData.push_back(stDataLine);

        SPP_MSG_DEBUG_LOG("stOnaContainer.emoticonList[%d]", stOnaContainer1.emoticonList.size());
    }

    if (!stOnaContainer2.emoticonList.empty())
    {
        taf::JceOutputStream<taf::BufferWriter>  stJceWriter;
        jce::TempletLine stDataLine;

        stDataLine.lineId = GetModGroupId(stRecModule.module_type);
        stDataLine.groupId = GetModGroupId(stRecModule.module_type);

        stDataLine.increaseId = 0;
        stDataLine.item.itemType = jce::EnumONADokiEmoticonList;

        stOnaContainer2.writeTo(stJceWriter);
        stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
        vecLineData.push_back(stDataLine);

        SPP_MSG_DEBUG_LOG("stOnaContainer.emoticonList[%d]", stOnaContainer2.emoticonList.size());
    }
    return 0;
}

int  CMsgOptSquare::CreateOnaEntNews( UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    // 
    if (IsSquareViewA())
    {
        CreateOnaEntNewsAndIPGroupAView(stRecModule, vecLineData);
    }
    else if (IsSquareViewB())
    {
        vector<string> vecFeedid;
        for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
        {
            const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
            if (stRecItem.item_type != EREC_TYPE_FEED && stRecItem.item_type != EREC_TYPE_CMS_SETTING)
            {
                SPP_MSG_ERROR_LOG("invalid item type[%d]", stRecItem.item_type);
                continue;
            }

            if (stRecItem.item_id.empty())
            {
                SPP_MSG_ERROR_LOG("invalid feedid or pid");
                continue;
            }

            // 
            vecFeedid.push_back(stRecItem.item_id);
            SPP_MSG_DEBUG_LOG("strFeedid[%s]", stRecItem.item_id.c_str());
        }

        vector<jce::TempletLine> vecTmpData;
        for (size_t i = 0; i < vecFeedid.size(); i++)
        {
            CreateOnaEntNewsBView(stRecModule, vecFeedid[i], vecTmpData);
        }

        ResizeLines(stRecModule.module_type, vecTmpData);

        // 是否存在
        if (!vecTmpData.empty())
        {
            // 下发标题
            string strCMSTitle = GetCMSTitle(stRecModule);

            // 标题groupid必须不一样 
            AppendTitle(vecLineData, strCMSTitle, GetModGroupId(stRecModule.module_type) + "Title");
            vecLineData.insert(vecLineData.end(), vecTmpData.begin(), vecTmpData.end());

            string strActUrl = DOKI_SECOND_PAGE_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));

            // 查看更多
            if (strCMSTitle.empty()) strCMSTitle = "娱乐资讯";
            AppendMore(vecLineData, "查看更多娱乐资讯", GetModGroupId(stRecModule.module_type), strActUrl, stRecModule);
        }
    }
    // 二级页和B样式采用同样onaview，逻辑略有差异
    else if (IsSecondPageView())
    {
        vector<string> vecFeedid;
        for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
        {
            const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
            if (stRecItem.item_type != EREC_TYPE_FEED && stRecItem.item_type != EREC_TYPE_CMS_SETTING)
            {
                SPP_MSG_ERROR_LOG("invalid item type[%d]", stRecItem.item_type);
                continue;
            }

            if (stRecItem.item_id.empty())
            {
                SPP_MSG_ERROR_LOG("invalid feedid or pid");
                continue;
            }

            // 
            vecFeedid.push_back(stRecItem.item_id);
            SPP_MSG_DEBUG_LOG("strFeedid[%s]", stRecItem.item_id.c_str());
        }

        // 二级页前后间距16
        for (size_t i = 0; i < vecFeedid.size(); i++)
        {
            CreateOnaEntNewsBView(stRecModule, vecFeedid[i], vecLineData);
            //AppendSpaceLine(vecLineData, 16);
        }
    }
}

int  CMsgOptSquare::CreateOnaEntNewsAndIPGroupAView( UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    jce::ONADokiCoverCard stOnaContainer;

    string strImgUrl;
    strImgUrl = GetCMSPoster(stRecModule, ESIZE_519_519);

    // 不足9张如何展示
    stOnaContainer.cardInfo.title = GetCMSTitle(stRecModule);
    stOnaContainer.cardInfo.subTitle = GetCMSSubTitle(stRecModule);
    
    if (strImgUrl.empty())
    {
        SPP_MSG_ERROR_LOG("no poster image found.");
        return 0;
    }

    jce::CircleMsgImageUrl stAppImg;

    stAppImg.url = strImgUrl;
    stOnaContainer.cardInfo.images.push_back(stAppImg);

    // 表情：正方图，壁纸：长方形图
    // 卡片整体跳转二级页
    string strExt; // 扩展上报参数
    if (stRecModule.module_type == E_MODID_ENTERTAINMENT_NEWS)  
    {
        stOnaContainer.cardInfo.cardAction.url = DOKI_SECOND_PAGE_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));
    }
    else { // ip group
        strExt = "sub_mod_id=" + stRecModule.module_id;
        stOnaContainer.cardInfo.cardAction.url = DOKI_SECOND_PAGE_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));
    }   

    AppendRecParam(stRecModule, strExt);
    SetReportParamViewA(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

    // 打包二进制数据
    jce::TempletLine stDataLine;

    stDataLine.lineId = GetModGroupId(stRecModule.module_type);
    stDataLine.groupId = GetModGroupId(stRecModule.module_type);

    stDataLine.increaseId = 0;
    stDataLine.item.itemType = jce::EnumONADokiCoverCard;

    stOnaContainer.writeTo(stJceWriter);
    stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stDataLine);

    SPP_MSG_DEBUG_LOG("stOnaContainer.");
}


//   B样式 -- 娱乐资讯
int  CMsgOptSquare::CreateOnaEntNewsBView( UniRecStruct::RecModule& stRecModule,
    const string& strFeedid, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    // 帖子信息
    if (strFeedid.empty())
    {
        SPP_MSG_ERROR_LOG("invalid. empty feedid");
        return 0;
    }

    // 普通帖子数据
    // 设计稿：http://tc-svn.tencent.com/video/video_videodzd_rep/iPhone_proj/document/ui_design/iPhone6.5.8/doki%E5%B9%BF%E5%9C%BAfeeds%E6%B5%81/preview/
    map<string, jce::CirclePrimaryFeed>::iterator mpItor = m_mpFeedidToDetail.find(strFeedid);
    if (mpItor == m_mpFeedidToDetail.end())
    {
        SPP_MSG_ERROR_LOG("feed info not found. mpItor == m_mpPidToPidInfo.end[%d]", mpItor == m_mpFeedidToDetail.end());
        return 0;
    }

    jce::ONADokiEntertamentNewsCard stOnaContainer;
    jce::CirclePrimaryFeed&  stFeedInfo = mpItor->second;

    FillFeedCard(stFeedInfo, stOnaContainer.cardInfo);
    stOnaContainer.cardInfo.userInfo.action = stFeedInfo.user.action;

    string strExt = "feed_id=" + strFeedid;
    AppendRecParam(stRecModule, strExt);
    SetReportParamViewB(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

    // 打包二进制数据
    jce::TempletLine stDataLine;

    // 二级页复用B样式onaview
    if (IsSecondPageView())
    {
        stDataLine.lineId = GetModGroupId(stRecModule.module_type, strFeedid);
        stDataLine.groupId = GetModGroupId(stRecModule.module_type, strFeedid);
    }
    else {
        stDataLine.lineId = GetModGroupId(stRecModule.module_type);
        stDataLine.groupId = GetModGroupId(stRecModule.module_type);
    }

    stDataLine.increaseId = 0;
    stDataLine.item.itemType = jce::EnumONADokiEntertamentNewsCard;

    stOnaContainer.writeTo(stJceWriter);
    stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stDataLine);

    SPP_MSG_DEBUG_LOG("add feedid[%s] content[%s]", strFeedid.c_str(), stFeedInfo.content.c_str());
    return 0;
}

//   B样式 -- IP聚合模块共用
int  CMsgOptSquare::CreateOnaIPGroup( UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;

    // 
    if (IsSquareViewA())
    {
        CreateOnaEntNewsAndIPGroupAView(stRecModule, vecLineData);
    }
    // 需求阶段确定：
    // 1\ 由推荐接口与cms运营配置共同保证，IP GROUP子模块连续下发给广场服务
    // 2\ IP GROUP模块不展示模块标题；广场模块依照推荐接口顺序展示，不考虑分页问题。
    else if (IsSquareViewB())
    {
        // 这里不需/无法再限制可出个数(ResizeLines)
        CreateOnaIPGroupBView(stRecModule, vecLineData);
    }
    // 二级页和B样式采用同样onaview，逻辑略有差异
    else if (IsSecondPageView())
    {
        CreateOnaIPGroupSecondPage(stRecModule, vecLineData);
    }
}

//   B样式 -- IP聚合模块共用
int  CMsgOptSquare::CreateOnaIPGroupBView( UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{ 
    taf::JceOutputStream<taf::BufferWriter>  stJceWriter;
    jce::ONADokiCoverCard stOnaContainer;

    // 不足9张如何展示
    stOnaContainer.cardInfo.title = GetCMSTitle(stRecModule);
    stOnaContainer.cardInfo.subTitle = GetCMSSubTitle(stRecModule);

    string strImgUrl = GetCMSPoster(stRecModule, ESIZE_519_519);
    if (strImgUrl.empty())
    {
        SPP_MSG_ERROR_LOG("no poster image found.");
        return 0;
    }

    jce::CircleMsgImageUrl stAppImg;

    stAppImg.url = strImgUrl;
    stOnaContainer.cardInfo.images.push_back(stAppImg);

    // 表情：正方图，壁纸：长方形图
    // 卡片整体跳转二级页
    stOnaContainer.cardInfo.cardAction.url = DOKI_SECOND_PAGE_ACTION_URL + CNewUrlCode::UrlEncode(GetSecondPageDataKey(stRecModule));
    
    string strExt = "sub_mod_id=" + stRecModule.module_id;
    AppendRecParam(stRecModule, strExt);
    SetReportParamViewB(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

    // 打包二进制数据
    jce::TempletLine stDataLine;

    stDataLine.lineId = GetModGroupId(stRecModule.module_type);
    stDataLine.groupId = GetModGroupId(stRecModule.module_type);

    stDataLine.increaseId = 0;
    stDataLine.item.itemType = jce::EnumONADokiCoverCard;

    stOnaContainer.writeTo(stJceWriter);
    stDataLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLineData.push_back(stDataLine);

    SPP_MSG_DEBUG_LOG("stOnaContainer.");
    return 0;
}

int  CMsgOptSquare::CreateOnaIPGroupSecondPage(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData)
{
    for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
    {
        UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];

        // 既不是feed，也不是配置的视频数据
        if (!IsRecItemFeed(stRecItem) && !IsRecItemCidVideo(stRecItem) && !IsRecItemVidVideo(stRecItem))
        {
            SPP_MSG_ERROR_LOG("invalid item type[%d] id[%s]", stRecItem.item_type, stRecItem.item_id.c_str());
            continue;
        }

        // 帖子数据 -- 普通帖子和配置帖子
        if (IsRecItemFeed(stRecItem))
        {
            string& strFeedid = stRecItem.item_id;
            map<string, jce::CirclePrimaryFeed>::iterator mpItor = m_mpFeedidToDetail.find(strFeedid);

            if (strFeedid.empty() || mpItor == m_mpFeedidToDetail.end())
            {
                SPP_MSG_ERROR_LOG("invalid feed. feedid[%s], mpItor == m_mpFeedidToDetail[%d]", strFeedid.c_str(), mpItor == m_mpFeedidToDetail.end());
                continue;
            }

            jce::CirclePrimaryFeed&  stFeedInfo = mpItor->second;

            // 与createOnaFeed的区别：
            // 本处不处明星说样式
            jce::ONADokiCommonFeedCard stOnaContainer;
            FillFeedCard(stFeedInfo, stOnaContainer.cardInfo);

            string strExt = "sub_mod_id=" + stRecModule.module_id;
            strExt += "&feed_id=" + strFeedid;

            AppendRecParam(stRecItem, strExt);
            SetSecondPageReportParam_IPGroup(stOnaContainer, strExt);

            // 打包二进制数据
            AppendTempleteLine(vecLineData, stOnaContainer, jce::EnumONADokiCommonFeedCard, GetModGroupId(E_MODID_FEED, strFeedid), GetModGroupId(E_MODID_FEED, strFeedid));
            SPP_MSG_DEBUG_LOG("add feedid[%s] content[%s]", strFeedid.c_str(), stFeedInfo.content.c_str());
        }
        // 配置的vid/cid数据
        else {
            string& strVid = stRecItem.item_id;

            jce::ONADokiVideoCoverCard stOnaContainer;
            FillVideoCard(stRecItem, stOnaContainer.cardInfo);

            string strExt = "sub_mod_id=" + stRecModule.module_id;
            strExt += "&vid=" + strVid;

            AppendRecParam(stRecItem, strExt);
            SetReportParamViewB(EModuleId(stRecModule.module_type), stOnaContainer, strExt);

            // 极其不合理！ APP同学制定的上报 一会按照功能，一会按照视图onaview类型，
            // 甚至同一个功能 既按照功能、也按照view类型上报，混乱不堪！找peterzkli
            stOnaContainer.reportEventId = "video_jce_poster_exposure";
            stOnaContainer.cardInfo.cardAction.reportEventId = "video_jce_action_click";

            // 打包二进制数据
            AppendTempleteLine(vecLineData, stOnaContainer, jce::EnumONADokiVideoCoverCard, GetModGroupId(E_MODID_FEED, strVid), GetModGroupId(E_MODID_FEED, strVid));
            SPP_MSG_DEBUG_LOG("add cms video[%s]", strVid.c_str());
        }
    }

    return 0;
}


void CMsgOptSquare::AppendMore(vector<jce::TempletLine>& vecLines,
    const string& strTitle, const string& strGroupId,
    const string& actionUrl, const UniRecStruct::RecModule& stRecModule)
{
    if (strTitle.empty())
    {
        SPP_MSG_ERROR_LOG("empty title");
        return;
    }

    // 娱乐资讯、美图、表情 三个模块不下发空白
    if (stRecModule.module_type != E_MODID_ENTERTAINMENT_NEWS
        && stRecModule.module_type != E_MODID_WALLPAPER
        && stRecModule.module_type != E_MODID_EMOTICON)
    {
        AppendSpaceLine(vecLines, 16);
    }

    // 构造标题onaview
    //-----------打包标题----------------------
    jce::ONADokiMoreInfoBar stOnaContainer;
    stOnaContainer.text = strTitle;

    // 数据上报
    {
        string strKey = "server_page_channel_" + m_jceChannelReq.channelItemId;
        string strParam = "data_type=button&sub_mod_id=more&mod_id=" + CNum2Str::Num2Str(stRecModule.module_type) + "&strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
        string strPageId = "&pageId=server_second_page_mod_" + CNum2Str::Num2Str(stRecModule.module_type) + "&strategyId=" + CNum2Str::Num2Str(m_iPolicyId);

        string strExpId = "common_button_item_exposure";
        string strClickId = "common_button_item_click";

        stOnaContainer.reportKey = strKey;
        stOnaContainer.reportParams = strParam;
        stOnaContainer.action.url = actionUrl;

        stOnaContainer.reportEventId = strExpId;
        stOnaContainer.action.reportKey = strKey;
        stOnaContainer.action.reportParams = strParam;
        stOnaContainer.action.reportEventId = strClickId;
        
        stOnaContainer.action.url += strPageId;
    }

    taf::JceOutputStream<taf::BufferWriter> stJceWriter;
    stOnaContainer.writeTo(stJceWriter);

    jce::TempletLine stTitleLine;
    stTitleLine.item.itemType = jce::EnumONADokiMoreInfoBar;
    stTitleLine.groupId = strGroupId;
    stTitleLine.increaseId = 0;
    stTitleLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLines.push_back(stTitleLine);

    AppendSpaceLine(vecLines, 16);
}

int  CMsgOptSquare::SetSecondPageReportParam_IPGroup(jce::ONADokiCommonFeedCard& stContainer, const string& strExtParam)
{
    string strParam, strPageId, strStrategy, strReportKey;

    strStrategy = "strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
    strParam = "mod_id=" + CNum2Str::Num2Str(E_MODID_IP_GROUP) + "&" + strStrategy;
    strPageId = "&pageId=server_second_page_mod_" + CNum2Str::Num2Str(E_MODID_IP_GROUP) + "&strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
    strReportKey = "server_second_page_mod_" + CNum2Str::Num2Str(E_MODID_IP_GROUP);


    strParam += "&" + strExtParam;
    stContainer.reportEventId = "feed_exposure";
    stContainer.cardInfo.cardAction.reportEventId = "feed_click";

    stContainer.reportKey = strReportKey;
    stContainer.cardInfo.cardAction.reportKey = strReportKey;

    stContainer.reportParams = strParam;
    stContainer.cardInfo.cardAction.reportParams = strParam;

    return 0;
}

void CMsgOptSquare::SetDokiItem(const StStarInfo&, jce::DokiInfo&)
{

}

void CMsgOptSquare::ConvertRecType(UniRecStruct::RecModule& stRecModule)
{
    // 推荐接口太他妈坑爹了， 原先约定二级页返回的module_type与一级页保持一致
    // 实际联调时只返回“置顶帖子”和“非置顶帖子”两种，太坑爹了！！
    if (IsSquareViewA() || IsSquareViewB() || IsSquareViewC())
    {
        switch (stRecModule.module_type)
        {
        case E_REC_MOD_CMS_SETTING:
            {
                string m_type = stRecModule.module_paras["m_type"];
                string ZT_leaf_ctype = stRecModule.module_paras["ZT_leaf_ctype"];

                // 占位符
                if (m_type == REC_KEY_CMS_PLACEHOLDER)
                {
                    // IP聚合模块
                    if (ZT_leaf_ctype == REC_KEY_PLACEHOLDER_IP)
                    {
                      stRecModule.module_type = E_MODID_IP_GROUP;
                    }
                    else if (ZT_leaf_ctype == REC_KEY_PLACEHOLDER_EMOTICON)
                    {
                        stRecModule.module_type = E_MODID_EMOTICON;
                    }
                    else if (ZT_leaf_ctype == REC_KEY_PLACEHOLDER_WALLPAPER)
                    {
                        stRecModule.module_type = E_MODID_WALLPAPER;
                    }
                    else if (ZT_leaf_ctype == REC_KEY_PLACEHOLDER_TOPTEN)
                    {
                        stRecModule.module_type = E_MODID_TOPTEN;
                    }
                    else if (ZT_leaf_ctype == REC_KEY_PLACEHOLDER_RENQI_UPDATE) // 榜单播报
                    {
                        stRecModule.module_type = E_MODID_DOKI_RENQI_UPDATE;
                    }
                    else if (ZT_leaf_ctype == REC_KEY_PLACEHOLDER_HOT_TOPIC)
                    {
                        stRecModule.module_type = E_MODID_HOT_TOPIC;
                    }
                    else
                    {
                        SPP_MSG_ERROR_LOG("invalid cms leaf type: %s", ZT_leaf_ctype.c_str());
                    }
                }
                else if (m_type == REC_KEY_CMS_BUMP_PIC)
                {
                    stRecModule.module_type = E_MODID_FOCUS_PIC;  // 焦点图
                }
                else if (m_type == REC_KEY_CMS_STAR_RANK_CHAIN)
                {
                    stRecModule.module_type = E_MODID_RENQI_MODCHAIN; // 榜单、福利社聚合入口 
                }
                else if (m_type == REC_KEY_CMS_DAILY_REPORT)
                {
                    stRecModule.module_type = E_MODID_DAIYLY_REPORT;  
                }
                else if (m_type == REC_KEY_CMS_IP_GROUP_A 
                    || m_type == REC_KEY_CMS_IP_GROUP_B)
                {
                    stRecModule.module_type = E_MODID_IP_GROUP;
                }
                else if (m_type == REC_KEY_CMS_BUMP_FEED)   // 置顶帖
                {
                    stRecModule.module_type = E_MODID_FEED; 
                }
                else
                {
                    SPP_MSG_ERROR_LOG("invalid cms m_type: %s", m_type.c_str());
                }
            }

            break;

        case E_REC_MOD_FEED:
            stRecModule.module_type = E_MODID_FEED;
            break;

        case E_REC_MOD_STAR_TALK:
            stRecModule.module_type = E_MODID_STAR_TALK;
            break;

        case E_REC_MOD_UGC_VIDEO:
            stRecModule.module_type = E_MODID_UGC_VIDEO;
            break;

        case E_REC_MOD_PGC_VIDEO:
            stRecModule.module_type = E_MODID_PGC_VIDEO;
            break;

        case E_REC_MOD_ENTERTAINMENT:
            stRecModule.module_type = E_MODID_ENTERTAINMENT_NEWS;
            break;

        default:
            SPP_MSG_ERROR_LOG("invalid rec module type: %d", stRecModule.module_type);
            break;
        }
    }
    else if (IsSecondPageView())
    {
        // 模块ID非法性检查已在解包阶段做过
        stRecModule.module_type = m_stDatakey.modid;
    }
}

void CMsgOptSquare::ProcessRecData(UniRecInterface::RecRsp& stRsp)
{
    bool doneTest = false;

    // 整理推荐数据
    for (size_t i = 0; i < stRsp.rec_modules.size(); i++)
    {
        UniRecStruct::RecModule& stRecModule = stRsp.rec_modules[i];

        // 推荐模块类型为一对多关系，譬如CMS占位类型，分为多个子类型
        // 本处将推荐模块类型转换为内部定义类型，实现一对一映射
        ConvertRecType(stRecModule);
        SPP_MSG_DEBUG_LOG("rec module: %s", GetModuleName(stRecModule.module_type).c_str());
        AppendBossKVData("recMod=" + GetModuleName(stRecModule.module_type));

        // 根据推荐数据，获取dokiid\feedid\pid等等，用于信息拉取
        StripRecDataAppend(stRecModule);

        // 获得需要二次读取数据的模块
        StripRecDataAppend(stRecModule, m_vecMidMoreWork);
    }
}

// 双瀑布feeds流
bool CMsgOptSquare::IsSquareViewA() const
{
    return (m_uiCmd == CMD_CHANNEL) && (m_iPolicyId == E_VIEW_A);
}

// 固定模块+双瀑布feeds流
bool CMsgOptSquare::IsSquareViewB() const
{
    return (m_uiCmd == CMD_CHANNEL) && (m_iPolicyId == E_VIEW_B);
}

bool CMsgOptSquare::IsSquareViewC() const
{
    return (m_uiCmd == CMD_CHANNEL) && (m_iPolicyId == E_VIEW_C);
}

bool CMsgOptSquare::IsSecondPageView() const
{
    return (m_uiCmd == CMD_SECOND_PAGE);
}

int CMsgOptSquare::GetSquareViewId()
{
    return m_iPolicyId;
}

bool CMsgOptSquare::IsSquareFirstPage() const
{
    return (m_uiCmd == CMD_CHANNEL) && (m_stDatakey.pagenum == 0);
}

int CMsgOptSquare::ConstructReqPack(UniRecInterface::RecReq& stReq)
{
    // 推荐参数说明
    // http://tapd.oa.com/REC_ALG/markdown_wikis/view/#1010154631008256673
    static long seqnum = 10000;
    seqnum++;

    if (IsSecondPageView())
    {
        SetSecondPageRecReqParam(m_stDatakey, stReq);
    }
    else {
        // 转为推荐翻页参数
        // 首页填空，第二页填page=1，第三页page=2，依次类推
        if (m_stDatakey.pagenum == 0){
            stReq.rec_ctx.ctx_paras["page_turn_info"] = "";
        }
        else{
            stReq.rec_ctx.ctx_paras["page_turn_info"] = "page=" + CNum2Str::Num2Str(m_stDatakey.pagenum);
        }

        // 模块数
        stReq.rec_ctx.ctx_paras["module_num"] = "8";
        stReq.rec_ctx.ctx_paras["module_item_num"] = "10";

        // ABTest样式
        if (IsSquareViewA())
        {
            // 固定模块 + 双瀑布feed流
            stReq.header.src_key = "110697"; 
        }
        else if (IsSquareViewB())
        {
            // 双瀑布feed流
            stReq.header.src_key = "100224";
        }
        else {
            SPP_MSG_ERROR_LOG("invalid req");
            return -1;
        }
    }

    stReq.rec_ctx.rec_scene = 2;
    stReq.header.seq_num = m_stVideoPacket.getGuid() + "_" + CNum2Str::Num2Str(time(NULL)) + "_" + CNum2Str::Num2Str(seqnum);
    
    SPP_MSG_DEBUG_LOG("seqnum[%s], src_key[%s], pageContext[%s]", stReq.header.seq_num.c_str(), stReq.header.src_key.c_str(), stReq.rec_ctx.ctx_paras["specified_modules"].c_str());
    return 0;
}


void CMsgOptSquare::StripRecDataAppend(UniRecStruct::RecModule& stRecModule)
{
    vector<string>& vecDokiid = m_vecDokiid;
    vector<string>& vecFeedid = m_vecFeedid;
    vector<string>& vecPid    = m_vecPid;
    vector<string>& vecWallPaper = m_vecWallPaper;
    vector<string>& vecEmoticon = m_vecEmoticon;

    switch (stRecModule.module_type)
    {
    case E_MODID_STAR_TALK:
        {
            if (stRecModule.rec_items.empty())
            {
                SPP_MSG_ERROR_LOG("star talk. empty rec_items");
                return;
            }

            AppendFeed(stRecModule, vecFeedid);
            AppendPidDoki(stRecModule, vecDokiid, vecPid, m_mpPidToDokiid);
        }
        break;

    case E_MODID_UGC_VIDEO:
        {
            // 广场页触发点击的帖子
            if (!m_stDatakey.extid.empty());
                vecFeedid.push_back(m_stDatakey.extid);

            AppendFeed(stRecModule, vecFeedid);
        }
        break;

    case E_MODID_FEED:
        {
            AppendFeed(stRecModule, vecFeedid);
            AppendVideo(stRecModule, m_vecCid, m_vecVid); // 普通feed可能出视频配置数据
        }
        break;

    case E_MODID_PGC_VIDEO:  // 饭制视频
    case E_MODID_EMOTICON:   // 
    case E_MODID_ENTERTAINMENT_NEWS:
        {
            AppendFeed(stRecModule, vecFeedid);
        }
        break;

    case E_MODID_IP_GROUP:     // 宫格聚合
        {
            // A、B两种样式，只展示模块封面图
            // 二级页数据 需要读取所有feedid信息
            if (IsSecondPageView())
            {
                AppendFeed(stRecModule, vecFeedid);
                AppendVideo(stRecModule, m_vecCid, m_vecVid);
            }
        }
        break;

   case E_MODID_TOPTEN:        // 十佳帖
        {
            
        }
        break;

    case E_MODID_WALLPAPER: // 推荐在此给或额外拉取接口获取，待定
    case E_MODID_DOKI_RENQI_UPDATE:   // 榜单播报，插件变更，UI变更
    case E_MODID_HOT_TOPIC:           // 热门话题   -- 无需变更，插件实现
    case E_MODID_DAIYLY_REPORT:       // 日报      -- 无需变更，插件实现
    case E_MODID_RENQI_MODCHAIN:      // 榜单、福利社入口  -- 无需变更，插件实现
    case E_MODID_DOKI_UPDATE_USR_FOLLOW: // 无此模块 
    case E_MODID_DOKI_UPDATE_RECOMMEND:  // 无此模块
    case E_MODID_VIDEO_TOPIC:         // AB样式均无此模块
    case E_MODID_DOKI_PARTY:          // 无此模块
        break;

    default:
        SPP_MSG_ERROR_LOG("invalid modid: %d", stRecModule.module_type);
        break;
    }
}


void CMsgOptSquare::StripRecDataAppend(const UniRecStruct::RecModule& stRecModule, vector<EModuleId>& vecUnfinished)
{
    switch (stRecModule.module_type)
    {
        case E_MODID_EMOTICON:
        case E_MODID_WALLPAPER:
        case E_MODID_FOCUS_PIC:     // 焦点图
        case E_MODID_TOPTEN:        // 读取二级页数据
            {
                // 需进一步RPC调用处理
                vecUnfinished.push_back(EModuleId(stRecModule.module_type));
                SPP_MSG_DEBUG_LOG("unfinished module[%d]", stRecModule.module_type);
            }
            break;

        default:
            break;
    }
}
   

// 主要是读取leafid，用于读取模块的详细信息
int CMsgOptSquare::GetCmsModuleRedisKey(vector<string>& vecModItemKey)
{
    int iRet = 0, iPlat = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    iPlat = (m_iPlatform == PLATFORM_IPHONE) ? 7 : 8;
    // 例子： get 944_100224_7_37001
    string strRedisKey = "944_" + m_jceChannelReq.channelItemId + "_" + CNum2Str::Num2Str(iPlat) + "_37001";

    COperateRedis *pGetCmsListTask = new COperateRedis(GET_CMS_SETTING);
    pGetCmsListTask->SetTaskKey(strRedisKey);
    pGetCmsListTask->SetReq_Str_Get(strRedisKey);
    oTaskExec.PushBack(pGetCmsListTask);

    iRet = oTaskExec.RunTaskList();
    if (0 != iRet)
    {
        SPP_MSG_ERROR_LOG("RunTaskError iExecRet[%d] ", iRet);
        return iRet;
    }

    iRet = pGetCmsListTask->GetResult();
    if (0 != iRet)
    {
        SPP_MSG_ERROR_LOG("Error TaskKey[%s] TaskRet[%d]", pGetCmsListTask->GetTaskKey().c_str(), iRet);
        return -1;
    }

    if (REDIS_DTYPE_STR != pGetCmsListTask->GetRspData().eRedisDataType
        || pGetCmsListTask->GetRspData().strResult.empty())
    {
        SPP_MSG_DEBUG_LOG("TaskKey[%s] valueType[%d] DataIsEmpty"
            , pGetCmsListTask->GetTaskKey().c_str(), pGetCmsListTask->GetRspData().eRedisDataType);
        return 0;
    }

    const string &strJsonModules = pGetCmsListTask->GetRspData().strResult;
    SPP_MSG_TRACE_LOG("strJsonModules = %s", strJsonModules.c_str());

    Json::Reader jsonReader;
    Json::Value  jsonModules;

    if (jsonReader.parse(strJsonModules, jsonModules) && jsonModules.isObject()
        && jsonModules.isMember("modlist") && jsonModules["modlist"].isArray())
    {
        Json::Value& jsonModuleList = jsonModules["modlist"];
        int iModNum = jsonModuleList.size();

        for (Json::Value::UInt uiIdx = 0; uiIdx < iModNum; uiIdx++)
        {
            Json::Value &jsonValue = jsonModuleList[uiIdx];

            string strMType, strLeafId, strTargetPlat;
            DEF_GET_JSON_STR(strMType, "mtype");
            DEF_GET_JSON_STR(strTargetPlat, "ZT_leaf_new_platform");

            SPP_MSG_DEBUG_LOG("strMType[%s] strLeafId[%s] Plat[%s]", 
                strMType.c_str(), strLeafId.c_str(), strTargetPlat.c_str());

            // 非焦点图
            if (strMType != "type_130")
            {
                continue;
                //SPP_MSG_DEBUG_LOG("module found. redis key[%s]", strKey.c_str());
            }

            // 没有platform参数，默认出
            bool bMatchPlatform = false;
            vector<string> vecPlatform;

            if (!strTargetPlat.empty())
            {
                CStrUnits::SplitToVec(strTargetPlat, "+", vecPlatform);

                for (size_t j = 0; j < vecPlatform.size(); j++)
                {
                    int iTarget = atoi(vecPlatform[j].c_str());
                    if (iTarget == iPlat || iTarget == -1) // -1表示全部平台均下发
                    {
                        bMatchPlatform = true;
                        break;
                    }
                }
            }
            else {
                bMatchPlatform = true;
            }

            // 找到匹配焦点图
            if (bMatchPlatform)
            {
                DEF_GET_JSON_STR(strLeafId, "leaf_id");
                string strKey = "944_ztid_" + m_jceChannelReq.channelItemId + "_leafid_" + strLeafId + "_p_" + CNum2Str::Num2Str(iPlat) + "_ver_37001";
                vecModItemKey.push_back(strKey);
                SPP_MSG_DEBUG_LOG("focus picture leafid found[%s]", strLeafId.c_str());
                break;
            }
        }
    }

    if (vecModItemKey.empty())
    {
        //Attr_API(NO_DAILY_REPORT_MOD, 1);
    }
}

// 读取key的详细信息
int CMsgOptSquare::GetCmsModuleConfDetail(const vector<string>& vecKeys, vector<StCmsItem>& vecItems)
{
    if (vecKeys.empty())
    {
        SPP_MSG_DEBUG_LOG("no keys, return");
        return 0;
    }

    int iRet = 0;
    string strErrMsg;
    CTaskExec oTaskExec; //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源

    int totalKeys = vecKeys.size();
    for (int iIndex = 0; iIndex < totalKeys; ++iIndex)
    {
        COperateRedis *pGetModuleConf = new COperateRedis(GET_CMS_SETTING);
        pGetModuleConf->SetTaskKey(vecKeys[iIndex]);
        pGetModuleConf->SetReq_Str_Get(vecKeys[iIndex]);
        oTaskExec.PushBack(pGetModuleConf);
    }

    iRet = oTaskExec.RunTaskList();
    if (0 != iRet)
    {
        SPP_MSG_ERROR_LOG("iExecRet[%d]", iRet);
        return iRet;
    }

    string       strConfDetail;
    map<string, string> mapKeyToVal;

    for (size_t uiIdx = 0; uiIdx < oTaskExec.GetVecPtrTask().size(); ++uiIdx)
    {
        IMtTask *pTask = oTaskExec.GetVecPtrTask()[uiIdx];
        if (GET_CMS_SETTING == pTask->GetTaskType())
        {
            COperateRedis *pGetModuleConf = (COperateRedis*)pTask;
            if (0 != pGetModuleConf->GetResult())
            {
                SPP_MSG_ERROR_LOG("GetCmsModuleConfDetail: Error TaskKey[%s] TaskRet[%d]",
                    pGetModuleConf->GetTaskKey().c_str(), pGetModuleConf->GetResult());
                continue;
            }

            if (REDIS_DTYPE_STR != pGetModuleConf->GetRspData().eRedisDataType
                || pGetModuleConf->GetRspData().strResult.empty())
            {
                //Attr_API(REDIS_HASH_GET_EMPTY, 1);

                SPP_MSG_DEBUG_LOG("GetCmsModuleConfDetail: taskKey[%s] valueType[%d] dataIsEmpty"
                    , pGetModuleConf->GetTaskKey().c_str(), pGetModuleConf->GetRspData().eRedisDataType);
                continue;
            }

            SPP_MSG_TRACE_LOG("GetCmsModuleConfDetail: key[%s] value[%s]"
                , pGetModuleConf->GetTaskKey().c_str(), pGetModuleConf->GetRspData().strResult.c_str());

            // 焦点图只有一项配置
            strConfDetail = pGetModuleConf->GetRspData().strResult;
            break;

            //mapKeyToVal[pGetModuleConf->SetTaskKey()] = pGetModuleConf->GetRspData().strResult;
        }
    }

    // 解析JSON数据
    if (strConfDetail.empty())
    {
        SPP_MSG_ERROR_LOG("detail empty");
        return 0;
    }

    // 
    Json::Reader jsonReader;
    Json::Value  jsonConf;

    // 解析 新闻项
    if (jsonReader.parse(strConfDetail, jsonConf) && jsonConf.isObject()
        && jsonConf.isMember("ZT_leaf_video") && jsonConf["ZT_leaf_video"].isArray())
    {
        Json::Value& jsonItemList = jsonConf["ZT_leaf_video"];
        int iNum = jsonItemList.size();

        for (Json::Value::UInt uiIdx = 0; uiIdx < iNum; uiIdx++)
        {
            Json::Value &jsonValue = jsonItemList[uiIdx];
            StCmsItem stNews;
            stNews.Parse(jsonValue, StCmsItem::EFOR_FOCUS_PIC);

            vecItems.push_back(stNews);
            SPP_MSG_TRACE_LOG("stNews, title[%s] url[%s] pic[%s]", stNews.strTitle.c_str(),
                stNews.strActionUrl.c_str(), stNews.strPic.c_str());
        }
    }

    // 日报的图标
    if (!vecItems.empty())
    {
        Json::Value &jsonValue = jsonConf;
        DEF_GET_JSON_STR(m_strDailyHeadUrl, "pic_70x40");
    }
    else{
        //Attr_API(NO_DAILY_REPORT_CONF, 1);
    }

    SPP_MSG_DEBUG_LOG("m_strDailyHeadUrl[%s] vecItems[%u]", m_strDailyHeadUrl.c_str(), vecItems.size());
    return 0;
}

// 推荐二级页source key
int CMsgOptSquare::SetSecondPageRecReqParam(const StDatakey& stDataKey, UniRecInterface::RecReq& stReq)
{
    string& strKey = stReq.header.src_key;
    stReq.rec_ctx.ctx_paras["module_num"] = "0";
    stReq.rec_ctx.ctx_paras["module_item_num"] = "0";

    // 翻页参数 -- 由omergao提供的硬编码
    // 翻页参数pagenum从0开始
    if (stDataKey.pagenum == 0)
    {
        stReq.rec_ctx.ctx_paras["page_turn_info"] = "";
        stReq.rec_ctx.ctx_paras["specified_modules"] = "video_tag^438^0^10";
    }
    else{
        stReq.rec_ctx.ctx_paras["page_turn_info"] = "1";
        stReq.rec_ctx.ctx_paras["specified_modules"] = "video_tag^439^0^10"; // 非首页
    }


    switch (stDataKey.modid)
    {
        case E_MODID_PGC_VIDEO:  // 饭制视频
        {
            strKey = "100224#square_fans_make";
        }
        break;

        case E_MODID_STAR_TALK: // 明星说
        {
            strKey = "100224#square_star_talk";
        }
        break;

        case E_MODID_UGC_VIDEO: // 饭拍展示2个
        {
            strKey = "100224#square_fans_shot";
        }
        break;

        case E_MODID_IP_GROUP: // 宫格模块
        {
            if (stDataKey.fromView == E_VIEW_A)
                strKey = "110697#cms_second";
            else
                strKey = "100224#cms_second";

            stReq.rec_ctx.ctx_paras["module_type"] = "410";
            stReq.rec_ctx.ctx_paras["module_id"] = stDataKey.strCMSId;
        }
        break;

        case E_MODID_ENTERTAINMENT_NEWS: // 
        {
            strKey = "100224#square_news";
            //stReq.rec_ctx.ctx_paras["module_type"] = "410";
            //stReq.rec_ctx.ctx_paras["module_id"] = stDataKey.strCMSId;
        }
        break;

        default:
            break;
    }

    SPP_MSG_DEBUG_LOG("strKey[%s], pageTurnInfo[%s], pageNum[%d]", strKey.c_str(), stReq.rec_ctx.ctx_paras["page_turn_info"].c_str(), stDataKey.pagenum);
    return 0;
}

bool CMsgOptSquare::IsVideoFeed(const jce::CirclePrimaryFeed& stFeedInfo, string &strPic)
{
    // wiki: http://tapd.oa.com/VideoServerDev/markdown_wikis/view/#1010095981007684155
    // 短视频
    bool bVideoFeed = false;

    if (!stFeedInfo.videos.empty() && !stFeedInfo.videos[0].vid.empty())
    {
        strPic = stFeedInfo.videos[0].imageUrl;
        bVideoFeed = true;
    }
    // 长视频
    else if (!stFeedInfo.bulletinBoardV2.videoData.vid.empty() ||
        !stFeedInfo.bulletinBoardV2.videoData.cid.empty() ||
        !stFeedInfo.bulletinBoardV2.pid.empty())
    {
        strPic = stFeedInfo.bulletinBoardV2.poster.imageUrl;
        bVideoFeed = true;
    }
    // 自拍视频
    else if (!stFeedInfo.selfVideo.videoData.vid.empty() ||
        !stFeedInfo.selfVideo.videoData.cid.empty())
    {
        strPic = stFeedInfo.selfVideo.coverPoster.imageUrl;
        bVideoFeed = true;
    }

    // 帖子可能不带封面图， 设置默认值
    if (bVideoFeed && strPic.empty())
        strPic = "https://puui.qpic.cn/vupload/0/20181225_1545725886519_7my0m879p77.png/0";  

    return bVideoFeed;
}

void CMsgOptSquare::SetVideoCover(const string& strVid, jce::CircleMsgImageUrl& stImage, jce::MarkLabel& stMarkLabel)
{
    stImage.maskType = 2;

    // 设置时长和宽高比
    if (m_mpVidToDetail.find(strVid) != m_mpVidToDetail.end())
    {
        StVideoInfo& video = m_mpVidToDetail[strVid];

        stImage.time = video.lDuration;
        stImage.aspectRatio = video.fWHRatio;

        // 播放时长角标
        stMarkLabel.type = 0;
        stMarkLabel.position = 3;

        if (video.lDuration >= 3600)
            stMarkLabel.primeText = CStrUnits::FormatTime_g(video.lDuration, "%H:%M:%S");
        else if (video.lDuration > 0)
            stMarkLabel.primeText = CStrUnits::FormatTime_g(video.lDuration, "%M:%S");
    }
}

bool CMsgOptSquare::IsVideoFeed(const jce::CirclePrimaryFeed& stFeedInfo)
{
    bool bVideoFeed = false;

    // 短视频
    if (!stFeedInfo.videos.empty() && !stFeedInfo.videos[0].vid.empty())
    {
        bVideoFeed = true;
    }
    // 长视频
    else if (!stFeedInfo.bulletinBoardV2.videoData.vid.empty() ||
        !stFeedInfo.bulletinBoardV2.videoData.cid.empty() ||
        !stFeedInfo.bulletinBoardV2.pid.empty())
    {
        bVideoFeed = true;
    }
    // 自拍视频
    else if (!stFeedInfo.selfVideo.videoData.vid.empty() ||
        !stFeedInfo.selfVideo.videoData.cid.empty())
    {
        bVideoFeed = true;
    }

    return bVideoFeed;
}

void CMsgOptSquare::GetVideoInfo(const jce::CirclePrimaryFeed& stFeedInfo, StVideoInfo& stInfo)
{
    // 短视频
    if (!stFeedInfo.videos.empty() && !stFeedInfo.videos[0].vid.empty())
    {
        stInfo.SetVid(stFeedInfo.videos[0].vid, stFeedInfo.videos[0].immersiveAction.url);
    }
    // 长视频
    else if (!stFeedInfo.bulletinBoardV2.videoData.vid.empty() ||
        !stFeedInfo.bulletinBoardV2.videoData.cid.empty() ||
        !stFeedInfo.bulletinBoardV2.pid.empty())
    {
        if (!stFeedInfo.bulletinBoardV2.videoData.vid.empty())
            stInfo.SetVid(stFeedInfo.bulletinBoardV2.videoData.vid, stFeedInfo.bulletinBoardV2.immersiveAction.url);
        else if (!stFeedInfo.bulletinBoardV2.videoData.cid.empty())
            stInfo.SetCid(stFeedInfo.bulletinBoardV2.videoData.cid, stFeedInfo.bulletinBoardV2.immersiveAction.url);
    }
    // 自拍视频
    else if (!stFeedInfo.selfVideo.videoData.vid.empty() ||
        !stFeedInfo.selfVideo.videoData.cid.empty())
    {
        if (!stFeedInfo.selfVideo.videoData.vid.empty())
            stInfo.SetVid(stFeedInfo.selfVideo.videoData.vid, stFeedInfo.selfVideo.immersiveAction.url);

        else if (!stFeedInfo.selfVideo.videoData.cid.empty())
            stInfo.SetCid(stFeedInfo.selfVideo.videoData.cid, stFeedInfo.selfVideo.immersiveAction.url);
    }

    //SPP_MSG_DEBUG_LOG("feedAction[%s]", stInfo.strActionUrl.c_str());
    return;
}

bool CMsgOptSquare::IsImageFeed(const jce::CirclePrimaryFeed& stFeedInfo, string &strPic)
{
    // wiki: http://tapd.oa.com/VideoServerDev/markdown_wikis/view/#1010095981007684155
    if (!stFeedInfo.photos.empty())
    {
        strPic = stFeedInfo.photos[0].url;
        return true;
    }

    return false;
}

bool CMsgOptSquare::IsImageFeed(const jce::CirclePrimaryFeed& stFeedInfo)
{
    if (!stFeedInfo.photos.empty())
    {
        return true;
    }

    return false;
}

void CMsgOptSquare::GetImageFaceArea(const jce::CirclePrimaryFeed& stFeedInfo, const string &strPic, jce::ImgFaceArea& area)
{
    // wiki: http://tapd.oa.com/VideoServerDev/markdown_wikis/view/#1010095981007684155
    if (!stFeedInfo.photos.empty())
    {
        for (int iIdx = 0; iIdx < stFeedInfo.photos.size(); iIdx++)
        {
            if (stFeedInfo.photos[iIdx].url == strPic)
            {
                area = stFeedInfo.photos[iIdx].faceArea;
                SPP_MSG_DEBUG_LOG("feedid(%s) x(%f), y(%f)", stFeedInfo.feedId.c_str(), area.xFloat, area.yFloat);
                return;
            }
        }

        SPP_MSG_DEBUG_LOG("feedid(%s) faceArea not found", stFeedInfo.feedId.c_str(), area.xFloat, area.yFloat);
    }

    return;
}

bool CMsgOptSquare::GetFeedImage(const jce::CirclePrimaryFeed& stFeedInfo, jce::CircleMsgImageUrl &stImg)
{
    // wiki: http://tapd.oa.com/VideoServerDev/markdown_wikis/view/#1010095981007684155
    if (!stFeedInfo.photos.empty())
    {
        stImg = stFeedInfo.photos[0];
        return true;
    }

    return false;
}

bool CMsgOptSquare::GetToptenFeedImage(const jce::CirclePrimaryFeed& stFeedInfo, 
    const CStarAdminPro::GoodFeedInfo& stFeedCfg, 
    jce::CircleMsgImageUrl &stImg)
{
    string strPic1, strPic2;
    bool   bVideoFeed = IsVideoFeed(stFeedInfo, strPic1);
    bool   bImgFeed = IsImageFeed(stFeedInfo, strPic2);

    if (!bImgFeed && !bVideoFeed)
    {
        SPP_MSG_DEBUG_LOG("neither imge feed nor video feed");
        return false;
    }

    // 优先出视频封面
    if (bVideoFeed && (!strPic1.empty() || !stFeedCfg.pic.empty()))
    {
        stImg.url = stFeedCfg.pic.empty() ? strPic1 : stFeedCfg.pic;
        stImg.maskType = 2;     //0-图片 2-视频
    }
    else if (bImgFeed && (!strPic2.empty() || !stFeedCfg.pic.empty()))
    {
        stImg.url = stFeedCfg.pic.empty() ? strPic2 : stFeedCfg.pic;
        stImg.maskType = 0;     //0-图片 2-视频
        GetImageFaceArea(stFeedInfo, stImg.url, stImg.faceArea);
    }
    else {
        SPP_MSG_DEBUG_LOG("feed doesn't have cover pic");
        return false;
    }

    SPP_MSG_DEBUG_LOG("feedid[%s], opertime[%ld]", stFeedCfg.feedid.c_str(), stFeedCfg.opertime);
    return true;
}

string CMsgOptSquare::GetCommentKey(const StNewPidInfo &stPidInfo)
{
    string strCommentKey = "pid=" + stPidInfo.strPid + "&roseId=" + stPidInfo.strRoseId + "&targetid=" + stPidInfo.strTargetId + "&source=9";
    return strCommentKey;
}

string CMsgOptSquare::GetCMSTitle(UniRecStruct::RecModule& stRecModule)
{
    return stRecModule.module_paras["module_rec_title"];
}

string CMsgOptSquare::GetCMSSubTitle(UniRecStruct::RecModule& stRecModule)
{
    return stRecModule.module_paras["sub_title"];
}

string CMsgOptSquare::GetCMSPoster(UniRecStruct::RecModule& stRecModule,  EPicSize eSize)
{
    // 推荐接口参数约定
    // pic_456x810; value: 456 * 810 尺寸的封面图，爱豆美图、十佳饭贴、娱乐资讯、爱豆表情包用
    // pic_70x40; value: 70 * 40 尺寸的封面图，日报用
    // ZT_leaf_pic_type； value : 小图横滑图片样式， 2 - 竖图 3 - 横图

    if (eSize == ESIZE_456_810)
        return stRecModule.module_paras["pic_456x810"];
    else if(eSize == ESIZE_316_178)
        return stRecModule.module_paras["pic_316x178"];
    else if(eSize == ESIZE_70_40)
        return stRecModule.module_paras["pic_70x40"];
    else if (eSize == ESIZE_519_519)
        return stRecModule.module_paras["pic_519x519"];
    else
        return stRecModule.module_paras["pic_456x810"];
}

string CMsgOptSquare::GetModGroupId(int iModId, const string& strFeedid)
{
    string strPrefix;
    switch (iModId)
    {
        case E_MODID_STAR_TALK:
            {
               strPrefix= "starTalk";          
            }
            break;

        case E_MODID_FEED:
            {
               strPrefix = "feed";
            }
            break;

        case E_MODID_PGC_VIDEO:
            {
               strPrefix =  "pgcVideo";
            }
            break;

        case E_MODID_UGC_VIDEO:
            {
               strPrefix = "ugcVideo";
            }
            break;

        case E_MODID_WALLPAPER:
            {
               strPrefix = "wallPaper";
            }
            break;

        case E_MODID_EMOTICON:
            {
               strPrefix = "emoticon";
            }
            break;

        case E_MODID_ENTERTAINMENT_NEWS:
            {
               strPrefix = "entNews";
            }
            break;

        case E_MODID_IP_GROUP:
            {
               strPrefix = "ipGroup";
            }
            break;

        default:
            break;
    }

    if (!strFeedid.empty())
        return strPrefix + "_" + strFeedid;
    else 
        return strPrefix;
}

void CMsgOptSquare::FillFeedCard(const jce::CirclePrimaryFeed& stFeedInfo, jce::DokiCardBaseInfo& stFeedCard)
{
    // 帖子正文
    stFeedCard.title = stFeedInfo.feedTitle.empty() ? stFeedInfo.content : stFeedInfo.feedTitle;
    stFeedCard.feedId = stFeedInfo.feedId;

    // 图片信息 - 与peterzkli确认：后台下发图片总数，剩余XX张APP展示
    // 视频信息 - 与peterzkli确认：复用photos结构；视频取第一张封面图，设置数据类型。

    // 优先展示视频信息
    string strVidPic; //视频封面图
    if (IsVideoFeed(stFeedInfo, strVidPic) && !strVidPic.empty())
    {
        StVideoInfo video;

        jce::CircleMsgImageUrl stImage;
        jce::MarkLabel         stMark;

        stImage.url = strVidPic;
        stImage.maskType = 2;

        // 读取宽高比信息、跳转链接
        GetVideoInfo(stFeedInfo, video);

        // 设置时长和宽高比
        SetVideoCover(video.GetId(), stImage, stMark); // 只针对vid
        
        // 需求初始： 
        // 时长信息 - 视频角标与帖子详情保持一致
        // 只有长视频有时长，自拍与短视频都没有

        // 现在以媒资库为准
        stFeedCard.images.push_back(stImage);

        if (!stMark.primeText.empty()) 
        stFeedCard.marklabels.push_back(stMark);

        // 设置跳转链接 -- 跳转沉浸式
        // APP668以上支持，老版本仍跳转帖子详情页
        if (m_bIsVerGe668)
        stFeedCard.cardAction.url = video.strActionUrl; 
    }
    // 图片帖子
    else if (!stFeedInfo.photos.empty())
    {
        // 图片展示总张数
        jce::MarkLabel stMarkLabel;

        stMarkLabel.type = 0;
        stMarkLabel.position = 3;

        stMarkLabel.primeText = CNum2Str::Num2Str(stFeedInfo.photos.size()) + "张";
        stFeedCard.marklabels.push_back(stMarkLabel);

        stFeedCard.images = stFeedInfo.photos;
    }
    // 其它类型帖子
    else 
    {
       
    }

    // 语音信息
    if (!stFeedInfo.voices.empty())
    {
        // stOnaContainer.voiceData = stFeedInfo.voices[0];
    }

    // 默认跳转帖子详情
    if (stFeedCard.cardAction.url.empty())
    stFeedCard.cardAction.url = stFeedInfo.feedAction.url; 

    // 发表时间
    stFeedCard.pubTime = stFeedInfo.time;
    stFeedCard.likeCount = stFeedInfo.likeCount;
    stFeedCard.feedId = stFeedInfo.feedId;
    stFeedCard.feedDataKey = stFeedInfo.dataKey;

    // 明星信息
    stFeedCard.userInfo.dokiName = stFeedInfo.user.actorName;
    stFeedCard.userInfo.dokiImgUrl = stFeedInfo.user.faceImageUrl;
    //stFeedCard.userInfo.action = stFeedInfo.user.action;

    // 明星样式
    if (stFeedInfo.user.userType == 1)
    stFeedCard.userInfo.badgeInfo.iconUrl = DOKI_VIP_ICON;// 大V角标
}

void CMsgOptSquare::FillVideoCard(UniRecStruct::RecItem& stRecItem, jce::DokiCardBaseInfo& stFeedCard)
{
    stFeedCard.title = stRecItem.item_paras["scms_title"];
    stFeedCard.subTitle = stRecItem.item_paras["scms_sectitle"];

    string strId     = stRecItem.item_id;
    string strHorPic = stRecItem.item_paras["scms_pic_316x178"];
    string strVerPic = stRecItem.item_paras["scms_pic_314x596"];
    string strUrl    = stRecItem.item_paras["scms_url"];

    jce::CircleMsgImageUrl stImage;
    jce::MarkLabel         stMark;

    // 跳转
    if (IsRecItemCidVideo(stRecItem))
    {
        stFeedCard.cardAction.url = string(VIDEO_ACTION_URL) + "?cid=" + strId + "&isAutoPlay=1";
    }
    else {
        stFeedCard.cardAction.url = string(VIDEO_ACTION_URL) + "?vid=" + strId + "&isAutoPlay=1";
    }

    // 设置时长和宽高比
    SetVideoCover(strId, stImage, stMark); // 只针对vid
    stImage.url = (stImage.aspectRatio <= 1.0 && !strVerPic.empty()) ? strVerPic : strHorPic;
    //SPP_MSG_DEBUG_LOG("vid[%s] ratio[%f] horPic[%s] verPic[%s] url[%s]", 
    //    strId.c_str(), stImage.aspectRatio, strHorPic.c_str(), strVerPic.c_str(), stImage.url.c_str());

    if (!stMark.primeText.empty()) 
        stFeedCard.marklabels.push_back(stMark);

    stFeedCard.images.push_back(stImage);
}

void CMsgOptSquare::GetFeedVoice(const jce::CirclePrimaryFeed& stFeedInfo, jce::ApolloVoiceData& stVoice)
{
    // 语音信息
    if (!stFeedInfo.voices.empty())
    {
        stVoice = stFeedInfo.voices[0];
    }
}

string CMsgOptSquare::GetSecondPageDataKey(UniRecStruct::RecModule& stRecModule)
{
    string datakey;

    switch (stRecModule.module_type)
    {
        case E_MODID_PGC_VIDEO:  // 饭制视频
        case E_MODID_STAR_TALK:  // 明星说
        case E_MODID_UGC_VIDEO:  // 饭拍展示2个
        case E_MODID_ENTERTAINMENT_NEWS: //
        {
            datakey = "modid=" + CNum2Str::Num2Str(stRecModule.module_type);
        }
        break;

        // IP宫格模块二级页src_key不同，需要区分来自哪种广场页样式
        case E_MODID_IP_GROUP:   // 宫格模块 
        {
             datakey = "modid=" + CNum2Str::Num2Str(stRecModule.module_type) + "&cmsid=" + stRecModule.module_id;
             datakey += "&fromview=";
             datakey += (GetSquareViewId() == E_VIEW_A) ? CNum2Str::Num2Str(E_VIEW_A) : CNum2Str::Num2Str(E_VIEW_B);
        }
        break;

        case E_MODID_EMOTICON:
        case E_MODID_WALLPAPER:
            if (IsSquareViewA() || IsSquareViewB())
                datakey = "pfrom=0&ftid=0&type=0&targetTabId=0";

            break;

        default:
            break;
    }

    SPP_MSG_DEBUG_LOG("mid[%d] datakey[%s]", stRecModule.module_type, datakey.c_str());
    return datakey;
}

string CMsgOptSquare::GetModuleName(int iModid)
{
    switch (iModid)
    {
    case E_MODID_FOCUS_PIC:           // 焦点图
        return "E_MODID_FOCUS_PIC";

    case E_MODID_DOKI_RENQI_UPDATE:   // 榜单播报，插件变更，UI变更
        return "E_MODID_DOKI_RENQI_UPDATE"; 

    case E_MODID_HOT_TOPIC:           // 热门话题   -- 无需变更，插件实现
        return "E_MODID_HOT_TOPIC";

    case E_MODID_DAIYLY_REPORT:       // 日报      -- 无需变更，插件实现
        return "E_MODID_DAIYLY_REPORT";

    case E_MODID_RENQI_MODCHAIN:      // 榜单、福利社入口  -- 无需变更，插件实现
        return "E_MODID_RENQI_MODCHAIN";

    case E_MODID_FEED:
        return "E_MODID_FEED";

    case E_MODID_STAR_TALK:
        return "E_MODID_STAR_TALK";

    case E_MODID_TOPTEN:
        return "E_MODID_TOPTEN";

    case E_MODID_PGC_VIDEO:  //饭制视频
        return "E_MODID_PGC_VIDEO";

    case E_MODID_UGC_VIDEO:
        return "E_MODID_UGC_VIDEO";

    case E_MODID_WALLPAPER: // 推荐在此给或额外拉取接口获取，待定
        return "E_MODID_WALLPAPER";

    case E_MODID_EMOTICON: // 
        return "E_MODID_EMOTICON";

    case E_MODID_ENTERTAINMENT_NEWS:
        return "E_MODID_ENTERTAINMENT_NEWS";

    case E_MODID_IP_GROUP:     // 宫格聚合
        return "E_MODID_IP_GROUP";

    case E_MODID_DOKI_UPDATE_USR_FOLLOW: // 无此模块 
        return "E_MODID_DOKI_UPDATE_USR_FOLLOW";

    case E_MODID_DOKI_UPDATE_RECOMMEND:  // 无此模块
        return "E_MODID_DOKI_UPDATE_RECOMMEND";

    case E_MODID_VIDEO_TOPIC:         // AB样式均无此模块
        return "E_MODID_VIDEO_TOPIC";

    case E_MODID_DOKI_PARTY:          // 无此模块
        return "E_MODID_DOKI_PARTY";

    default:
        return "invalid modid[" + CNum2Str::Num2Str(iModid) + "]";
    }

    return "";
}

string CMsgOptSquare::GetViewName(int iPolicyId)
{
    switch (iPolicyId)
    {
    case E_VIEW_A:          
        return "E_VIEW_A";

    case E_VIEW_B:          
        return "E_VIEW_B";

    case E_VIEW_C:        
        return "E_VIEW_C";

    default:
        break;
    }

    return "unkownView";
}

bool CMsgOptSquare::GetWallPaperImg(const jce::DokiWallPaperItem& stItem, string& strImage)
{
    if (!stItem.liveWallpaperItem.preViewGifUrl.empty())
    {
        strImage = stItem.liveWallpaperItem.preViewGifUrl;
        return true;
    }
    else if (!stItem.picWallpaperItem.imageInfo.url.empty())
    {
        strImage = stItem.picWallpaperItem.imageInfo.url;
        return true;
    }

    return false;
}

bool CMsgOptSquare::GetEmoticonImg(const jce::ONADokiEmoticonItem& stItem, string& strImage)
{
    if (!stItem.data.url.empty())
    {
        strImage = stItem.data.url;
        return true;
    }

    return false;
}

void CMsgOptSquare::AppendFeed(const UniRecStruct::RecModule& stRecModule, vector<string>& vecFeedid)
{
    for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
    {
        const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
        if (stRecItem.item_type != EREC_TYPE_FEED && stRecItem.item_type != EREC_TYPE_CMS_SETTING)
        {
            SPP_MSG_ERROR_LOG("%s. invalid item type[%d]", GetModuleName(stRecModule.module_type).c_str(), stRecItem.item_type);
            continue;
        }

        if (stRecItem.item_id.empty())
        {
            SPP_MSG_ERROR_LOG("%s. invalid feedid", GetModuleName(stRecModule.module_type).c_str());
            continue;
        }

        vecFeedid.push_back(stRecItem.item_id);

        // 数据分析组啥都不愿意填，模块id放在key中极不合理！
        // 668后禁止这种上报方式！
        m_mpFeedidToModid[stRecItem.item_id] = stRecModule.module_type;
        SPP_MSG_DEBUG_LOG("%s. feed id[%s]", GetModuleName(stRecModule.module_type).c_str(), stRecItem.item_id.c_str());
    }
}


bool CMsgOptSquare::IsRecItemFeed(UniRecStruct::RecItem& stRecItem)
{
    if (stRecItem.item_id.empty())
    {
        SPP_MSG_DEBUG_LOG("invalid id");
        return false;
    }

    if (stRecItem.item_type == EREC_TYPE_FEED ||
        (stRecItem.item_type == EREC_TYPE_CMS_SETTING && (stRecItem.item_paras["scms_ctype"] == "forum"))
        )
    {
        SPP_MSG_DEBUG_LOG("feed info. id[%s]", stRecItem.item_id.c_str());
        return true;
    }

    return false;
}

bool CMsgOptSquare::IsRecItemCidVideo(UniRecStruct::RecItem& stRecItem)
{
    if (stRecItem.item_id.empty())
    {
        SPP_MSG_DEBUG_LOG("invalid id");
        return false;
    }

    if (stRecItem.item_type == EREC_TYPE_CMS_COVER && (stRecItem.item_paras["scms_ctype"] == "album"))
    {
        SPP_MSG_DEBUG_LOG("cms setting. id[%s]", stRecItem.item_id.c_str());
        return true;
    }

    return false;
}

bool CMsgOptSquare::IsRecItemVidVideo(UniRecStruct::RecItem& stRecItem)
{
    if (stRecItem.item_id.empty())
    {
        SPP_MSG_DEBUG_LOG("invalid id");
        return false;
    }

    if (stRecItem.item_type == EREC_TYPE_CMS_VIDEO && (stRecItem.item_paras["scms_ctype"] == "video"))
    {
        SPP_MSG_DEBUG_LOG("cms setting. id[%s]", stRecItem.item_id.c_str());
        return true;
    }

    return false;
}


bool CMsgOptSquare::IsRecItemPid(UniRecStruct::RecItem& stRecItem)
{
    return false;
}

void CMsgOptSquare::AppendPidDoki(const UniRecStruct::RecModule& stRecModule, vector<string>& vecDokiid, 
    vector<string>& vecPid, map<string, vector<string> >& mpPidToDokiid)
{
    for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
    {
        const UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];
        if (stRecItem.item_type != EREC_TYPE_FEED && stRecItem.item_type != EREC_TYPE_PID)
        {
            SPP_MSG_ERROR_LOG("invalid item type[%d]", stRecItem.item_type);
            continue;
        }

        if (stRecItem.item_id.empty())
        {
            SPP_MSG_ERROR_LOG("%s. invalid feedid|pid", GetModuleName(stRecModule.module_type).c_str());
            continue;
        }

        map<string, string>::const_iterator mpItor = stRecItem.item_paras.find("item_rec_reason");
        if (mpItor == stRecItem.item_paras.end())
        {
            SPP_MSG_ERROR_LOG("%s. item_id[%s], item_rec_reason not found.", 
                GetModuleName(stRecModule.module_type).c_str(), stRecItem.item_id.c_str());
            continue;
        }

        vector<string> tmpDokiid;
        CStrUnits::SplitToVec(mpItor->second, "&", tmpDokiid);

        if (!tmpDokiid.empty())
        {
            for (size_t i = 0; i < tmpDokiid.size(); i++)
            {
                string strDokiid = tmpDokiid[i];

                // 存在除0123456789外的字符
                if (strDokiid.find_first_not_of("0123456789") != string::npos)
                {
                    // 非法参数
                    SPP_MSG_DEBUG_LOG("%s. item_id[%s], invalid strDokiid[%s].",
                        GetModuleName(stRecModule.module_type).c_str(), 
                        stRecItem.item_id.c_str(), strDokiid.c_str());

                    continue;
                }

                vecDokiid.push_back(strDokiid);

                // 同个明星配置多个，后出的pid会覆盖前面的
                if (stRecItem.item_type == EREC_TYPE_PID)
                {
                    const string& strPid = stRecItem.item_id;
                    mpPidToDokiid[strPid].push_back(strDokiid);
                }
            }

            // 同个明星配置多个，后出的pid会覆盖前面的
            if (stRecItem.item_type == EREC_TYPE_PID)
            {
                const string& strPid = stRecItem.item_id;
                vecPid.push_back(strPid);
            }
        }

        SPP_MSG_DEBUG_LOG("%s. item_id[%s], item_rec_reason[%s].", GetModuleName(stRecModule.module_type).c_str(), 
            stRecItem.item_id.c_str(), mpItor->second.c_str());
    }
}

void CMsgOptSquare::AppendVideo(UniRecStruct::RecModule& stRecModule, vector<string>& vecCid, vector<string>& vecVid)
{
    for (size_t i = 0; i < stRecModule.rec_items.size(); i++)
    {
        UniRecStruct::RecItem& stRecItem = stRecModule.rec_items[i];

        if (IsRecItemCidVideo(stRecItem))
        {
            vecCid.push_back(stRecItem.item_id);

            SPP_MSG_DEBUG_LOG("%s. get cid[%s]",  GetModuleName(stRecModule.module_type).c_str(), stRecItem.item_id.c_str());
            continue;
        }

        if (IsRecItemVidVideo(stRecItem))
        {
            vecVid.push_back(stRecItem.item_id);

            SPP_MSG_DEBUG_LOG("%s. get vid[%s]",  GetModuleName(stRecModule.module_type).c_str(), stRecItem.item_id.c_str());
            continue;
        }
    }
}

void CMsgOptSquare::AppendRecParam(UniRecStruct::RecModule& stRecModule, string& strExt)
{
    if (!stRecModule.rec_items.empty())
        strExt += "&algid=" + stRecModule.rec_items[0].item_paras["recall_alg"]; // 算法id，用于改进模型

    strExt += "&bucketId=" + m_stRecRsp.rsp_paras["exp_id"];
    strExt += "&seqnum=" + m_stRecRsp.header.seq_num;
}

void CMsgOptSquare::AppendRecParam(UniRecStruct::RecItem& stRecItem, string& strExt)
{
    strExt += "&algid=" + stRecItem.item_paras["recall_alg"]; // 算法id，用于改进模型
    strExt += "&bucketId=" + m_stRecRsp.rsp_paras["exp_id"];
    strExt += "&seqnum=" + m_stRecRsp.header.seq_num;
}

int CMsgOptSquare::ReportBoss()
{
    m_stBossData.lBossId = BOSS_ID;

    // 请求数据
    static char szBuffer[4096] = { '\0' }; //数据库扩展字段最大长度
    if (m_uiCmd == CMD_CHANNEL)
    {
        // 请求参数
        snprintf(szBuffer, sizeof(szBuffer)-1, "(req.channelItemId=%s)(req.pageContext=%s)(strategyId=%s)(bucketId=%d)",
            m_jceChannelReq.channelItemId.c_str(), m_jceChannelReq.pageContext.c_str(), GetViewName(m_iPolicyId).c_str(), m_iBucketId);
        m_stBossData.strKvData += szBuffer;

        // 返回参数
        snprintf(szBuffer, sizeof(szBuffer)-1, "(rsp.errCode=%d)(rsp.hasNextPage=%d)(rsp.pageContext=%s)(rsp.data.size=%d)",
            m_jceChannelRsp.errCode, m_jceChannelRsp.hasNextPage, m_jceChannelRsp.pageContext.c_str(), m_jceChannelRsp.data.size());
        m_stBossData.strKvData += szBuffer;
    }
    else if (m_uiCmd == CMD_SECOND_PAGE)
    {
        // 请求参数
        snprintf(szBuffer, sizeof(szBuffer)-1, "(req.datakey=%s)(req.pageContext=%s)(strategyId=%d)(bucketId=%d)",
            m_jceSecondPageReq.dataKey.c_str(), m_jceSecondPageReq.pageContext.c_str(), m_iPolicyId, m_iBucketId);
        m_stBossData.strKvData += szBuffer;

        // 返回参数
        snprintf(szBuffer, sizeof(szBuffer)-1, "(rsp.errCode=%d)(rsp.hasNextPage=%d)"
            "(rsp.pageContext=%s)(rsp.data.size=%d)(rsp.title=%s)",
            m_jceSecondPageRsp.errorCode, m_jceSecondPageRsp.hasNextPage, 
            m_jceSecondPageRsp.pageContext.c_str(), m_jceSecondPageRsp.uiData.size(),
            m_jceSecondPageRsp.title.c_str());

        m_stBossData.strKvData += szBuffer;
    }

    CMsgOptBase::ReportBoss();
}