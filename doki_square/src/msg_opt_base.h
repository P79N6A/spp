/*
 *  Filename: msg_opt_base.h
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

#ifndef __MSG_OPT_BASE_H__
#define __MSG_OPT_BASE_H__

//视频通用公共库
#include "video_packet.h"	//通用视频包
#include "cs_packet2.h"		//通用视频中转包
#include "config.h"         //配置文件读取库

//封装的spp通用task操作
#include "task_operate_jcedata.h"
#include "task_operate_uniondata.h"
#include "task_operate_redis.h"
#include "task_exec.h"

//工具函数
#include "myUtils.h"
#include "myUtilsTVideo.h"
using namespace MyUtils;

//项目基本头文件
#include "task_commdef_proj.h"
#include "commdef.h"

//第三方库函数
#include "UnifiedConfigAPI.h"       //配置中心api
#include "comm_cache_api.h"         //cache
#include "unified_config_wuji.h"    //配置中心api（101配置）

//jce协议文件
#include "qqvideo_message.h"
#include "qqvideo_interface.h"
#include "snowball.h"               //血球中转固定回包格式
#include "qqvideo_doki.h"


//全局变量的前置声明
extern StConfInfo          g_stConf;
extern UnifiedConfigAPI    g_confApi;  //配置中心--明星聊天室cid对应的pid信息
extern vapi::CommCacheApi  g_cacheApi; //共享内存cache

extern UnifiedConfigAPI    g_confApiAll;            //配置中心--明星聊天室cid对应的pid、明星id信息（取最新两周的全部的数据）
extern int64_t             g_confApiAllUpdateTime;  //配置中心--配置更新时间
extern map<string,string>  g_mapStaridToPid;        //从配置中心数据中获取的明星id到直播pid的映射（只需要直播前和直播中的数据，只获取最近2周的新配置）

extern UnifiedConfigWuji    g_conf101Global;            //配置中心--101全局配置
extern UnifiedConfigWuji    g_conf101Card;              //配置中心--101卡片配置
extern UnifiedConfigWuji    g_confComeOne;              //配置中心--101卡片配置

//全局变量的前置声明--自定义框架的全局变量，存储接口连接和监控信息
extern CManagerSvrConn     g_oMgSvrConn; //下层接口连接信息
extern CManagerMcall       g_oMgMcall;   //监控信息--模调
extern CManagerMoni        g_oMgMoni;    //监控信息--monitor

class CMsgOptBase : public CSyncMsg
{
public:
    CMsgOptBase()
    {
        m_iError     = 0;
        m_uiCmd      = 0;
        m_bSendAhead = false;
        m_iErrInner  = 0;
        
    };

    virtual ~CMsgOptBase() {}; //基类构造函数设置为虚函数

    /**
     * @brief  HandleProcess 同步消息处理函数
     * @note   spp框架执行的主体函数
     * @retval 0, 成功-用户自己回包到前端,框架不负责回包处理; 其它, 失败-框架关闭与proxy连接, 但不负责回业务报文
     */
    virtual int  HandleProcess();

    // ====继承CMsgOptBase的类需要实现以下几个虚函数，定义各自的设置请求数据、解包校验操作、处理操作、回包操作================
    virtual int  SetMsgReqData(char *pData, uint32_t uiLen) //将框架中的请求数据写入到msg中来，0==>成功  非零==>失败
    {
        return -1;
    }

    virtual int  EncodeAndCheckReqData() //解析和校验请求数据，0==>成功  非零==>失败
    {
        return -1;
    }

    virtual void SendRspData() //发送回包数据
    {
        return;
    }

    virtual int  DealProcess() //处理请求操作过程，0==>成功  非零==>失败
    {
        return -1;
    }

    //设置错误码的函数，对外提供设置接口
    void SetErrCode(int iError)
    {
        m_iError = iError;
        return;
    }

public:
    // ====公共回包函数，供继承CMsgOptBase的类使用=======================================================================
    template <typename T_rspJce>
    void SendRspDataVPacket(T_rspJce &jceRsp, CVideoPacket &oVideoPacket); //回包函数--回复videopacket包
    void SendRspDataCommBuf(char *pData, uint32_t uiLen); 				   //回包函数--回复通用buf数据，例如拼接好的http包
    void SendRspDataEmpty();											   //回包函数--回复空包
    void SendRspDataSnowball(int iError, const string& strMsg, CVideoPacket &oVideoPacket);  //回包函数--雪球中转固定格式回包

public:
    // =====以下为公共函数列表，各个继承类共同调用的函数===================================================================
    virtual int  GetModuleId();           // 模块ID函数--各个模块各自实现--用于ReportKey和ReportParam
    virtual string GetChannelId();        // 频道ID--各个模块选择性实现--默认100224
    virtual string GetExtReportParam();   // 扩展参数上报--各个模块选择实现--推荐模块需上报algid\bucketid

    // boss流水函数
    // 1\ 设置boss id
    // 2\ 设置lError、lCmd、strKvData
    // 3\ 派生类若要上报流水，则需要重载以下函数，并在函数结尾调用父类ReportBoss!
    //    派生类若要上报流水，则需要重载以下函数，并在函数结尾调用父类ReportBoss!
    //    派生类若要上报流水，则需要重载以下函数，并在函数结尾调用父类ReportBoss!
    virtual int ReportBoss();             // 根据m_stBossData.lBossId判断是否上报     -- bossId未设置则不上报 -- 可派生类实现
    void AppendBossKVData(const string& data);

    // 上报函数  -- 模块包括title、Item
    virtual string GetModRepKey();
    virtual string GetTitleRepKey();
    virtual string GetItemRepKey();
    virtual string GetReportParam();     // 包含公共上报参数

public:
    // =====onaview 打包函数==============================================
    void AppendSpaceLine(vector<jce::TempletLine>& vecLines, int iHeight);  // 添加空行 -- 队尾
    void InsertSpaceLine(vector<jce::TempletLine>& vecLines,  int iHeight);  // 添加空行 -- 队首

    // 注意：终端存在去重逻辑， 某些场景譬如帖子，必须传入可用于去重的ID
    template<typename T_onaJce>
    void AppendTempleteLine(vector<jce::TempletLine>& vecLines, T_onaJce& jceOnaview, int iOnaType, const string& strGroupId = "", const string& strLineId = "");

    // 添加插件
    void AppendPluginLine(
        const string& strChannel, long lPluginId, 
        const string& strDatakey, const string& strGroupId, 
        vector<jce::TempletLine> &vecData);

    // 添加标题
    void AppendTitle(vector<jce::TempletLine>& vecLines,   
        const string& strTitle, const string& strGroupId);

    // 查看更多
    void AppendMore(vector<jce::TempletLine>& vecLines,  
        const string& strTitle, const string& strGroupId, 
        const string& actionUrl = "", const string& reportKey ="", const string& reportParam="");

public:
    // 工具函数
    int ParseDatakey(const string& strParam, StDatakey& stKey);      // strParam格式：key1=val1&key2=val2


protected: //供继承类直接访问
    int 	 m_iError;     //存储统一错误码
    int      m_iErrInner;// 内部错误码 

    uint32_t m_uiCmd;	   //请求命令字
    bool     m_bSendAhead; //是否已经提前回包了
    string   m_strReqKey;  //请求的key，可以是用户id、设备信息等

    StBossData   m_stBossData;      //boss流水数据
    CVideoPacket m_stVideoPacket;	//视频统一请求包VideoPacket结构
};


// ====模板函数的实现===================================================================================================
template <typename T_rspJce>
void CMsgOptBase::SendRspDataVPacket(T_rspJce &jceRsp, CVideoPacket &oVideoPacket)
{
    if (m_bSendAhead)//已经提前回包了，直接返回
    {
        SPP_MSG_TRACE_LOG("sendRspData: has already sendData");
        return;
    }

    blob_type blob;
    blob.data = NULL;
    blob.len = 0;

    try
    {
        taf::JceOutputStream<taf::BufferWriter> osJce;
        jceRsp.writeTo(osJce);

        oVideoPacket.setResult(m_iError);
        oVideoPacket.setBody(osJce.getBuffer(), osJce.getLength()); //函数限制了最大的传递buf为2M
        oVideoPacket.encode();
    }
    catch (exception &e)
    {
        Attr_API(JCE_EN_ERR, 1);
        SPP_MSG_ERROR_LOG("sendRspData: SetVPacketError, jceDecodeError,errmsg[%s]", e.what());

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

template<typename T_onaJce>
void CMsgOptBase::AppendTempleteLine(vector<jce::TempletLine>& vecLines, T_onaJce& jceOnaview, int iOnaType, const string& strGroupId, const string& strLineId)
{
    taf::JceOutputStream<taf::BufferWriter> stJceWriter;
    jceOnaview.writeTo(stJceWriter);

    jce::TempletLine stTempletLine;

    stTempletLine.item.itemType = jce::EONAViewType(iOnaType);
    stTempletLine.groupId = strGroupId;
    stTempletLine.lineId = strLineId;
    stTempletLine.increaseId = 0;

    stTempletLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLines.push_back(stTempletLine);
}


#endif // __MSG_OPT_BASE_H__
