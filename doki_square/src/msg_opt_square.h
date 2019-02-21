/*
 *  Filename: msg_opt_doki.h
 *
 *  Created on: 2018-09-06
 *      Author: beepeng   beepeng@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 加入DOKI弹窗
 *
 */

#ifndef __MSG_OPT_DOKI_H__
#define __MSG_OPT_DOKI_H__

#include "msg_opt_base.h"

//jce协议文件
#include "qqvideo_interface.h"
#include "qqvideo_fantuan.h"
#include "star_timeline.h"
#include "starCard.h"
#include "qqvideo_live.h"
#include "newsapp_DM.h"
#include "ft_read.h"
#include "rec_feed.h"
#include "rec_interface_uni.h"
#include "star_admin.h"
#include "config_admin.h"
#include "doki_api_extend.h"
#include "doki_api.h"
#include "fans_growth_h5.h"
#include "star_rank.h"
#include "userinfo.h"
#include "welfare.h"
#include "bucket_api.h"
#include "qqvideo_doki.h"
#include "qqvideo_live.h"
#include "ad_promotion_issue_jce.h"
#include "ad_promotion_comm_enum.h"


// DOKI弹窗欢迎界面
class CMsgOptSquare : public CMsgOptBase
{
  public:
    CMsgOptSquare()
    {
        m_iPlatform = 0;
        m_bLoginUser = true;
        m_lVuid = 0;
        m_bIsPhone = false;
        m_bIsVerGe658 = false;
        m_bIsVerGe668 = false;
        m_iPolicyId = 0;
        m_iBucketId = 0;

        m_jceSecondPageRsp.hasNextPage = true;
        m_jceChannelRsp.hasNextPage = true;
    };

    ~CMsgOptSquare() {};

    // ====继承类需要实现以下几个虚函数，定义各自的设置请求数据、解包校验操作、处理操作、回包操作=========================
    virtual int  SetMsgReqData(char *pData, uint32_t uiLen); //将框架中的请求数据写入到msg中来，0==>成功  非零==>失败
    virtual int  EncodeAndCheckReqData();					 //解析和校验请求数据，0==>成功  非零==>失败
    virtual void SendRspData();								 //发送回包数据
    virtual int  DealProcess();								 //处理请求操作过程，0==>成功  非零==>失败

    // =====业务逻辑处理相关函数=====================================================================================
    int  HandleChannelPage();                                    //关注列表
    int  HandleSecondPage();                                 //关注列表 -- APP接口 -- 包括头像、昵称和跳转链接

public: 
    // 流程函数 -- 
    int  GetRecommend(const UniRecInterface::RecReq& stReq, UniRecInterface::RecRsp& stRsp);
    int  GetDokiInfo(vector<string>& vecDokiid, map<string, StStarInfo>& mpStarInfo);               //doki信息
    int  GetFeedInfo(const vector<string>& vecFeedid, map<string, jce::CirclePrimaryFeed>& mpFeedInfo);   //feed信息
    int  GetWelfareData(const WelfareClubInner::StWelfareNewDataReq& stReq, WelfareClubInner::StWelfareNewDataRsp& stRsp); // 获取福利和表情包
    int  GetFocusPicture(); //获取焦点图数据

    //   频道页回包数据
    int  Channel_PrepareData_1();     //如有必要在此执行各类并发请求
    int  Channel_PrepareData_2();
    int  Channel_PrepareData_3();
    int  ConstructChannelData();      //回包

    //   模块二级页 -- 单机模块跳转进入
    int  SecondPage_PrepareData_1();  //如有必要在此执行各类并发请求
    int  ConstructSecondPage();       //回包

    //   构建模块数据
    template<typename t_extrData>
    int  ConstructModule(EModuleId eModuleId, const UniRecStruct::RecModule& stRecModule, 
        const t_extrData& stExtrInfo, vector<jce::TempletLine>& vecLines);

public:
    // 构建视图函数 --
    int  CreateOnaFocPic(const vector<StCmsItem> &vecFocusPic, vector<jce::TempletLine>& vecLineData);
    int  CreateOnaDailyNews(vector<jce::TempletLine>& vecLineData);
    int  CreateOnaRankChain(vector<jce::TempletLine>& vecLineData); //星耀榜系列
    int  CreateOnaHotTopic(vector<jce::TempletLine>& vecLineData);
    int  CreateOnaTopten(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);    //十佳
    int  CreateOnaRenqiChangePlugin(vector<jce::TempletLine>& vecLineData); //榜单播报

    //   明星说样式
    int  CreateOnaStarTalk(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);

    //   明星说样式 - 明星驾到
    int  CreateOnaStarTalkPid(
        UniRecStruct::RecModule& stRecModule, const StStarTalkFeed& stPidFeed, vector<jce::TempletLine>& vecLineData);

    //   明星说样式 - 明星说普通feed
    int  CreateOnaStarTalkFeed( 
        UniRecStruct::RecModule& stRecModule, const StStarTalkFeed& stPidFeed, vector<jce::TempletLine>& vecLineData);

    //   明星说样式 - DOKI列表 -- A样式
    int  CreateOnaStarTalkAView(
        UniRecStruct::RecModule& stRecModule, const StStarTalkFeed& stPidFeed, vector<jce::TempletLine>& vecLineData);

    //   普通feed样式
    int  CreateOnaFeed(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);

    //   爱豆饭制、饭拍视频
    int  CreateOnaVideo(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);
    int  CreateOnaVideoAView(UniRecStruct::RecModule& stRecModule, const vector<string>& vecFeedid, vector<jce::TempletLine>& vecLineData);
    int  CreateOnaVideoBView(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData, bool bNewFeedView=true);

    //   饭拍视频 -- 二级页有两个形态
    //   在广场页读取，依照普通帖子feed返回
    //   在二级页读取，依照ONADokiFansVideoCard返回
    int  CreateOnaUGCSecondPage(UniRecStruct::RecModule& stRecModule, const string& strFeedid, vector<jce::TempletLine>& vecLineData);

    //   壁纸、表情包
    int  CreateOnaWelfare(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);
    
    //   A样式 -- 聚合 -- 美图、表情包共用
    int  CreateOnaWallPaperAView(  UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);
    
    //   B样式 -- 美图
    int  CreateOnaWallPaperBView(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);

    //   B样式 -- 表情包
    int  CreateOnaEmoticonBView(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);

    //   娱乐资讯
    int  CreateOnaEntNews(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);

    //   A样式 -- 娱乐资讯、IP聚合模块共用
    int  CreateOnaEntNewsAndIPGroupAView(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);

    //   B样式 -- 娱乐资讯
    int  CreateOnaEntNewsBView(UniRecStruct::RecModule& stRecModule, 
        const string& strFeedid, vector<jce::TempletLine>& vecLineData);

    //   IP聚合模块 -- 
    int  CreateOnaIPGroup(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);

    //   B样式 -- IP聚合模块 -- 二级页也用此函数
    //   推荐接口留下的坑：
    //       推荐返回多个IP聚合模块（即多个rec_module结构体），
    //       由广场服务将同个请求的多个IP聚合模块下发成一个groupID返给APP
    int  CreateOnaIPGroupBView(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);
    int  CreateOnaIPGroupSecondPage(UniRecStruct::RecModule& stRecModule, vector<jce::TempletLine>& vecLineData);


    //   设置数据上报
    template <typename T_OnaView, typename T_SrcInfo>
    int  SetReportParam(EModuleId eModid, T_OnaView& stContainer, T_SrcInfo& stInfo);

    void AppendMore(vector<jce::TempletLine>& vecLines,
        const string& strTitle, const string& strGroupId,
        const string& actionUrl, const UniRecStruct::RecModule& stRecModule);

public:
    // 数据上报函数 --
    template <typename T_OnaView>
    int  SetReportParamViewA(EModuleId eModid, T_OnaView& stContainer, const string& strExtParam = "");

     template <typename T_OnaView>
    int  SetReportParamViewA_WithoutCardBase(EModuleId eModid, T_OnaView& stContainer, const string& strExtParam = "");

    template <typename T_OnaView>
    int  SetReportParamViewB(EModuleId eModid, T_OnaView& stContainer, const string& strExtParam = "");

    template <typename T_OnaView>
    int  SetReportParamViewB_WithoutCardBase(EModuleId eModid, T_OnaView& stContainer, const string& strExtParam = "");

    // 娱乐资讯二级页模块
    int  SetSecondPageReportParam_IPGroup(jce::ONADokiCommonFeedCard& stContainer, const string& strExtParam = "");


public: //工具函
    void SetDokiItem(const StStarInfo&, jce::DokiInfo&);
    void ConvertRecType(UniRecStruct::RecModule& stRecModule); //将推荐module_type字段转为内部识别的EModuleId
    void ProcessRecData(UniRecInterface::RecRsp& stRsp);

    bool IsSquareViewA() const;       //双feed瀑布流
    bool IsSquareViewB() const;       //固定模块+卡片feed+【推荐TAB】
    bool IsSquareViewC() const;       //外网现有样式 -- 不会出现本视图，因为doki_square_rec服务已过滤
    bool IsSecondPageView() const;    //二级页视图
    int  GetSquareViewId();           //view id即样式策略ID
    bool IsSquareFirstPage() const;   //广场页首页请求

    int  ConstructReqPack(UniRecInterface::RecReq& stReq);
    // 读取推荐模块，并新增到列表尾部
    void StripRecDataAppend(UniRecStruct::RecModule& stRecModule);
    void StripRecDataAppend(const UniRecStruct::RecModule& stRecModule, vector<EModuleId>& vecUnfinished);

    // 主要是读取leafid，用于读取模块的详细信息
    int GetCmsModuleRedisKey(vector<string>& vecModItemKey);

    // 读取key的详细信息
    int GetCmsModuleConfDetail(const vector<string>& vecKeys, vector<StCmsItem>& vecItems);

    // 二级页推荐请求参数
    int SetSecondPageRecReqParam(const StDatakey& stDataKey, UniRecInterface::RecReq& stReq);

public:
    // 工具函数
    bool IsVideoFeed(const jce::CirclePrimaryFeed& stFeedInfo, string &strPic);       //是否视频贴
    bool IsVideoFeed(const jce::CirclePrimaryFeed& stFeedInfo);                       //是否视频贴
    void GetVideoInfo(const jce::CirclePrimaryFeed& stFeedInfo, StVideoInfo& stInfo); 
    void SetVideoCover(const string& strVid, jce::CircleMsgImageUrl& stImage, jce::MarkLabel& stMarkLabel);

    bool IsImageFeed(const jce::CirclePrimaryFeed& stFeedInfo, string &strPic);       //是否图片贴
    bool IsImageFeed(const jce::CirclePrimaryFeed& stFeedInfo);       //是否图片贴
    void GetImageFaceArea(const jce::CirclePrimaryFeed& stFeedInfo, const string &strPic, jce::ImgFaceArea& area);

    bool GetFeedImage(const jce::CirclePrimaryFeed& stFeedInfo, jce::CircleMsgImageUrl &stImg);
    bool GetToptenFeedImage(const jce::CirclePrimaryFeed& stFeedInfo, const CStarAdminPro::GoodFeedInfo& stFeedCfg, jce::CircleMsgImageUrl &stImg);
    string GetCommentKey(const StNewPidInfo &stPidInfo);

    template<typename T>
    void ResizeLines(int eModId, vector<T>& vecLineData);

    string GetCMSTitle(UniRecStruct::RecModule& stRecModule);
    string GetCMSSubTitle(UniRecStruct::RecModule& stRecModule);
    string GetCMSPoster(UniRecStruct::RecModule& stRecModule, EPicSize eSize = ESIZE_456_810);
    string GetModGroupId(int eModId, const string& strFeedid="");

    void FillFeedCard(const jce::CirclePrimaryFeed& stFeedInfo, jce::DokiCardBaseInfo& stFeedCard);
    void FillVideoCard(UniRecStruct::RecItem& stRecItem, jce::DokiCardBaseInfo& stFeedCard);

    void GetFeedVoice(const jce::CirclePrimaryFeed& stFeedInfo, jce::ApolloVoiceData& stVoice);
    string GetSecondPageDataKey(UniRecStruct::RecModule& stRecModule);
    
    string GetModuleName(int iModid);
    string GetViewName(int iPolicyId);

    bool GetWallPaperImg(const jce::DokiWallPaperItem& stItem, string& strImage);
    bool GetEmoticonImg(const jce::ONADokiEmoticonItem& stItem, string& strImage);
    void AppendFeed(const UniRecStruct::RecModule& stRecModule, vector<string>& vecFeedid);

    bool IsRecItemFeed(UniRecStruct::RecItem& stRecItem); // 是否feed推荐
    bool IsRecItemCidVideo(UniRecStruct::RecItem& stRecItem); 
    bool IsRecItemVidVideo(UniRecStruct::RecItem& stRecItem); 
    bool IsRecItemPid(UniRecStruct::RecItem& stRecItem); 

    // 帖子推荐，则取dokiid，pid推荐则同时存入pid与dokiid的映射关系
    void AppendPidDoki(const UniRecStruct::RecModule& stRecModule, 
        vector<string>& vecDokiid, vector<string>& vecPid, map<string, vector<string> >& mpPidToDokiid);

    // 取配置视频数据
    void AppendVideo(UniRecStruct::RecModule& stRecModule, vector<string>& vecCid, vector<string>& vecVid);

    // 增加推荐上报参数
    void AppendRecParam(UniRecStruct::RecModule& stRecModule, string& strExt);
    void AppendRecParam(UniRecStruct::RecItem& stRecItem, string& strExt);

public:   // 父类函数
    int ReportBoss();

private:
    int                                  m_iPlatform;
    bool                                 m_bLoginUser;          // 是否登录用户
    long                                 m_lVuid;

    // 频道页请求
    jce::ChannelDataRequest              m_jceChannelReq;       // 频道请求
    jce::ChannelDataResponse             m_jceChannelRsp;       //

    jce::DokiGroupDataPageRequest        m_jceSecondPageReq;        // 模块二级页请求
    jce::DokiGroupDataPageResponse       m_jceSecondPageRsp;        // 模块二级页响应

    // 基本数据
    int                                  m_iBucketId;           // 当前用户所属的桶 -- APP携带上来
    int                                  m_iPolicyId;           // 当前用户所属策略
    string                               m_strChannelId;
    bool                                 m_bIsPhone;            // 是否Phone端请求
    bool                                 m_bIsVerGe658;         // 版本是否>=658
    bool                                 m_bIsVerGe668;         // 版本是否>=668
    StDatakey                            m_stDatakey;           // 翻页参数

    // 中间数据
    // 注意：取到推荐数据后，立即处理将module_type转为
    // 内部定义的EModuleId类型
    UniRecInterface::RecRsp              m_stRecRsp;            // 推荐响应包  

    vector<string>                       m_vecDokiid;           // 如果需要读取dokiid信息，则保存在这里
    vector<string>                       m_vecWallPaper;        // 如果需要读取dokiid信息，则保存在这里
    vector<string>                       m_vecEmoticon;         // 如果需要读取dokiid信息，则保存在这里
    vector<string>                       m_vecFeedid;           // 如果需要读取feedid信息，则保存在这里  
    vector<string>                       m_vecPid;              // 如果需要读取pid信息，则保存在此处
    map<string, int>                     m_mpFeedidToModid;     // 用于透传给帖子服务
    vector<EModuleId>                    m_vecMidMoreWork;      // 需要额外读取数据 进行处理的模块
    vector<UniRecStruct::RecModule>      m_vecIPGroup;          // IP宫格模块信息，B样式数据额外存储；处理完后清除结构体   

    vector<string>                       m_vecVid;              //   
    vector<string>                       m_vecCid;              // 
    map<string, StVideoInfo>             m_mpVidToDetail;       //
 
    map<string, StStarInfo>              m_mpDokiidToDetail;      // 
    map<string, jce::CirclePrimaryFeed>  m_mpFeedidToDetail;      // feed详情信息
    //map<string, StNewPidInfo>            m_mpDokiidToPidInfo;   // 明星直播信息
    map<string, StNewPidInfo>            m_mpPidToPidInfo;      // 直播信息
    map<string, vector<string> >         m_mpPidToDokiid;       // 直播id到dokiid - 直播可配置多个明星 
    
    // 表情包数据和壁纸
    map<string, jce::ONADokiWallPaperItem> m_mpWallPaper;
    map<string, jce::ONADokiEmoticonItem>  m_mpEmoticon; 

    // 十佳帖数据
    CStarAdminPro::SquareGoodFeedsResp    m_stToptenRsp;

    // PR管理台配置数据
    AdvertisingPromotion::DokiPageAd      m_stPRConfig;


    // 中间数据
    // 1、标题对应于cms的ZT_leaf_head字段
    //    标题是否隐藏CMS没有字段标识，ZT_leaf_head字段不存在或为空都表示隐藏模块标题
    // 2、模块有效性判断 -- CMS下发的模块默认都有效
    string m_strModTitle;                 //模块标题 -- 
    string m_strDailyHeadUrl;             //图片URL
    vector<StCmsItem>                     m_vecFocusPic;

    // cms存储格式
    // 1\ 读取模块列表信息，包含模块key信息             
    //    get 944_100224_information
    // 2\ 根据模块key信息（leaf id）读取模块具体信息 
    //    get 944_ztid_100224_leafid_20171205015310_p_7_ver_37001 -- iphone
    //    get 944_ztid_100224_leafid_20171205015310_p_8_ver_37001 -- aphone
    vector<string>                       m_vecModItemKey;

    // 扩展上报参数
    stringstream                         m_oss;
};


template<typename t_extrData>
int CMsgOptSquare::ConstructModule(EModuleId eModuleId, const UniRecStruct::RecModule& stRecModule,
    const t_extrData& stExtrInfo, vector<jce::TempletLine>& vecLines)
{
    /*stJceWriter.reset();
    jceOnaview.writeTo(stJceWriter);

    jce::TempletLine stTempletLine;

    stTempletLine.item.itemType = EONAViewType(iOnaType);
    stTempletLine.groupId = "groupId_" + strId;
    stTempletLine.increaseId = 0;

    stTempletLine.item.data.assign((char*)stJceWriter.getBuffer(), (char*)(((char*)stJceWriter.getBuffer()) + stJceWriter.getLength()));
    vecLines.push_back(stTempletLine);*/
}


template<typename T>
void CMsgOptSquare::ResizeLines(int eModId, vector<T>& vecLineData)
{
    int iCount = vecLineData.size();

    // A样式保持不变
    if (IsSquareViewB())
    {
        switch (eModId)
        {
            case E_MODID_IP_GROUP:
            case E_MODID_PGC_VIDEO:  // 饭制视频展示4个或2个
            case E_MODID_STAR_TALK: 
            {
                // 超过4张、截断
                // 超过2张、截断
                // 不足2张、不出  保证每行2个
                if (vecLineData.size() > 4)
                {
                    vecLineData.resize(4);
                }
                else if (vecLineData.size() == 3)
                {
                    vecLineData.resize(2);
                }
                else if (vecLineData.size() == 1)
                {
                    vecLineData.clear();
                }
            }
            break;

            case E_MODID_UGC_VIDEO: // 饭拍展示2个
            {
                // 超过4张、截断
                // 超过2张、截断
                // 不足2张、不出  保证每行2个
                if (vecLineData.size() > 2)
                {
                    vecLineData.resize(2);
                }
                else if (vecLineData.size() == 1) // 单个不展示
                {
                    vecLineData.clear();
                }
            }
            break;

            case E_MODID_WALLPAPER:  // 表情包和美图
            case E_MODID_EMOTICON: 
            {
                // 壁纸和表情包，均有两个onaview构成
                // 每个onaiew最多三张图
                // 超过3张、截断
                // 不足3张、不出  保证每行3个
                if (vecLineData.size() > 3)
                {
                    vecLineData.resize(3);
                }
                else if (vecLineData.size() < 3)
                {
                    vecLineData.clear();
                }
            }
            break;            

            case E_MODID_ENTERTAINMENT_NEWS: 
            {
                // 超过2张、截断
                // 不足2张、不出  保证每行2个
                if (vecLineData.size() > 2)
                {
                    vecLineData.resize(2);
                }
                else if (vecLineData.size() < 2)
                {
                    vecLineData.clear();
                }
            }
            break;    
        default:
            break;
        }
    }

    SPP_MSG_DEBUG_LOG("%s: resize from[%d] to[%d]", GetModuleName(eModId).c_str(), iCount, vecLineData.size());
}

template <typename T_OnaView>
int  CMsgOptSquare::SetReportParamViewA(EModuleId eModid, T_OnaView& stContainer, const string& strExtParam)
{
    string strParam, strPageId, strStrategy;

    strStrategy = "strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
    strParam = "mod_id=" + CNum2Str::Num2Str(eModid) + "&" + strStrategy;
    strPageId = "&pageId=server_second_page_mod_" + CNum2Str::Num2Str(eModid) + "&strategyId=" + CNum2Str::Num2Str(m_iPolicyId);

    switch (eModid)
    {
    case E_MODID_FEED:
        {
            strParam += "&data_type=button";
            strParam += "&" + strExtParam;

            stContainer.reportEventId = "feed_exposure";
            stContainer.cardInfo.cardAction.reportEventId = "feed_click";

            stContainer.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;
            stContainer.cardInfo.cardAction.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;

            stContainer.reportParams = strParam;
            stContainer.cardInfo.cardAction.reportParams = strParam;
        }
        break;

    case E_MODID_IP_GROUP:     // 宫格聚合
        {
            strParam += "&data_type=button";
            strParam += "&" + strExtParam;   // IP宫格模块，外部传入cms_id=xxx

            stContainer.reportEventId = "common_button_item_exposure";
            stContainer.cardInfo.cardAction.reportEventId = "common_button_item_click";

            stContainer.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;
            stContainer.cardInfo.cardAction.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;

            stContainer.reportParams = strParam;
            stContainer.cardInfo.cardAction.reportParams = strParam; 
            stContainer.cardInfo.cardAction.url += strPageId;        
        }
        break;

    case E_MODID_ENTERTAINMENT_NEWS:
    case E_MODID_TOPTEN:
        {
            strParam += "&data_type=button";
            strParam += "&sub_mod_id=pic";

            stContainer.reportEventId = "common_button_item_exposure";
            stContainer.cardInfo.cardAction.reportEventId = "common_button_item_click";

            stContainer.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;
            stContainer.cardInfo.cardAction.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;

            stContainer.reportParams = strParam;
            stContainer.cardInfo.cardAction.reportParams = strParam; 
            stContainer.cardInfo.cardAction.url += strPageId;
        }
        break;

    case E_MODID_DOKI_UPDATE_USR_FOLLOW: // 无此模块 
    case E_MODID_DOKI_UPDATE_RECOMMEND:  // 无此模块
    case E_MODID_VIDEO_TOPIC:         // AB样式均无此模块
    case E_MODID_DOKI_PARTY:          // 无此模块
        break;

    default:
        SPP_MSG_ERROR_LOG("invalid modid: %d", eModid);
        break;
    }

    return 0;
}

template <typename T_OnaView>
int  CMsgOptSquare::SetReportParamViewA_WithoutCardBase(EModuleId eModid, T_OnaView& stContainer, const string& strExtParam)
{
    string strParam, strPageId, strStrategy;

    strStrategy = "strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
    strParam = "mod_id=" + CNum2Str::Num2Str(eModid) + "&" + strStrategy;
    strPageId = "&pageId=server_second_page_mod_" + CNum2Str::Num2Str(eModid) + "&strategyId=" + CNum2Str::Num2Str(m_iPolicyId);

    switch (eModid)
    {
    case E_MODID_PGC_VIDEO:  //饭制视频
    case E_MODID_UGC_VIDEO:
    case E_MODID_WALLPAPER: // 推荐在此给或额外拉取接口获取，待定
    case E_MODID_EMOTICON: // 
    case E_MODID_STAR_TALK:
        {
            strParam += "&data_type=button";
            strParam += "&sub_mod_id=pic";

            stContainer.reportEventId = "common_button_item_exposure";
            stContainer.cardAction.reportEventId = "common_button_item_click";

            stContainer.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;
            stContainer.cardAction.reportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;

            stContainer.reportParams = strParam;
            stContainer.cardAction.reportParams = strParam; 
            stContainer.cardAction.url += strPageId;
        }
        break;

    default:
        SPP_MSG_ERROR_LOG("invalid modid: %d", eModid);
        break;
    }

    return 0;
}

template <typename T_OnaView>
int  CMsgOptSquare::SetReportParamViewB(EModuleId eModid, T_OnaView& stContainer, const string& strExtParam)
{
    string strParam, strPageId, strStrategy, strReportKey;

    strStrategy = "strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
    strParam = "mod_id=" + CNum2Str::Num2Str(eModid) + "&" + strStrategy;
    strPageId = "&pageId=server_second_page_mod_" + CNum2Str::Num2Str(eModid) + "&strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
    
    if (IsSecondPageView())
    {
        strReportKey = "server_second_page_mod_" + CNum2Str::Num2Str(eModid);
    }
    else {
        strReportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;
    }

    switch (eModid)
    {
    case E_MODID_FEED:
        {
            strParam += "&" + strExtParam;

            stContainer.reportEventId = "feed_exposure";
            stContainer.cardInfo.cardAction.reportEventId = "feed_click";

            stContainer.reportKey = strReportKey;
            stContainer.cardInfo.cardAction.reportKey = strReportKey;

            stContainer.reportParams = strParam;
            stContainer.cardInfo.cardAction.reportParams = strParam;
        }
        break;

    case E_MODID_IP_GROUP:     // 宫格聚合
        {
            strParam += "&data_type=content";  // sub_mod_id与其它模块不一样，由extParam传进来
            strParam += "&" + strExtParam;

            stContainer.reportEventId = "common_button_item_exposure";
            stContainer.cardInfo.cardAction.reportEventId = "common_button_item_click";

            stContainer.reportKey = strReportKey;
            stContainer.cardInfo.cardAction.reportKey = strReportKey;

            stContainer.reportParams = strParam;
            stContainer.cardInfo.cardAction.reportParams = strParam; 
            stContainer.cardInfo.cardAction.url += strPageId;
        }
        break;

    case E_MODID_STAR_TALK: // 明星说普通帖子样式
    case E_MODID_ENTERTAINMENT_NEWS:
    case E_MODID_PGC_VIDEO:  //饭制视频
    case E_MODID_UGC_VIDEO:
        {
            strParam += "&data_type=content";
            strParam += "&" + strExtParam;   //帖子ID外部传入

            stContainer.reportEventId = "feed_exposure";
            stContainer.cardInfo.cardAction.reportEventId = "feed_click";

            stContainer.reportKey = strReportKey;
            stContainer.cardInfo.cardAction.reportKey = strReportKey;

            stContainer.reportParams = strParam;
            stContainer.cardInfo.cardAction.reportParams = strParam; 
            stContainer.cardInfo.cardAction.url += strPageId;
        }
        break;

    case E_MODID_DOKI_UPDATE_USR_FOLLOW: // 无此模块 
    case E_MODID_DOKI_UPDATE_RECOMMEND:  // 无此模块
    case E_MODID_VIDEO_TOPIC:         // AB样式均无此模块
    case E_MODID_DOKI_PARTY:          // 无此模块
        break;

    default:
        SPP_MSG_ERROR_LOG("invalid modid: %d", eModid);
        break;
    }

    return 0;
}

template <typename T_OnaView>
int  CMsgOptSquare::SetReportParamViewB_WithoutCardBase(EModuleId eModid, T_OnaView& stContainer, const string& strExtParam)
{
    string strParam, strPageId, strStrategy, strReportKey;

    strStrategy = "strategyId=" + CNum2Str::Num2Str(m_iPolicyId);
    strParam = "mod_id=" + CNum2Str::Num2Str(eModid) + "&" + strStrategy;
    strPageId = "&pageId=server_second_page_mod_" + CNum2Str::Num2Str(eModid) + "&strategyId=" + CNum2Str::Num2Str(m_iPolicyId);

    if (IsSecondPageView())
    {
        strReportKey = "server_second_page_mod_" + CNum2Str::Num2Str(eModid);
    }
    else {
        strReportKey = "server_page_channel_" + m_jceChannelReq.channelItemId;
    }

    switch (eModid)
    {
    case E_MODID_WALLPAPER: // 推荐在此给或额外拉取接口获取，待定
    case E_MODID_EMOTICON:  // 
    case E_MODID_STAR_TALK: // 明星驾到样式
        {
            strParam += "&data_type=content";
            strParam += "&sub_mod_id=pic";
            strParam += "&" + strExtParam;

            stContainer.reportEventId = "common_button_item_exposure";
            stContainer.cardAction.reportEventId = "common_button_item_click";

            stContainer.reportKey = strReportKey;
            stContainer.cardAction.reportKey = strReportKey;

            stContainer.reportParams = strParam;
            stContainer.cardAction.reportParams = strParam; 
            stContainer.cardAction.url += strPageId;
        }
        break;

    default:
        SPP_MSG_ERROR_LOG("invalid modid: %d", eModid);
        break;
    }

    return 0;
}

#endif // __MSG_OPT_STAR_CARD_H__
