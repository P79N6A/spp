/*
 *  Filename: msg_opt_base.cpp
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 * 	Description: 基础操作基类，给继承类提供公共的函数接口
 * 			1. 不要在CMsgOptBase中处理业务逻辑，各个命令字、http请求路由到不同的继承类中处理，这样程序接口更清晰
 * 			2. CMsgOptBase除了添加一些各继承类共用的函数接口以外，其他的代码基本不用修改
 *
 */

#include "msg_opt_base.h"
#include "myUtilsTVideo.h"

// =====主体处理函数=============================================================================================
int CMsgOptBase::HandleProcess()
{
    Attr_API(SPP_REQ_ALL, 1);
    int iRet = EncodeAndCheckReqData(); //解包

    if (0 == iRet)
    {
        iRet = DealProcess(); //业务逻辑处理
        if (0 == iRet)
            Attr_API(SPP_REQ_SUCC, 1);
        else
            Attr_API(SPP_REQ_FAIL, 1);
    }
    else
    {
        Attr_API(REQ_DATA_ERR, 1);
    }

    SendRspData(); //回包

    int iCostTime_ms = GetMsgCost();
    Attr_API_Avg(SPP_REQ_AVG_TIME, iCostTime_ms);

    if(0 == m_iError)
        SPP_MSG_TRACE_LOG("rspDataInfo: optSucc m_iError[%d] costtime[%dms]", m_iError, iCostTime_ms);
    else
        SPP_MSG_ERROR_LOG("rspDataInfo: optErr  m_iError[%d] costtime[%dms]", m_iError, iCostTime_ms);

    ReportBoss();
    return 0;
}

// ====公共回包函数，供继承CMsgOptBase的类使用=======================================================================
void CMsgOptBase::SendRspDataCommBuf(char *pData, uint32_t uiLen)
{
    if (m_bSendAhead) //已经提前回包了，直接返回
    {
        SPP_MSG_TRACE_LOG("sendRspData: has already sendData");
        return;
    }

    blob_type blob;
    blob.data = pData;
    blob.len  = uiLen;

    this->SendToClient(blob); //传递给blob的数据一定要在调用SendToClient时是可访问的
    m_bSendAhead = true;
    return;
}

void CMsgOptBase::SendRspDataEmpty()
{
    if (m_bSendAhead) //已经提前回包了，直接返回
    {
        SPP_MSG_TRACE_LOG("sendRspData: has already sendData");
        return;
    }

    blob_type blob;
    blob.data = NULL;
    blob.len  = 0;

    this->SendToClient(blob); //传递给blob的数据一定要在调用SendToClient时是可访问的
    m_bSendAhead = true;
    return;
}

void CMsgOptBase::SendRspDataSnowball(int iErr, const string& strMsg, CVideoPacket &oVideoPacket)
{
    if (m_bSendAhead) //已经提前回包了，直接返回
    {
        SPP_MSG_TRACE_LOG("sendRspData: has already sendData");
        return;
    }

    blob_type blob;
    blob.data = NULL;
    blob.len = 0;

    try
    {
        jce::stSnowBallRsp jceRsp;
        jceRsp.errcode = iErr;
        jceRsp.errmsg = strMsg;

        taf::JceOutputStream<taf::BufferWriter> osJce;
        jceRsp.writeTo(osJce);

        oVideoPacket.setBody(osJce.getBuffer(), osJce.getLength()); //函数限制了最大的传递buf为2M
        oVideoPacket.encode();
        SPP_MSG_TRACE_LOG("SendRspDataSnowball: iErr[%d],errmsg[%s]", iErr, strMsg.c_str());
    }
    catch (exception &e)
    {
        Attr_API(JCE_EN_ERR, 1);
        SPP_MSG_ERROR_LOG("SendRspDataSnowball: SetVPacketError, jceDecodeError,errmsg[%s]", e.what());

        this->SendToClient(blob); //数据异常、回一个空包
        m_bSendAhead = true;
        return;
    }

    blob.data = (char *)oVideoPacket.getPacket();
    blob.len = oVideoPacket.getPacketLen();
    this->SendToClient(blob); //传递给blob的数据一定要在调用SendToClient时是可访问的
    m_bSendAhead = true;

    return;
}

// =====以下为公共函数列表，各个继承类共同调用的函数==================================================================
string CMsgOptBase::GetModRepKey()
{
    stringstream oss;
    int iModId = GetModuleId();
    string strChannelId = GetChannelId();

    switch (iModId)
    {
    case E_MODID_DOKI_UPDATE_RECOMMEND:   // 为你推荐doki动态
        oss << "doki_channel_recommend_module_" << strChannelId;
        break;

    case E_MODID_DOKI_UPDATE_USR_FOLLOW:  // 关注DOKI动态
        oss << "doki_channel_follow_module_" << strChannelId;
        break;
    }

    return oss.str();
}

string CMsgOptBase::GetTitleRepKey()
{
    stringstream oss;
    int iModId = GetModuleId();
    string strChannelId = GetChannelId();

    switch (iModId)
    {
    case E_MODID_DAIYLY_REPORT:           // 日报
        oss << "doki_channel_journal_title_" << strChannelId;
        break;

    case E_MODID_DOKI_UPDATE_USR_FOLLOW:  // 关注DOKI动态
        oss << "doki_channel_follow_title_" << strChannelId;
        break;

    case E_MODID_DOKI_UPDATE_RECOMMEND:   // 为你推荐doki动态
        oss << "doki_channel_recommend_title_" << strChannelId;
        break;

    case E_MODID_DOKI_PARTY:              // DOKI大爬梯
        oss << "doki_channel_sideslip_title_" << strChannelId;
        break;
    }

    return oss.str();
}

string CMsgOptBase::GetItemRepKey()
{
    stringstream oss;
    int iModId = GetModuleId();
    string strChannelId = GetChannelId();

    switch (iModId)
    {
    case E_MODID_DAIYLY_REPORT:           // 日报
        oss << "doki_channel_journal_item_" << strChannelId;
        break;

    case E_MODID_DOKI_UPDATE_USR_FOLLOW:  // 关注DOKI动态
        oss << "doki_channel_follow_item_" << strChannelId;
        break;

    case E_MODID_DOKI_UPDATE_RECOMMEND:   // 为你推荐doki动态
        oss << "doki_channel_recommend_item_" << strChannelId;
        break;

    case E_MODID_DOKI_PARTY:              // DOKI大爬梯
        oss << "doki_channel_sideslip_item_" << strChannelId;
        break;
    }

    return oss.str();
}

string CMsgOptBase::GetReportParam()
{
    string strRepKey;
    stringstream oss;

    int iModId = GetModuleId();
    string strChannelId = GetChannelId();

    int iBucketId = m_stVideoPacket.getQUAInfo().extentData.bucketInfo.bucketId;
    string strExtRepParam = GetExtReportParam();

    oss << "ztid=" << strChannelId;
    oss << "&plat_bucketid=" << iBucketId;

    if (!strExtRepParam.empty())
    oss << "&" << strExtRepParam;

    return oss.str();
}

int CMsgOptBase::GetModuleId()
{
    return 0;
}

string CMsgOptBase::GetChannelId()
{
    return "100224";
}

string CMsgOptBase::GetExtReportParam()
{
    return "";
}

int CMsgOptBase::ReportBoss()
{
    if (m_stBossData.lBossId > 0)
    {
        m_stBossData.lCmd = m_uiCmd;
        m_stBossData.lError = m_iError;

        m_stBossData.strGuid = m_stVideoPacket.getVideoCommHeader().AccessInfo.Guid;
        m_stBossData.strOmgid = m_stVideoPacket.getVideoCommHeader().AccessInfo.QUAInfo.omgId;

        MyUtils::CUserLoginInfo::GetQqVuidOpenidFVPacket(m_stBossData.lQQ, m_stBossData.lVuid,
            m_stBossData.strOpenid, m_stVideoPacket);

        static char formatBuff[4096] = { '\0' }; //数据库扩展字段最大长度
        snprintf(formatBuff, sizeof formatBuff - 1, "[platform=%d] [version=%s] [appid=%ld] [kv=[%s]]",
            m_stVideoPacket.getQUAInfo().platform,
            m_stVideoPacket.getQUAInfo().versionName.c_str(),
            m_stVideoPacket.getCallerID(),
            m_stBossData.strKvData.c_str());

        BossAPI::logprintf(m_stBossData.lBossId, "%d,%s,%s,%s,%ld,%ld,%d,%s,%s,%s",
            m_stBossData.lCmd, 
            m_stBossData.strGuid.c_str(), m_stBossData.strOmgid.c_str(), m_stBossData.strOpenid.c_str(),
            m_stBossData.lQQ, m_stBossData.lVuid, 
            m_stBossData.lError,
            formatBuff,
            m_stBossData.strFilter1.c_str(),
            m_stBossData.strFilter2.c_str());
    }

    return 0;
}

void CMsgOptBase::AppendBossKVData(const string& data)
{
    static char szBuffer[4096] = { '\0' }; //数据库扩展字段最大长度

    // 请求参数
    snprintf(szBuffer, sizeof(szBuffer)-1, "(%s)", data.c_str());
    m_stBossData.strKvData += szBuffer;
}

void CMsgOptBase::AppendSpaceLine(vector<jce::TempletLine>& vecLines, int iHeight)
{
    static long long groupId = CTimer::GetCurrentTime_us();
    taf::JceOutputStream<taf::BufferWriter> stJceWriter;

    if (++groupId <= 0)
    {
        groupId = CTimer::GetCurrentTime_us();
    }

    jce::ONASplitSpace stSplitlineOnaView;
    stSplitlineOnaView.color = "";
    stSplitlineOnaView.height = iHeight;
    stSplitlineOnaView.writeTo(stJceWriter);

    jce::TempletLine jceTempletline;
    jceTempletline.lineId = "spaceline_" + CNum2Str::Num2Str(groupId);
    jceTempletline.groupId = jceTempletline.lineId;

    jceTempletline.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    jceTempletline.item.itemType = jce::EnumONASplitSpace;
    vecLines.push_back(jceTempletline);
}


void CMsgOptBase::InsertSpaceLine(vector<jce::TempletLine>& vecLines, int iHeight)
{
    taf::JceOutputStream<taf::BufferWriter> stJceWriter;

    jce::ONASplitSpace stSplitlineOnaView;
    stSplitlineOnaView.color = "";
    stSplitlineOnaView.height = iHeight;
    stSplitlineOnaView.writeTo(stJceWriter);

    jce::TempletLine jceTempletline;
    jceTempletline.lineId = "spaceline";
    jceTempletline.groupId = "spaceline";

    jceTempletline.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    jceTempletline.item.itemType = jce::EnumONASplitSpace;
    vecLines.insert(vecLines.begin(), jceTempletline);
}

int CMsgOptBase::ParseDatakey(const string& strParam, StDatakey& stKey)
{
    Json::Value jsValue;
    MyUtils::CStrUnits::SplitToJson(strParam, jsValue);

    if (m_uiCmd == CMD_CHANNEL)
    {
        if (jsValue.isMember("pagenum"))
        {
            stKey.pagenum = jsValue["pagenum"].isString() ? atoi(jsValue["pagenum"].asString().c_str()) : 0;
        }

        if (stKey.pagenum < 0)
            stKey.pagenum = 0;

        SPP_MSG_TRACE_LOG("pagesize: %d, pagenum: %d", stKey.pagesize, stKey.pagenum);
    }
    else if (m_uiCmd == CMD_SECOND_PAGE)
    {
        // 翻页参数
        if (jsValue.isMember("pagenum"))
        {
            stKey.pagenum = jsValue["pagenum"].isString() ? atoi(jsValue["pagenum"].asString().c_str()) : 0;
        }

        if (stKey.pagenum < 0)
            stKey.pagenum = 0;

        // 模块请求参数
        if (jsValue.isMember("modid"))
        {
            stKey.modid = jsValue["modid"].isString() ? atoi(jsValue["modid"].asString().c_str()) : 0;
        }

        if (stKey.modid < 0)
            stKey.modid = 0;

        if (jsValue.isMember("cmsid"))
        {
            stKey.strCMSId = jsValue["cmsid"].isString() ? jsValue["cmsid"].asString().c_str() : "";
        }

        // 扩展参数 -- 只用于广场B样式ugc饭拍视频模块
        // extid=feedid 表示读取老样式feed流，并把feedid放在第一个feed下发；用于给app支持视频沉浸式播放和左右横滑下一个视频
        // extid=空     表示点击饭拍视频模块进入模块二级页，出卡片化的自拍视频feed流（658定义结构体）。空为默认值。
        if (jsValue.isMember("extid"))
        {
            stKey.extid = jsValue["extid"].isString() ? (jsValue["extid"].asString()) : "";
        }

        // 从A/B哪个视图进入二级页
        if (jsValue.isMember("fromview"))
        {
            stKey.fromView = jsValue["fromview"].isString() ? atoi(jsValue["fromview"].asString().c_str()) : 0;
        }

        SPP_MSG_TRACE_LOG("pagenum: %d, modid: %d, cmsid[%s], extid[%s]", 
            stKey.pagenum, stKey.modid, stKey.strCMSId.c_str(), stKey.extid.c_str());
    }

    return 0;
}

void CMsgOptBase::AppendPluginLine(
    const string& strChannel, long lPluginId, const string& strDatakey, 
    const string& strGroupId, vector<jce::TempletLine> &vecData)
{
     taf::JceOutputStream<taf::BufferWriter> stJceWriter;

    jce::TempletLine stLine;
    jce::ServerTempletItem filterTemplet;
    jce::ChannelModPluginDataRequest pluginRequest;

    pluginRequest.scence = 1;
    pluginRequest.channelItemId = strChannel;

    //pluginRequest.modId = stGroupInfo.iZTModIdx;
    pluginRequest.position = strDatakey;
    filterTemplet.pluginRequest = pluginRequest;
    filterTemplet.route = CNum2Str::Num2Str(lPluginId); //插件ID

    stLine.item.itemType = (jce::EONAViewType)jce::EnumServerONAPlugin;
    stLine.groupId = strGroupId;
    stLine.lineId = strGroupId;
    stLine.increaseId = 0;

    filterTemplet.writeTo(stJceWriter);
    stLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));

    vecData.push_back(stLine);
    SPP_MSG_DEBUG_LOG("append plugin. channel[%s] pluginId[%d] groupid[%s]", strChannel.c_str(), lPluginId, strGroupId.c_str());
}

void CMsgOptBase::AppendTitle(vector<jce::TempletLine>& vecLines, 
    const string& strTitle, const string& strGroupId)
{
    if (strTitle.empty())
    {
        SPP_MSG_ERROR_LOG("empty title");
        return;
    }

    // 构造标题onaview
    //-----------打包标题----------------------
    jce::ONAPosterTitle stOnaContainer;

    stOnaContainer.title = strTitle;
    stOnaContainer.style = 4;
    //stOnaContainer.reportKey = GetTitleRepKey();

    taf::JceOutputStream<taf::BufferWriter> stJceWriter;
    stOnaContainer.writeTo(stJceWriter);

    jce::TempletLine stTitleLine;
    stTitleLine.item.itemType = jce::EnumONAPosterTitle;
    stTitleLine.groupId = strGroupId;
    stTitleLine.increaseId = 0;
    stTitleLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLines.push_back(stTitleLine);
}

void CMsgOptBase::AppendMore(vector<jce::TempletLine>& vecLines, 
    const string& strTitle, const string& strGroupId,
    const string& actionUrl, const string& reportKey, const string& reportParam)
{
    if (strTitle.empty())
    {
        SPP_MSG_ERROR_LOG("empty title");
        return;
    }

    AppendSpaceLine(vecLines, 16);

    // 构造标题onaview
    //-----------打包标题----------------------
    jce::ONADokiMoreInfoBar stOnaContainer;

    stOnaContainer.text = strTitle;
    stOnaContainer.reportKey = reportKey;
    stOnaContainer.reportParams = reportParam;
    stOnaContainer.action.url = actionUrl;

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