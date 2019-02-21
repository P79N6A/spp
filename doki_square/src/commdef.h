/*
 *  Filename: commdef.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 * 	Description: 程序相关的宏定义、配置文件、错误码定义等
 *
 */

#ifndef __COMMDEF__
#define __COMMDEF__

#include <string>
#include <string.h>
#include <sstream>

#include "Attr_API.h"
#include "json.h"
#include "qqvideo_common.h"

using namespace std;

// ====cmd=======================================================
//业务请求命令字
#define CMD_DYNAMIC_DATA              0xe91f //jce::DynamicData 详情页动态数据请求
#define CMD_SECOND_PAGE               0xfdf6 //模块二级页
#define CMD_CHANNEL                   0xe893 //频道页

//中转命令字
#define CMD_TRANS_ADD_TWEET_NEW       0xebe9 //中转命令字--发表评论(新)
#define CMD_TRANS_DELETE_TWEET_NEW    0xebec //中转命令字--删除评论(评论审核、用户删除)(新)
#define CMD_DOKI_EVENT                0xf3a7 //doki事件中转命令

//下层服务命令字
#define CMD_LIVE_COMMENT_BEF          0xe903 //直播前评论
#define CMD_LIVE_COMPERE              0xe8fb //直播主持人
#define CMD_LIVE_POLL                 0xe8f6 //直播轮询服务
#define CMD_DM_ONLINE_LIST            0xe8fd //直播弹幕
#define CMD_GET_FEED_DETAIL           0xf082 //读取帖子详情
#define CMD_GET_FT_ATTENT             0xf1bc //读取饭团关注列表
#define CMD_GET_FEED_LINE             0xf085 //请求帖子详情(列表显示时使用，显示的是摘要信息,无回复评论数据)
#define CMD_GET_FT_INFO               0xf41b //请求饭团基础信息（为了获取明星粉丝数据）
#define CMD_GET_FANSCLUB_IDX          0xf1e7 //请求后援会第一条索引
#define CMD_GET_101_VOTE              0xf55a //获取用户的101投票列表
#define CMD_GET_COME_ONE_VOTE         0xf6ee //获取明日之子排行榜信息、用户投票信息
#define CMD_GET_CHAOYIN_RANK          0xf795 //获取明日之子排行榜信息、用户投票信息

#define CMD_GET_FLW_STARNEWS          0xf831 //关注明星的明星动态
#define CMD_GET_REC_STARNEWS          0xf832 //推荐明星的明星动态


// 消息订阅命令字
#define CMD_CONSUMER_FANS_ADMIN_JIAJING   0xf9ea //帖子加精或壁纸生成雪球中转
#define CMD_GET_FEED_FANS             0xf9ff // 帖子圈粉数
#define CMD_JOIN_DOKI_WELCOME         0xf9f6 // 加入DOKI弹窗 
#define CMD_FANS_TASK                 0xf24e // 粉丝任务中转
#define CMD_FLW_DOKI_TASK             0xec29 // 关注任务中转
#define CMD_FEED_FAN_RANK             0xfacb // DOKI下用户帖子圈粉数排行榜
#define CMD_DOKI_LIST                 0xfb49 // 关注的DOKI列表  -- 纯ID接口
#define CMD_DOKI_LIST_APP             0xfb80 // 关注的DOKI列表  -- APP接口
#define CMD_DOKI_

// ====常量定义===================================================
//直播状态
#define STATUS_PID_BEF  1
#define STATUS_PID_ING  2
#define STATUS_PID_END  3

//富媒体类型 : 0普通评论，1 带图片, 2 带视频, 4 投票, 8 竞猜, 16 语音, 32 抽奖, 64 H5, 128 道具,  4096 直播互动信息 ...  bit位，
#define RICHTYPE_NORMAL             0
#define RICHTYPE_PIC                1
#define RICHTYPE_VIDEO              2
#define RICHTYPE_VOTE               4
#define RICHTYPE_VOTE_GUESS         8
#define RICHTYPE_AUDIO              16
#define RICHTYPE_LOTTERY            32
#define RICHTYPE_H5                 64
#define RICHTYPE_PROP               128
#define RICHTYPE_INTERACTION        4096   //直播互动信息

// redis存储的key前缀
#define REDIS_PREFIX_JIAJING_ADMIN     "jiajing_admin_"       //加精操作人有序集       jiajing_admin_dokiid
#define REDIS_PERFIX_JIAJING_POSTER    "jiajing_poster_"      //被加精发表者集合       jiajing_poster_dokiid_yearweek
// 注意：REDIS_PREFIX_FANSVAL必须与load_data.py导数据脚本写入的KEY保持一致
#define REDIS_PREFIX_FANSVAL           "fansval_rank_"        //用户粉丝值有序集       fansval_rank_nameid      粉丝值  vuid
#define REDIS_PREFIX_TOPTEN_POSTER     "topten_poster_"       //十佳帖发帖人           周维度

// 以下两个索引由添加关注接口写入
#define REDIS_PREFIX_FEED_FANS         "feedfans_"            //帖子圈粉数有序集       feedfans_feedid_dokiid
#define REDIS_PREFIX_FEED_FANS_RANK    "feedfans_rank_"       //用户圈粉数排行有序集    feedfans_dokiid_yearmonthday

// 以下索引由加入弹窗接口写入
#define REDIS_PREFIX_USR_INFO          "usrinfo_"             //用户信息哈希集         usrinfo_vuid
#define USR_INFO_HASH_FIELD_LETTER     "letter"               //标识是否弹出过团长信
#define USR_INFO_HASH_FIELD_RANK       "rank_"                //用户在dokiid下的排名

// 以下索引由DOKI列表接口与DOKI基本信息接口使用
#define MY_FLW_LIST                    "ft_myfantuan_"        //用户关注doki列表

//boss日志上报
#define BOSS_ID                        6745 

//粉丝等级体系各级别粉丝值
#define LV8_FANSVAL                    3000
#define LV9_FANSVAL                    4500
#define LV10_FANSVAL                   9000
#define LV11_FANSVAL                   13500
#define LV12_FANSVAL                   18000

//事件上报
#define DOKI_CHANNEL_FOCPIC_REPORT_KEY "doki_channel_focpic_100224"
#define DOKI_CHANNEL_RANK_TITLE_REPORT_KEY "doki_channel_chart_tag_100224"  // 榜单标题
#define DOKI_CHANNEL_RANK_ITME_REPORT_KEY "doki_channel_chart_item_100224"  // 榜单内容
#define DOKI_CHANNEL_FEED_REPORT_KEY "doki_channel_polymer_100224"  //doki feed信息
#define DOKI_CHANNEL_TOPIC_TITLE_REPORT_KEY "doki_channel_topic_tag_100224"
#define DOKI_CHANNEL_TOPIC_ITEM_REPORT_KEY "doki_channel_topic_single_100224"

// 6.1.0 report key
#define DOKI_COMMON_JCE_CLICK_KEY "video_jce_action_click"  //通用
#define DOKI_SQUAER_PR_FOCUS_PIC_KEY "doki_square_focus_promotion_100224"
#define DOKI_SQUARE_TOPIC_WITH_PIC_KEY "doki_channel_topic_singlepic_100224" //话题
#define DOKI_SQUARE_VOTE_CLICK_KEY "doki_star_home_vote_button_click" //投票
#define DOKI_SQUARE_CHAT_MSG_LIST_CLICK_KEY "doki_star_home_msg_button_click"

// 跳转链接宏定义
#define VIDEO_ACTION_URL        "txvideo://v.qq.com/VideoDetailActivity"
#define LIVE_ACTION_URL         "txvideo://v.qq.com/TencentLiveActivity"
#define H5_ACTION_URL           "txvideo://v.qq.com/Html5Activity"
#define FEED_DETAIL_ACTION_URL  "txvideo://v.qq.com/PrimaryFeedDetailTimelineActivity"
#define DOKI_TOPIC_VIDEO_ACTION_URL "txvideo://v.qq.com/TopicDetailONAViewsActivity"
#define DOKI_TOPIC_ACTION_URL   "txvideo://v.qq.com/TopicDetailActivity"
#define TOPIC_ACTION_URL        ""
#define DOKI_HOMEPAGE_ACTION_URL "txvideo://v.qq.com/FanCircleActivity"
#define DOKI_SECOND_PAGE_ACTION_URL "txvideo://v.qq.com/DokiGroupDataActivity?dataKey="
#define DOKI_WALLPAPER_ACTION_URL   "txvideo://v.qq.com/DokiWelfareActivity?dataKey="
#define DOKI_EMOTICON_ACTION_URL   "txvideo://v.qq.com/DokiWelfareActivity?dataKey="
#define DOKI_VIP_ICON            "https://puui.qpic.cn/vupload/0/20181224_1545620710866_bqypugr70z.png/0"


// 模块间距
#define MODULE_SPACE_HEIGHT     16

// union批处理数量
#define UNION_BATCH_CNT         30

// ====存储数据===================================================
struct StBossData
{
    StBossData()
    {
         lBossId = lError = lCmd = lQQ = lVuid = 0;
    }

    // BOSS上报字段
    long lBossId;
    long lCmd;
    long lQQ;
    long lVuid;
    long lError;

    string strGuid;
    string strOmgid;
    string strOpenid;

    // 每一对KV数值，采用格式"(key=value)"
    string strKvData; 

    // 过滤字段
    string strFilter1;
    string strFilter2;

    // 中间状态数据 -- 不直接用于上报
    string strMidData; 
};


struct StStarInfo //存储明星信息
{
    int    iDokiType;
	string strStarId;
	string strStarName;
	string strStarHead;
	string strFansId;
    string strRankList;    //榜单状态

    StStarInfo()
    {
        iDokiType = 0;
    }
};


struct StNewPidInfo
{
    string strPid;
    string strTitle;
    int    iPlayStatus;     //union中的直播状态
    string strLiveType;
    string strBeginTime;
    string strEndTime;
    string strTargetId;
    string strRoseId;
    string strStreamRatio;
    int    iLiveStatus;    //union直播状态ving直播和fans直播不一样，这里转成一致的直播状态

    long llPlayCount;      //从livepoll获取的直播在线人数，用于直播前、中卡片的显示
    long llAttentCount;
    long llPlayCountUnion; //union中的直播人数，用于直播后卡片的显示

    long llFeedTime;       //发表时间
    jce::MediaContentInfo stContentInfo;

    StNewPidInfo()
    {
        iPlayStatus = 0;
        iLiveStatus = 0;
        llPlayCount = 0;
        llPlayCountUnion = 0;
        llAttentCount = 0;
        llFeedTime = 0;
    }
};


struct StFanLvl
{
    StFanLvl()
    {
        iLevelId = 1;
        strName = "路人粉";
    }

    int iLevelId;   // 1 ~ 12等级
    string strName; // 等级名称
};


// ====配置文件===================================================
struct StConfInfo
{
    int iMsgTimeout_ms;
    int iIsTransCheckEnv;
    int iIsTestEnv;
    int iProxyTimeout_ms;

    StConfInfo()
    {
        reSetData();
    }

    void reSetData()
    {
        iMsgTimeout_ms   = 0;
        iIsTransCheckEnv = 0;
        iIsTestEnv       = 0;
        iProxyTimeout_ms = 0;
    }

    string OutConfData()
    {
        stringstream oss;
        oss<<"iMsgTimeout_ms="<<iMsgTimeout_ms
            <<";iIsTransCheckEnv="<<iIsTransCheckEnv
            <<";iIsTestEnv="<<iIsTestEnv;

        return oss.str();
    }
};


// CMS配置项
// 当前使用于 DOKI大爬梯与日报模块
struct StCmsItem
{
    string strTitle;   //标题
    string strPic;     //子模块图片
    string strPid;     //直播需根据ving直播或个人直播下发不同的链接
    string strGif;     //gif图片

    // 横图
    string pic_540x304; //
    string pic_498x280; //

    // 竖图
    string pic_324x454;
    string pic_216x302; 

    // 焦点图
    string pic_1080x404; //
    string pic_1300x404; //

    //模块跳转链接 -- 非CMS原始数据， 为最终返回给终端的链接
    //譬如配置的H5为：http://v.qq.com/activty/xxx，则strUrl最终值
    //为：txvideo://v.qq.com/Html5Activity?url=httpxxx
    string strActionUrl;     

    //用于上报的参数，格式： key1=val1&key2=val2
    string strRptParams;

    enum EItemForModule
    {
        EFOR_ANONYMOUS  = 0,
        EFOR_DAILY_NEWS = 1,
        EFOR_DOKI_PARTY = 2,
        EFOR_FOCUS_PIC  = 3,
    };

    void Parse(Json::Value &jsonValue, EItemForModule ForWhich= EFOR_ANONYMOUS)
    {
        string title, ctype;
        DEF_GET_JSON_STR(strTitle, "title");
        DEF_GET_JSON_STR(ctype, "ctype");

        // 数据上报
        if (!strTitle.empty())
        {
            strRptParams = "title=";
            strRptParams += strTitle;
        }

        ostringstream oss;

        // 每个CMS项只会配置一种数据
        // 专辑 -- 
        if (ctype == "album")
        {
            string cid;
            DEF_GET_JSON_STR(cid, "cid");

            if (!cid.empty()){
                oss << VIDEO_ACTION_URL << "?cid=" << cid << "&isAutoPlay=1";
                strRptParams += strRptParams.empty() ? ("cid=") : ("&cid=");
                strRptParams += cid;
            }
        }
        // 视频
        else if (ctype == "video")
        {
            string vid;
            DEF_GET_JSON_STR(vid, "vid");

            if (!vid.empty()){
                oss << VIDEO_ACTION_URL << "?vid=" << vid << "&isAutoPlay=1";
                strRptParams += strRptParams.empty() ? ("vid=") : ("&vid=");
                strRptParams += vid;
            }
        }
        // 直播
        else if (ctype == "program")
        {
            // todo: 直播 -- 需要读取union数据 -- 判断横屏竖屏
            string pid;
            DEF_GET_JSON_STR(pid, "pid");

            /*if (mpLiveInfo.find(stMyPic.value) != mpLiveInfo.end())
            {
                oss << LIVE_ACTION_URL << "?pid=" << stMyPic.value;
                stPidInfo& pid_info = mpLiveInfo[stMyPic.value];

                if (pid_info.live_type == "fans")
                {
                    oss << "&style=1";
                }

                oss << "&streamStyle=" << GetStreamStyleFromStreamRatio(pid_info.stream_ratio);
            }
            else*/
            if (!pid.empty())
            {
                strPid = pid;
                oss << LIVE_ACTION_URL << "?pid=" << pid << "&isFullScreen=0";
            }
        }
        else if (ctype == "h5")
        {
            string h5_url;
            DEF_GET_JSON_STR(h5_url, "h5_url");

            if (!h5_url.empty() && h5_url.find("http") == 0)
            {
                // HTTP链接
                oss << H5_ACTION_URL << "?url=" << CNewUrlCode::UrlEncode(h5_url);
            }
            else if (!h5_url.empty())
            {
                // 非HTTP链接
                oss << h5_url;
            }
        }
        // 帖子详情
        else if (ctype == "forum")
        {
            string forumId;
            DEF_GET_JSON_STR(forumId, "forumId");

            if (!forumId.empty())
            {
                string datakey;
                datakey += "id=" + forumId;
                datakey += "&scene=fantuanexcl";

                oss << FEED_DETAIL_ACTION_URL << "?dataKey=" << CNewUrlCode::UrlEncode(datakey);
            }
        }
        // doki话题
        else if (ctype == "doki_toplic")
        {
            string dokiTid;
            DEF_GET_JSON_STR(dokiTid, "dokiTid");

            //跳转到话题详情页
            if (!dokiTid.empty())
            {
                vector<string> vecElems;
                CStrUnits::SplitToVec(dokiTid, "_", vecElems);

                int iTopicType = 0;
                string datakey;

                if (vecElems.size() >= 2)
                {
                    iTopicType = atoi(vecElems[0].c_str());
                }

                if (iTopicType == 3) //视频话题
                {
                    datakey += "topicId=" + dokiTid;
                    oss << DOKI_TOPIC_VIDEO_ACTION_URL <<"?demandVersion=6.1.5";
                    oss << "&dataKey=" << CNewUrlCode::UrlEncode(datakey);
                }
                else
                {
                    datakey += "topicId=" + dokiTid;
                    oss << DOKI_TOPIC_ACTION_URL << "?dataKey=" << CNewUrlCode::UrlEncode(datakey);
                }
            }
        }
        // 非doki话题
        else if (ctype == "event")
        {
            string eventId;
            DEF_GET_JSON_STR(eventId, "eventId");

            //跳转到话题详情页
            if (!eventId.empty())
            {
                vector<string> vecElems;
                CStrUnits::SplitToVec(eventId, "_", vecElems);

                int iTopicType = 0;
                string datakey;

                if (vecElems.size() >= 2)
                {
                    iTopicType = atoi(vecElems[0].c_str());
                }

                if (iTopicType == 3) //视频话题
                {
                    datakey += "topicId=" + eventId;
                    oss << "txvideo://v.qq.com/TopicDetailONAViewsActivity?demandVersion=6.1.5";
                    oss << "&dataKey=" << CNewUrlCode::UrlEncode(datakey);
                }
                else
                {
                    datakey += "topicId=" + eventId;
                    oss << "txvideo://v.qq.com/TopicDetailActivity?dataKey=" << CNewUrlCode::UrlEncode(datakey);
                }
            }
        }
        // 明星主页
        else if (ctype == "doki")
        {
            string dokiId;
            DEF_GET_JSON_STR(dokiId, "dokiId");

            if (!dokiId.empty())
            oss << DOKI_HOMEPAGE_ACTION_URL << "?starid=" << dokiId;
        }
        
        strActionUrl = oss.str();
        if (ForWhich == EFOR_DOKI_PARTY)
        {
            DEF_GET_JSON_STR(strPic, "pic_498x280");
            // 横图
            DEF_GET_JSON_STR(pic_540x304, "pic_540x304");
            DEF_GET_JSON_STR(pic_498x280, "pic_498x280");

            // 竖图
            DEF_GET_JSON_STR(pic_324x454, "pic_324x454");
            DEF_GET_JSON_STR(pic_216x302, "pic_216x302");

            DEF_GET_JSON_STR(strGif, "gif");
        }
        else if (ForWhich == EFOR_FOCUS_PIC)
        {
            DEF_GET_JSON_STR(pic_1080x404, "pic_1080x404");
            DEF_GET_JSON_STR(pic_1300x404, "pic_1300x404");
        }
    }
};


// 图片质量排序
struct StPhotoSorter
{
    map<string, int>& mpPhotoScore;
    StPhotoSorter(map<string, int>& mpPhotoToScore) 
        : mpPhotoScore(mpPhotoToScore)
    {

    }

    bool operator()(const jce::CircleMsgImageUrl& p1, const jce::CircleMsgImageUrl& p2)
    {
        return mpPhotoScore[p1.url] < mpPhotoScore[p2.url];
    }
};


typedef struct _stDatakey
{
    //  广场页参数
    int pagesize;
    int pagenum;    // 从0开始

    //  模块二级页参数
    int modid;      // 模块id -- 用于模块二级页请求

    // 扩展参数 -- 只用于广场B样式ugc饭拍视频模块
    // extid=feedid 表示读取老样式feed流，并把feedid放在第一个feed下发；用于给app支持视频沉浸式播放和左右横滑下一个视频
    // extid=空     表示点击饭拍视频模块进入模块二级页，出卡片化的自拍视频feed流（658定义结构体）。空为默认值。
    string extid;      // 扩展ID -- 仅用于饭拍(UGC)视频，extid=1则表示从广场页拉取视频流，采用CirclePrimaryFeed下发

    // 用于设定推荐请求参数
    // A/B两种样式，IP宫格模块对应的二级页请求参数有区别
    int fromView;      // 相当于策略ID -

    // cms模块id，用去推荐数据请求
    string strCMSId;

    _stDatakey()
    {
        pagesize = 10;
        pagenum = 0;
        modid = 0;
        fromView = 0;
    }

}StDatakey;


enum EVideoType
{
    EVT_UNDEFINE,
    EVT_VID,
    EVT_CID,
    EVT_PID,
};

struct StVideoInfo
{
    string strCid;
    string strVid;
    EVideoType eType;

    // 视频相关信息
    long   lDuration;      // 时长，秒   
    double fWHRatio;       // 宽高比，小于1表示竖屏
    string strTitle;       // 标题
    string strActionUrl;   // 点击视频的跳转链接

    // 处理函数
    StVideoInfo()
    {
        eType = EVT_UNDEFINE;
        lDuration = 0;
        fWHRatio = 0.000001;    // 默认为横屏
    }

    bool IsCid() { 
        return eType == EVT_CID; 
    }

    bool IsVid() { 
        return eType == EVT_VID;  
    }

    string GetId(){
        if (eType == EVT_CID)
            return strCid;
        else if (eType == EVT_VID)
            return strVid;
        else 
            return "";
    }

    void SetVid(const string& id, const string& strUrl = "")
    {
        if (!id.empty())
        {
            strVid = id;
            strActionUrl = strUrl;

            eType = EVT_VID;
        }
    }

    void SetCid(const string& id, const string& strUrl = "")
    {
        if (!id.empty())
        {
            strCid = id;
            strActionUrl = strUrl;

            eType = EVT_CID;
        }
    }
};


// 模块ID定义
enum EModuleId
{
    E_MODID_FEED              = 10000,         // 单个帖子
    E_MODID_FOCUS_PIC         = 10001,         // 焦点图
    E_MODID_DAIYLY_REPORT     = 10002,         // 日报
    E_MODID_RENQI_MODCHAIN    = 10003,         // 星耀榜系列模块
    E_MODID_HOT_TOPIC         = 10004,         // 话题热榜
    E_MODID_DOKI_UPDATE_USR_FOLLOW = 10005,    // 明星卡片 -- 关注的明星
    E_MODID_DOKI_UPDATE_RECOMMEND = 10006,     // 明星卡片 -- 为你推荐的明星
    E_MODID_DOKI_RENQI_UPDATE = 10007,         // 人气榜变更播报 
    E_MODID_VIDEO_TOPIC       = 10008,         // 视频话题
    E_MODID_DOKI_PARTY        = 10009,         // DOKI大爬梯
    E_MODID_STAR_TALK         = 10010,         // 明星说
    E_MODID_PGC_VIDEO         = 10011,         // 饭制视频
    E_MODID_UGC_VIDEO         = 10012,         // 饭拍视频
    E_MODID_WALLPAPER         = 10013,         // 美图/壁纸
    E_MODID_EMOTICON          = 10014,         // 表情包
    E_MODID_TOPTEN            = 10015,         // 十佳帖
    E_MODID_ENTERTAINMENT_NEWS= 10016,         // 娱乐资讯
    E_MODID_IP_GROUP          = 10017,         // 定制组合 - 大IP聚合模块
};


// 需求单：http://tapd.oa.com/doki/prong/stories/view/1020357852063515463
// A样式 对应双瀑布feed流
// B样式 对应固定模块+feed流
// AB宏定义值 与真实策略ID相对应，有liaosong分配
enum ESquareViewType
{
    E_VIEW_C =   0,    // 广场老版本样式，【广场】出固定模块，【推荐】出推荐feed流
    E_VIEW_A = 218,    // 双瀑布feed流，feed卡片化
    E_VIEW_B = 219,    // 【广场】+ 【推荐】 广场模块卡片化，【推荐】出老样式feed流（非卡片化）
    E_VIEW_SECOND_PAGE = 10000, // 二级页
};

// 推荐接口定义
/*
    请求参数：
    +-- header
    +-- seq_num  //
    +-- src_key  // 100224 - 前面模块后面帖子流
    110697 - 双瀑布流
    +-- rec_ctx
    +-- rec_scene  // 填2
    +-- ctx_paras  // kv形式
    +-- key : "module_num"; value : 10        // 请求模块数，一页下发10个模块
    +-- key : "module_item_num" ; value : 10  // 请求视频数，填1
    +-- key : "page_turn_info"; value : 首页填空，第二页填page=1，第三页page=2，依次类推

    回包：
    +-- header
        +-- seq_num
        +-- src_key
    +-- status
    +-- rsp_code  // 填错误码
    +-- rsp_msg   // 填错误信息
    +-- rec_modules     // 模块id列表
    +-- UniRecStruct::RecModule
    +-- module_id     // 模块id
    +-- module_type   // 模块类型   通过这个区别是什么模块，
        cms模块-410
        DOKI广场帖子聚合-459
        DOKI广场明星说-460
        DOKI广场饭拍-461,
        DOKI广场饭制-462
        娱乐热点-463
    +-- rec_items
    +-- UniRecStruct::RecItem
    +-- item_id     // 推荐物品id
    +-- item_type   // 推荐物品类型，14是帖子，4是直播
    +-- item_paras  // kv形式，以下仅包括推荐模块，如明星说、饭拍饭制、纯一个帖子模块里的物品属性，CMS视频/帖子/直播属性会在这个基础上扩展。
    +-- key : item_score; value : 视频的分值
    +-- key : recall_alg; value : 上报算法
    +-- key : item_report; value: 上报信息
    +-- key : item_rec_reason; value : 关联的dokiid,(样式A明星说根据该位置判断doki头像)\
    +-- module_paras  // 模块属性 以下仅包括推荐模块属性，如明星说、饭拍饭制、纯一个帖子模块的模块属性，CMS模块属性会在这基础上扩展
    +-- key: module_rec_title; value : 模块标题，CMS模块不通过这个字段判断，cms属性中会带模块标题、跳转链接等属性
    +-- key : m_type; value :  cms的模块类型
    占位符       type_35
    普通cms模块   type_77
    置顶帖子模块  type_125
    APP焦点图    type_130
    小入口       type_164
    日报         type_163
    +-- key : ZT_leaf_ctype; value :  标识是什么占位符模块
    视频话题  extend_doki_videotopic
    宫格聚合  extend_doki_box_together
    表情      extend_doki_expression
    美图      extend_doki_mito
    十佳饭贴  extend_doki_toptenforum
    为你推荐  extend_doki_recommendedforyou
    明星动态  extend_doki_stardynamics
    榜单播报  extend_doki_broadcasting
    热门话题  extend_hot_topic
    明星驾到  extend_star_arrive
    星耀榜    extend_doki_shining
    +-- key : ZT_leaf_head; value : 模块主标题
    +-- key : ZT_leaf_pic_type； value : 小图横滑图片样式， 2-竖图 3-横图
    +-- rsp_paras  // kv形式
    +-- key : exp_id; value : 推荐分桶
*/

// 推荐接口：所有返回都是模块，包括feed
// 模块分为两个类别：推荐模块和CMS模块
enum ERecModType
{
    E_REC_MOD_CMS_SETTING = 410,
    E_REC_MOD_FEED        = 459,   // 普通帖子
    E_REC_MOD_STAR_TALK   = 460,   // 明星说
    E_REC_MOD_UGC_VIDEO   = 461,   // 饭拍视频
    E_REC_MOD_PGC_VIDEO   = 462,   // 饭制视频
    E_REC_MOD_ENTERTAINMENT = 463, // 娱乐资讯
};

enum ERecContentType
{
    EREC_TYPE_PID         = 4,    // 直播       -- 推荐定义类型
    EREC_TYPE_FEED        = 14,   // 帖子       -- 推荐定义类型
    EREC_TYPE_CMS_SETTING = 9 ,   // CMS配置    -- CMS置顶数据
    EREC_TYPE_CMS_VIDEO   = 12 ,  // CMS配置    -- CMS置顶数据
    EREC_TYPE_CMS_COVER   = 13 ,  // CMS配置    -- CMS置顶数据
    EREC_TYPE_DOKI_LIST   = 100,  // DOKI列表   -- 内部定义
};

enum EPicSize
{
    ESIZE_456_810,
    ESIZE_316_178,
    ESIZE_70_40,
    ESIZE_519_519,
};

// 用于明星说结构
struct StStarTalkFeed
{
    string strDokiid;
    string strFeedid;
    string strPid;
    vector<string> vecDokiid;   // 展示DOKI列表
    ERecContentType eType;

    // 默认为feed数据
    StStarTalkFeed()
    {
        eType = EREC_TYPE_FEED;
    }
};

#define REC_KEY_CMS_PLACEHOLDER      "type_35"   // 占位符
#define REC_KEY_CMS_BUMP_FEED        "type_125"  // 置顶帖

#define REC_KEY_CMS_BUMP_PIC         "type_130"  // 焦点图
#define REC_KEY_CMS_STAR_RANK_CHAIN  "type_164"  // 星耀榜系列模块
#define REC_KEY_CMS_DAILY_REPORT     "type_163"  // 日报
#define REC_KEY_CMS_IP_GROUP_A       "type_223"  // IP宫格A样式
#define REC_KEY_CMS_IP_GROUP_B       "type_216"  // IP宫格B样式

#define REC_KEY_PLACEHOLDER_IP       "extend_doki_box_together"
#define REC_KEY_PLACEHOLDER_EMOTICON "extend_doki_expression"
#define REC_KEY_PLACEHOLDER_WALLPAPER "extend_doki_mito"
#define REC_KEY_PLACEHOLDER_TOPTEN   "extend_doki_toptenforum"
#define REC_KEY_PLACEHOLDER_RENQI_UPDATE "extend_doki_broadcasting"
#define REC_KEY_PLACEHOLDER_HOT_TOPIC "extend_hot_topic"


#define MCALL_REPORT_FUN(start, end, moduleid, interfaceid, iRet) \
do {\
    uint64_t ddwStart = (uint64_t)start.tv_sec * 1000 + start.tv_usec / 1000; \
    uint64_t ddwEnd = (uint64_t)end.tv_sec * 1000 + end.tv_usec / 1000; \
    unsigned iTimeCost = ddwEnd - ddwStart; \
    MODCALL_INIT_REPORT(moduleid, 0, moduleid, interfaceid, iTimeCost * 1000, iRet, "127.0.0.1"); \
} while (0)


// ====错误码=====================================================
class CErrCodeProj
{
  public:
    enum ENUM_PROJ_ERROR
    {
        SUCCESS                     = 0,     //操作成功

        ERR_REQUEST_CMD_INVALID     = -7900, //请求数据无效命令字
        ERR_REQUEST_JCE_DECODE      = -7901, //请求数据jce解码失败错误
        ERR_REQUEST_INVALID_PARAM   = -7902, //请求参数错误
        ERR_REQUEST_OVERLOAD        = -7903, //请求过载

        ERR_NO_VUID                 = -7918, //用户无vuid登录信息
        ERR_DOKI_BASIC_INFO_2006    = -7919, //DOKI基本信息失败 - 2006
        ERR_DOKI_BASIC_INFO_2039    = -7920, //DOKI基本信息失败 - 2039

        ERR_REQ_RECOMMEND_FAIL      = -7921, //推荐请求失败
        ERR_REQ_RECOMMEND_EMPTY     = -7922, //无物件数据
    };

    static string GetErrorMsg(int iError)
    {
        switch(iError)
        {
            case SUCCESS               : return "SUCCESS";
            case ERR_REQUEST_JCE_DECODE: return "ERR_REQUEST_JCE_DECODE";
            case ERR_REQUEST_INVALID_PARAM: return "ERR_REQUEST_INVALID_PARAM";
        }

        stringstream ssTemp;
        ssTemp<<iError;
        return ssTemp.str();
    };
};

// ====monitor监控属性============================================
enum ENUM_MONITOR
{
    SPP_REQ_ALL                = 525542, //请求总数
    SPP_REQ_FAIL               = 525543, //请求总失败数
    SPP_REQ_AVG_TIME           = 525544, //处理平均耗时
    SPP_REQ_SUCC               = 525568, //请求总成功数

    SPP_LOGIC_ERR              = 474293, //程序逻辑错误
    REQ_DATA_ERR               = 474294, //请求数据错误总数
    GLOBAL_CONF_ERR            = 474295, //全局配置错误

    JCE_EN_ERR                 = 474296, //jce编码失败
    JCE_DE_ERR                 = 474297, //jce解码失败

    REDIS_EN_ERR               = 474298, //redis打包失败
    REDIS_DE_ERR               = 474299, //redis解包失败

    UNION_PID_ALL              = 474304, //Union获取Pid数据总数
    UNION_PID_FAIL             = 474305, //Union获取Pid数据失败数
    UNION_PID_SUCC             = 474306, //Union获取Pid数据成功数

    UNION_EN_ERR               = 487351, //union打包失败
    UNION_DE_ERR               = 487352, //union解包失败
    UNION_STAR_ALL             = 487353, //union获取明星信息总数
    UNION_STAR_FAIL            = 487354, //union获取明星信息失败数
    UNION_STAR_SUCC            = 487355, //union获取明星信息成功数

    // doki信息
    GET_DOKI_INFO_ALL,
    GET_DOKI_INFO_FAIL,
    GET_DOKI_INFO_RETERR,
    GET_DOKI_INFO_SUCC,

    MONI_FEEDID_INVALID        = 492576, //feedid无效
    CARDTYPE_GLOBALSTARCMT     = 497810, //全局明星评论卡片数

    UNICONF_GETCIDPID_STAR_ALL = 497811, //配置中心匹配cid对应直播中明星总数
    UNICONF_GETCIDPID_STAR_HIT = 497812, //配置中心匹配cid对应直播中明星命中数


    // 请求数统计
    CHANNEL_REQ_ALL = 525545,           
    CHANNEL_REQ_FAIL = 525546,
    CHANNEL_REQ_SUCC = 525547,
    CHANNEL_REQ_EMPTY = 525548,

    SECOND_PAGE_REQ_ALL = 525549,
    SECOND_PAGE_FAIL,
    SECOND_PAGE_SUCC,
    SECOND_PAGE_EMPTY = 525552,

    // 
    GET_PREPARE_DATA_1_FAIL,
    GET_PREPARE_DATA_2_FAIL,

    GET_RECOMMEND_REQ_ALL,
    GET_RECOMMEND_REQ_FAIL,
    GET_RECOMMEND_REQ_EMPTY,

    GET_FEED_DETAIL_REQ,
    GET_FEED_DETAIL_FAIL = 525559,
    GET_FEED_STAT_ERR,
    GET_FEED_DETAIL_SUCC,

    GET_WALLPAPER_REQ,
    GET_WALLPAPER_REQ_FAIL,
    GET_WALLPAPER_REQ_SUCC,

    GET_EMOTICON_REQ,
    GET_EMOTICON_FAIL,
    GET_EMOTICON_SUCC,

    GET_RECOMMEND_REQ_RPC_FAIL = 526661,

    // 
    SPP_OVERLOAD = 528049, // 过载
};

// ====模调监控属性================================================
enum ENUM_MCALL
{
    //本服务
    MCMD_THIS        = 0, //
    MCIF_THIS        = 0, //

    //外部接口
    MCMD_EXTERN_NAME = 0, //
    MCIF_ONE         = 0, //
    MCIF_TWO         = 0, //
};


enum EModuleCallId
{
    MCALL_MOD_ID = 210113607,                       // 广场卡片化
    MCALL_IF_CHANNEL                = 110312226,    // 频道页请求
    MCALL_IF_SECOND_PAGE            = 110312227,    // 模块二级页
    MCALL_IF_RECOMMEND_CHANNEL      = 110312493,    // 频道页推荐接口
    MCALL_IF_SECOND_PAGE_RECOMMEND  = 110312494,    // 模块二级页
    MCALL_IF_RECOMMEND_SECOND_PAGE  = 110312719,    // 频道页模块二级页推荐接口
};

#define PRINT_VECTOR(A) \
    for (int i=0; i < A.size(); i++) \
    { \
       SPP_MSG_DEBUG_LOG("vector[%s]", A[i].c_str()); \
    } \
    

#endif  // __COMMDEF__
