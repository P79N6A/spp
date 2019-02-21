/*
 *  Filename: myUtilsTVideo.h
 *
 *  Created on: 2016-08-22
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2016 Tencent. All rights reserved.
 *
 * 	Description: 工具函数--tencent_video相关
 *
 *
 *  修改记录：
 *      新增登陆态相关的类，新增常用的宏定义    on 2017-02-28
 *      新增登陆态相关的函数                    on 2017-03-02
 *      业务相关的函数迁移到myUtilsTVideo中     on 2017-05-11
 *      增加登陆信息操作相关的类                on 2017-06-07
 *      修改登录态获取函数                      on 2017-08-03
 *      添加拼接action跳转的函数                on 2017-08-25
 *
 */

#ifndef __MY_UTILS_TVIDEO_H__
#define __MY_UTILS_TVIDEO_H__

#include <cmath>
#include "myUtils.h"
#include "jce/Jce.h"
#include "video_packet.h"

using namespace std;

//视频平台相关的宏定义
#define APPID_DEFAULT       0        //默认为腾讯视频移动app

#define APPID_WEB_1         1000001  //腾讯视频Web端
#define APPID_CLIENT        1000003  //腾讯视频PC客户端
#define APPID_WEB           1000004  //腾讯视频Web端

#define APPID_TVIDEO_MOBILE 1000005  //腾讯视频移动app(iphone aphone ipad)

#define APPID_HLW           1000006  //系统分配的callerid，表示腾讯视频下的好莱坞业务。
#define APPID_VWEB          1000007  //系统分配的callerid，表示腾讯视频下的v站业务。
#define APPID_TV            1000008  //系统分配的appid，表示腾讯视频下的tv业务。
#define APPID_VPP           1000009  //系统分配的appid，表示腾讯视频下的v+业务。

#define APPID_XIANFENG      1100001  //接入层分配的appid，表示腾讯先锋app端。
#define APPID_LIVE_FANS     1100002  //接入层分配的appid，表示企鹅直播app端。现在改名为腾讯直播了
#define APPID_NEWS_APP      1100003  //接入层分配的appid，表示新闻直播app端。
#define APPID_SPORT_APP     1100004  //接入层分配的appid，表示腾讯体育app端。
#define APPID_KD_APAD       1100005  //接入层分配的appid，表示腾讯视频apad端。儿童版也是用的这个

#define APPID_HTTP_WEB      3000001  //http统一接入层--web端通用appid，一般用在v站以及其他无法归属的端
#define APPID_HTTP_WXTINY   3000002  //http统一接入层--http统一接入层--微信小程序
#define APPID_HTTP_PC_MAC   3000003  //http统一接入层--http统一接入层--mac客户端
#define APPID_HTTP_33       3000004  //http统一接入层--三生三世十里桃花（微信小程序）
#define APPID_HTTP_PC_WIN   3000005  //http统一接入层--pc客户端
#define APPID_HTTP_TV       3000006  //http统一接入层--tv端
#define APPID_HTTP_WANGZHE  3000007  //http统一接入层--王者荣耀
#define APPID_HTTP_XUANJING 3000008  //http统一接入层--腾讯炫镜
#define APPID_HTTP_MEIZU    3000009  //http统一接入层--魅族


//平台编号
#define PLATFORM_PC          1       //视频平台PC客户端
#define PLATFORM_WEB         2       //视频平台Web端
#define PLATFORM_APHONE      3       //视频平台APhone
#define PLATFORM_IPAD        4       //视频平台IPad
#define PLATFORM_IPHONE      5       //视频平台IPhone
#define PLATFORM_TV          8       //视频平台TV盒子

//登录态类别标志
#define LOGIN_STRONG_TYPE    1        //视频登陆-QQ强登陆
#define LOGIN_WEAK_TYPE      7        //视频登陆-QQ弱登陆
#define LOGIN_VASKEY_TYPE    8        //视频登陆-Vaskey登陆
#define LOGIN_VUSER_TYPE     9        //视频登陆-内部vuserid登陆
#define LOGIN_WEIXIN_TYPE    100      //视频登陆-微信登陆

//用户登陆类型
#define LOGIN_FLAG_UNDEF     0
#define LOGIN_FLAG_QQ        1
#define LOGIN_FLAG_WX        2
#define LOGIN_FLAG_GUID      3

namespace MyUtils
{

// ================ class CJceCode===================================================
class CJceCode //jce数据打解包
{
public:
	//jce打包函数
    template<typename JceObj>
    static int EncodeJcePkg(std::string &strOutput, JceObj &stInJce, std::string &strErrMsg)
    {
        try
        {
            taf::JceOutputStream<taf::BufferWriter> osJce;
            stInJce.writeTo(osJce);
            strOutput.assign(osJce.getBuffer(),osJce.getLength());
        }
        catch(exception &e)
        {
            strErrMsg = string(e.what());
            return -1;
        }
        return 0;
    }

    //jce打包函数
    template<typename JceObj>
    static int EncodeJcePkgAppend(std::string &strOutput, JceObj &stInJce, std::string &strErrMsg)
    {
        try
        {
            taf::JceOutputStream<taf::BufferWriter> osJce;
            stInJce.writeTo(osJce);
            strOutput.append(osJce.getBuffer(),osJce.getLength());
        }
        catch(exception &e)
        {
            strErrMsg = string(e.what());
            return -1;
        }
        return 0;
    }

    //jce解包函数
    template<typename JceObj>
    static int DecodeJcePkg(JceObj &stOutJce, const std::string &strInput, std::string &strErrMsg)
    {
        try
        {
            taf::JceInputStream<taf::BufferReader> isJce;
            isJce.setBuffer(strInput.c_str(),strInput.length());
            stOutJce.readFrom(isJce);
        }
        catch(exception &e)
        {
            strErrMsg = string(e.what());
            return -1;
        }
        return 0;
    }

    //jce解包函数
    template<typename JceObj>
    static int DecodeJcePkg(JceObj &stOutJce,const char *pDataBuf, uint32_t uiDataLen, std::string &strErrMsg)
    {
        try
        {
            taf::JceInputStream<taf::BufferReader> isJce;
            isJce.setBuffer(pDataBuf,uiDataLen);
            stOutJce.readFrom(isJce);
        }
        catch(exception &e)
        {
            strErrMsg = string(e.what());
            return -1;
        }
        return 0;
    }
};

// ================ class CVPacketJceCode============================================
class CVPacketJceCode
{
public:
	//以video_packet为包头的jce数据打包函数
	template <typename T_inJce>
    static int SetReqData(T_inJce  &jceInData, unsigned short usCmd, CVideoPacket &oVideoPacket, string &strErrMsg)
    {
        string strJceData;
        try
        {
            taf::JceOutputStream<taf::BufferWriter> osJce;
            jceInData.writeTo(osJce);
            strJceData.assign(osJce.getBuffer(),osJce.getLength());
        }
        catch(exception &e)
        {
            strErrMsg = string(e.what());
            return -1;
        }

        oVideoPacket.setCommand(usCmd);
        oVideoPacket.setBody(strJceData);
        int iRet = oVideoPacket.encode();
        if(0 != iRet)
        {
            strErrMsg = "videoPacket_error: encode err[" + CNum2Str::Num2Str(iRet) + "]";
            return -2;
        }
        return 0;
    }

    //以video_packet为包头的jce数据解包函数
    template <typename T_outJce>
    static int GetRspData(T_outJce &jceOutData, const char *pBuf, int iBufLen, string &strErrMsg)
    {
        CVideoPacket oVideoPacket;
        oVideoPacket.set_packet((uint8_t*)pBuf, iBufLen);
        int iRet = oVideoPacket.decode();
        if (iRet != 0)
        {
        	strErrMsg = "videoPacket_error: decode err[" + CNum2Str::Num2Str(iRet) + "]";
            return -2;
        }

        try
        {
            taf::JceInputStream<taf::BufferReader> isJce;
            isJce.setBuffer(oVideoPacket.getBody().c_str(),oVideoPacket.getBody().length());
            jceOutData.readFrom(isJce);
        }
        catch(exception &e)
        {
            strErrMsg = string(e.what());
            return -1;
        }

        return 0;
    }

    //以video_packet为包头的jce数据解包函数，需获取video_packet包头
    template <typename T_outJce>
    static int GetRspDataWithVPacket(T_outJce &jceOutData, const char *pBuf, int iBufLen, CVideoPacket &oVideoPacket, string &strErrMsg)
    {
        oVideoPacket.set_packet((uint8_t*)pBuf, iBufLen);
        int iRet = oVideoPacket.decode();
        if (iRet != 0)
        {
            strErrMsg = "videoPacket_error: decode err[" + CNum2Str::Num2Str(iRet) + "]";
            return -2;
        }

        try
        {
            taf::JceInputStream<taf::BufferReader> isJce;
            isJce.setBuffer(oVideoPacket.getBody().c_str(),oVideoPacket.getBody().length());
            jceOutData.readFrom(isJce);
        }
        catch(exception &e)
        {
            strErrMsg = string(e.what());
            return -1;
        }

        return 0;
    }
};

// ================ class CSpeWork===================================================
class CSpeWork //业务相关的操作函数
{
public:
    //cookie中的uin一般会在带上o的前缀,这个函数去掉uin的字母前缀
    static int64_t GetUinFromCookie(const string &strCookieUin)
    {
        if (strCookieUin.empty())
            return 0;

        int size = strCookieUin.size();
        int i = 0;
        while (i < size)
        {
            if (isdigit(strCookieUin[i]))
                break;
            i++;
        }

        int64_t llUin = atoll(strCookieUin.substr(i).c_str());
        return llUin;
    }

    static string GetStrUinFromCookie(const string &strCookieUin)
    {
        if (strCookieUin.empty())
            return "";

        int size = strCookieUin.size();
        int i = 0;
        while (i < size)
        {
            if (isdigit(strCookieUin[i]) && '0' != strCookieUin[i])
                break;
            i++;
        }

        return strCookieUin.substr(i);
    }

    //是否是合法的vid 11位数字和小写字母的组合
    static bool IsValidVId(const string &strVid)
    {
        if(strVid.length() != 11)//字符串为空返回false
            return false;

        for(size_t uiIdx = 0; uiIdx < strVid.length(); uiIdx ++)
        {
            bool bTemp = (strVid[uiIdx] >= '0' && strVid[uiIdx] <= '9')
                            || (strVid[uiIdx] >= 'A' && strVid[uiIdx] <= 'Z')
                            || (strVid[uiIdx] >= 'a' && strVid[uiIdx] <= 'z');

            if(!bTemp)
                return false;
        }

        return true;
    }

    //是否是合法的cid 15位数字和小写字母的组合
    static int IsValidCId(const string &strCid)
    {
        if(strCid.length() != 15)
            return false;

        for(size_t uiIdx = 0; uiIdx < strCid.length(); uiIdx ++)
        {
            bool bTemp = (strCid[uiIdx] >= '0' && strCid[uiIdx] <= '9')
                            || (strCid[uiIdx] >= 'A' && strCid[uiIdx] <= 'Z')
                            || (strCid[uiIdx] >= 'a' && strCid[uiIdx] <= 'z');

            if(!bTemp)
                return false;
        }

        return true;
    }

    //评论的articleid转换成视频的pid vid cid 或者lid(lid 目前判断不了)
    //iFlag输出(实际为union的字典表): 0==>未知(可能是lid 也可能是未知的值) 2010==>pid 1001==>vid 1003==>cid 1006==>ugc(V+)
    //strVideoId输出: pid 或者 vid 或者cid 或者V+视频的vid
    static void ArticleId2VideoId(const string &strArticleId,string &strVideoId,int &iFlag)
    {
        //作为iFlag返回数据值,实际为union的字典表
        const int FLAG_Unkown = 0;
        const int FLAG_Pid    = 2010;
        const int FLAG_Vid    = 1001;
        const int FLAG_Cid    = 1003;
        const int FLAG_Ugc    = 1006;

        strVideoId = "";
        iFlag = FLAG_Unkown;

        //判断pid(前缀pid=,且为数字)
        string strFix = "pid=";
        size_t uiPos = strArticleId.find(strFix);
        if(uiPos != string::npos)
        {
            strVideoId = strArticleId.substr(uiPos+strFix.length());
            if(CStrUnits::StrIsNum(strVideoId))
                iFlag = FLAG_Pid;
            else
                iFlag = FLAG_Unkown;
        }

        //判断ugc(前缀VPP_,且为数字或字符)
        strFix = "VPP_";
        uiPos = strArticleId.find(strFix);
        if(uiPos != string::npos)
        {
            strVideoId = strArticleId.substr(uiPos+strFix.length());
            if(CStrUnits::StrIsNumOrAZaz(strVideoId))
                iFlag = FLAG_Ugc;
            else
                iFlag = FLAG_Unkown;
        }


        //判断vid(后缀_0,且为11个数字或者字符)
        strFix = "_0";
        uiPos = strArticleId.find(strFix);
        if(uiPos != string::npos)
        {
            strVideoId = strArticleId.substr(0,uiPos);
            if(CStrUnits::StrIsNumOrAZaz(strVideoId,11))
                iFlag = FLAG_Vid;//vid
            else
                iFlag = FLAG_Unkown;//未知
        }

        //判断cid(15个字符或者数字)
        if(CStrUnits::StrIsNumOrAZaz(strArticleId,15))
        {
            strVideoId = strArticleId;
            iFlag = FLAG_Cid;
        }

        return;
    }

    static string CreateUserKey(const string &strMainLogin, int64_t llUin, int64_t llVuserid)
    {
        if("qq" == strMainLogin)
            return CNum2Str::Num2Str(llUin);
        else if("wx" == strMainLogin)
            return "v" + CNum2Str::Num2Str(llVuserid);
        else
            return "";
    }

    //视频的平台号转成评论使用的平台号
    static int TransPlatVideoToComment(int iVideoPlat)
    {
        switch(iVideoPlat)
        {
            case PLATFORM_WEB: //2
                return 0;
            case PLATFORM_APHONE: //3
                return 8;
            case PLATFORM_IPHONE: //5
                return 7;
            case PLATFORM_TV:   //8
                return 10;
            case PLATFORM_IPAD: //4
                return 5;
            default:        //其他未定义的全部转换成0
                return 0;
        }

        return 0;
    }


    //视频账户分号段(取模) ，useKey是qq vuid(前加V) guid(前加GUID)
    int SplitUserKeyByMod(const string &strUserKey, int iMod)
    {
        if(CStrUnits::StrIsNum(strUserKey)) //qq qq登录用户
        {
            return atoll(strUserKey.c_str()) % iMod;
        }
        else if(0 == strUserKey.find("v")) //vuseri wx登录用户
        {
            return atoll(strUserKey.substr(1).c_str()) % iMod;
        }
        else if(0 == strUserKey.find("GUID")) //guid 未登录用户
        {
            return CHashFunction::BKDRHash(strUserKey.substr(4)) % iMod;
        }

        return 0;
    }
};

// ================ class CVideoCommUI=============================================
class CVideoCommUI
{
public:
    static string GetTVideoActionUrlToPidDetail(const string &strPid, const string &strLiveType, const string &strStreamRatio)
    {
        //腾讯视频action跳转到直播底层页的url
        string strActionUrl = "txvideo://v.qq.com/TencentLiveActivity?pid="+strPid+"&isAutoPlay=1";
        if(0 == strLiveType.compare("fans"))
            strActionUrl += "&style=1";

        double dStreamRatio = 0; //默认认为是横屏的
        if(!strStreamRatio.empty()) //不为空时才取值
            dStreamRatio = atof(strStreamRatio.c_str());

        //当stream_ratio取值=0 || >1就是横屏 其余是竖屏，  streamStyle取值目前只有两个：0横屏，1竖屏
        if( dStreamRatio >= 1.0 || abs(dStreamRatio-0.0) < 0.00001 ) //横屏
            strActionUrl += "&streamStyle=0";
        else
            strActionUrl += "&streamStyle=1";

        return strActionUrl;
    }

    static string GetTVideoActionUrlToVideoDetail(const string &strLid, const string &strCid, const string &strVid)
    {
        return "txvideo://v.qq.com/VideoDetailActivity?lid="+strLid+"&cid="+strCid+"&vid="+strVid+"&isAutoPlay=1";
    }

    static string GetTLiveActionUrlToPidDetail(const string &strPid, const string &strLiveType, const string &strStreamRatio)
    {
        //直播app的跳转url
        string strActionUrl = "txlive://views/LiveDetailView?program_id=" + strPid + "&is_full_screen=1";
        if(0 == strLiveType.compare("fans"))
            strActionUrl += "&style=1";
        else
            strActionUrl += "&style=2";

        double dStreamRatio = 0; //默认认为是横屏的
        if(!strStreamRatio.empty()) //不为空时才取值
            dStreamRatio = atof(strStreamRatio.c_str());

        //当stream_ratio取值=0 || >1就是横屏 其余是竖屏，  streamStyle取值目前只有两个：0横屏，1竖屏
        if( dStreamRatio >= 1.0 || abs(dStreamRatio-0.0) < 0.00001 ) //横屏
            strActionUrl += "&stream_style=0";
        else
            strActionUrl += "&stream_style=1";

        return strActionUrl;
    }

    static string GetTLiveActionUrlToVideoDetail(const string &strLid, const string &strCid, const string &strVid,const string &strStreamRatio)
    {
        string strActionUrl = "txlive://views/VideoDetailView?lid="+strLid+"&cid="+strCid+"&vid="+strVid;

        double dStreamRatio = 0; //默认认为是横屏的
        if(!strStreamRatio.empty()) //不为空时才取值
            dStreamRatio = atof(strStreamRatio.c_str());

        //当stream_ratio取值=0 || >1就是横屏 其余是竖屏，  streamStyle取值目前只有两个：0横屏，1竖屏
        if( dStreamRatio >= 1.0 || abs(dStreamRatio-0.0) < 0.00001 ) //横屏
            strActionUrl += "&stream_style=0";
        else
            strActionUrl += "&stream_style=1";

        return strActionUrl;
    }

    static string GetStreamStyleFromStreamRatio(const string &strStreamRatio)
    {
        string strStreamStyle;

        double dStreamRatio = 0; //默认认为是横屏的
        if(!strStreamRatio.empty()) //不为空时才取值
            dStreamRatio = atof(strStreamRatio.c_str());

        //当stream_ratio取值=0 || >1就是横屏 其余是竖屏，  streamStyle取值目前只有两个：0横屏，1竖屏
        if( dStreamRatio >= 1.0 || abs(dStreamRatio-0.0) < 0.00001 ) //横屏
            strStreamStyle = "0";
        else
            strStreamStyle = "1";

        return strStreamStyle;
    }

    //评论数据时间显示的逻辑
    //明星最新一条主贴的发表时间，精确到分钟，格式为2017-07-14 14:34 ，若跨年则显示年份，不跨年则不显示年份。
    static string GetFeedDateTime(int64_t llFeedTime)
    {
        string strTime;
        string strFeedYear = CStrUnits::FormatTime(llFeedTime,"%Y");
        string strCurYear  = CStrUnits::FormatTime(time(NULL),"%Y");
        if(strFeedYear == strCurYear)
            strTime = CStrUnits::FormatTime(llFeedTime,"%m-%d %H:%M");
        else
            strTime = CStrUnits::FormatTime(llFeedTime,"%Y-%m-%d %H:%M");

        return strTime;
    }


    //发表时间展示规则为：
    // 1h内：XX分钟前
    // 24小时内：XX小时前
    // 1天前：昨天
    // 2天前：展示日期 例如：12-27
    // 本年内：展示日期 例如：12-27
    // 上年以前：展示日期 例如：2016-12-27

    /**
     * @brief  明星卡片时间显示的逻辑
     * @note   逻辑如下:
     *            0-1分钟内：刚刚
     *            1h内：XX分钟前
     *            24小时内：XX小时前
     *            1天前：昨天
     *            2天前：展示日期 例如：12-27
     *            本年内：展示日期 例如：12-27
     *            上年以前：展示日期 例如：2016-12-27
     * @param  llReqTimestamp:需要转换的时间戳，比当前时间戳要小
     * @retval
     */
    static string GetDateTimeForStarCard(int64_t llReqTimestamp)
    {
        int64_t llCurTimestamp = time(NULL);

        int64_t llTimeSpace = llCurTimestamp - llReqTimestamp;
        if(llTimeSpace < 0) //请求时间戳比当前时间还要大，直接返回空串
        {
            return "";
        }
        else if(llTimeSpace < 60) //0-1分钟内：刚刚
        {
            return "刚刚";
        }
        else if(llTimeSpace < 3600) //1h内：XX分钟前
        {
            return CNum2Str::Num2Str(llTimeSpace/60) + "分钟前";
        }
        else if(llTimeSpace < 86400)//24小时内：XX小时前
        {
            return CNum2Str::Num2Str(llTimeSpace/3600) + "小时前";
        }
        else if(llTimeSpace < 2*86400)//24-48小时间：属于昨天的显示“昨天”，不属于昨天的显示MM_DD
        {
            //获取昨天0点的时间戳
            int llTimeYesterday = (llCurTimestamp - 86400)/86400*86400 - 8*3600; //- 8*3600修正8时区

            if(llTimeYesterday > llReqTimestamp) //不属于昨天的显示MM_DD
                return CStrUnits::FormatTime(llReqTimestamp,"%m-%d");
            else
                return "昨天";
        }
        else //大于48小时的，本年内显示MM_DD，非本年内的显示YYYY-MM-DD
        {
            string strReqYear = CStrUnits::FormatTime(llReqTimestamp,"%Y");
            string strCurYear = CStrUnits::FormatTime(llCurTimestamp,"%Y");

            if(strReqYear == strCurYear) //本年内
                return CStrUnits::FormatTime(llReqTimestamp,"%m-%d");
            else
                return CStrUnits::FormatTime(llReqTimestamp,"%Y-%m-%d");
        }

        return "";
    }

    // 为0时不展示，小于10000时，显示具体数字，大于等于1万时，显示X.X万。
    // XX+strExtText; XX万+strExtText
    static string GetUserNum(int64_t llNum, const string &strExtText)
    {
        string strText;

        if (llNum >= 0 && llNum < 10000)
        {
            strText = CNum2Str::Num2Str(llNum) + strExtText;
        }
        else if (llNum >= 10000)
        {
            char buf[20] = {0};
            snprintf(buf, sizeof(buf), "%.1f", llNum / 10000.0);

            strText = string(buf) + "万" + strExtText;
        }

        return strText;
    }

};

// ================ class CUserLoginInfo=============================================
class CUserLoginInfo //用户登陆信息相关
{
public:

    //======videopacket相关======
    static int GetUserInfoFVPacket(int &iUserType, string &strUserId,CVideoPacket &stVideoPacket)
    {
        iUserType = LOGIN_FLAG_UNDEF;
        strUserId = "";

        bool bHasMainLogin = false;
        for(size_t i = 0; i < stVideoPacket.getLoginToken().size() ;i++)
        {
            videocomm::HLoginToken &oHLoginToken = stVideoPacket.getLoginToken()[i];

            //前端逻辑整理：qq登陆，会将qq强弱登录态的mainlogin设置为true; 微信登陆，将微信票据的mainlogin设置为true 内部票据设置为false
            if(oHLoginToken.bMainLogin && LOGIN_WEIXIN_TYPE == oHLoginToken.TokenKeyType)//若微信为主登陆态，需要取内部票据
            {
                bHasMainLogin = true;

                for(size_t vuserIndex = 0; vuserIndex < stVideoPacket.getLoginToken().size() ;vuserIndex++)
                {
                    videocomm::HLoginToken &oVuserToken = stVideoPacket.getLoginToken()[vuserIndex];
                    if(LOGIN_VUSER_TYPE == oVuserToken.TokenKeyType)
                    {
                        iUserType = LOGIN_FLAG_WX;
                        strUserId = CNum2Str::Num2Str(oVuserToken.TokenUin);
                        break;
                    }
                }

                break; //微信登陆态只判断内部票据，获取一次内部票据之后，直接跳出for循环
            }
            else if(oHLoginToken.bMainLogin && LOGIN_STRONG_TYPE == oHLoginToken.TokenKeyType ) //若qq强登录态带有主登陆态标志，则表示qq为主登陆态
            {
                bHasMainLogin = true;

                videocomm::HLoginToken &mainToken = stVideoPacket.getLoginToken()[i];

                iUserType = LOGIN_FLAG_QQ;
                strUserId = CNum2Str::Num2Str(mainToken.TokenUin);
                break;
            }
            else if(oHLoginToken.bMainLogin && LOGIN_WEAK_TYPE == oHLoginToken.TokenKeyType )//若qq弱登录态带有主登陆态标志，则表示qq为主登陆态
            {
                bHasMainLogin = true;

                videocomm::HLoginToken &mainToken = stVideoPacket.getLoginToken()[i];

                iUserType = LOGIN_FLAG_QQ;
                strUserId = CNum2Str::Num2Str(mainToken.TokenUin);
                break;
            }
        }

        //没有mainlogin且guid是合法的 才认为是guid登陆
        if(!bHasMainLogin && 32 == stVideoPacket.getGuid().length()) //guid必须是32位长度的
        {
            iUserType = LOGIN_FLAG_GUID;
            strUserId = stVideoPacket.getGuid();
        }

        return 0;
    }

    static int GetQqVuidGuidFVPacket(uint64_t &ullQq, uint64_t &ullVuserid, string &strGuid, CVideoPacket &stVideoPacket)
    {
        ullQq      = 0;
        ullVuserid = 0;
        strGuid    = "";

        bool bHasMainLogin = false;

        for(size_t i = 0; i < stVideoPacket.getLoginToken().size() ;i++)
        {
            videocomm::HLoginToken &oHLoginToken = stVideoPacket.getLoginToken()[i];

            //前端逻辑整理：qq登陆，会将qq强弱登录态的mainlogin设置为true; 微信登陆，将微信票据的mainlogin设置为true 内部票据设置为false
            if(oHLoginToken.bMainLogin && LOGIN_WEIXIN_TYPE == oHLoginToken.TokenKeyType)//若微信为主登陆态，需要取内部票据
            {
                bHasMainLogin = true;

                for(size_t vuserIndex = 0; vuserIndex < stVideoPacket.getLoginToken().size() ;vuserIndex++)
                {
                    videocomm::HLoginToken &oVuserToken = stVideoPacket.getLoginToken()[vuserIndex];
                    if(LOGIN_VUSER_TYPE == oVuserToken.TokenKeyType)
                    {
                        ullVuserid = oVuserToken.TokenUin;
                        break;
                    }
                }

                break; //微信登陆态只判断内部票据，获取一次内部票据之后，直接跳出for循环
            }
            else if(oHLoginToken.bMainLogin && LOGIN_STRONG_TYPE == oHLoginToken.TokenKeyType ) //若qq强登录态带有主登陆态标志，则表示qq为主登陆态
            {
                bHasMainLogin = true;

                videocomm::HLoginToken &mainToken = stVideoPacket.getLoginToken()[i];

                ullQq = mainToken.TokenUin;
                break;
            }
            else if(oHLoginToken.bMainLogin && LOGIN_WEAK_TYPE == oHLoginToken.TokenKeyType )//若qq弱登录态带有主登陆态标志，则表示qq为主登陆态
            {
                bHasMainLogin = true;

                videocomm::HLoginToken &mainToken = stVideoPacket.getLoginToken()[i];

                ullQq = mainToken.TokenUin;
                break;
            }
        }

        //没有mainlogin且guid是合法的 才认为是guid登陆
        if(!bHasMainLogin && 32 == stVideoPacket.getGuid().length()) //guid必须是32位长度的
        {
            strGuid = stVideoPacket.getGuid();
        }

        return 0;
    }
    
    // 非根据登录态获取 
    // 即如果主登录态为微信，当同时填写了QQ，则qq信息亦会返回
    static int GetQqVuidOpenidFVPacket(long &ullQq, long &ullVuserid, string &strOpenid, CVideoPacket &stVideoPacket)
    {
        ullQq = 0;
        ullVuserid = 0;

        bool bHasMainLogin = false;
        for (size_t i = 0; i < stVideoPacket.getLoginToken().size(); i++)
        {
            videocomm::HLoginToken &oHLoginToken = stVideoPacket.getLoginToken()[i];

            //前端逻辑整理：qq登陆，会将qq强弱登录态的mainlogin设置为true; 微信登陆，将微信票据的mainlogin设置为true 内部票据设置为false
            if (LOGIN_WEIXIN_TYPE == oHLoginToken.TokenKeyType)//若微信为主登陆态，需要取内部票据
            {
                videocomm::HLoginToken &mainToken = stVideoPacket.getLoginToken()[i];
                strOpenid = mainToken.TokenID;
            }
            else if (LOGIN_VUSER_TYPE == oHLoginToken.TokenKeyType)//若微信为主登陆态，需要取内部票据
            {
                videocomm::HLoginToken &mainToken = stVideoPacket.getLoginToken()[i];
                ullVuserid = mainToken.TokenUin;
            }
            else if (LOGIN_STRONG_TYPE == oHLoginToken.TokenKeyType) //若qq强登录态带有主登陆态标志，则表示qq为主登陆态
            {
                videocomm::HLoginToken &mainToken = stVideoPacket.getLoginToken()[i];
                ullQq = mainToken.TokenUin;
            }
            else if (LOGIN_WEAK_TYPE == oHLoginToken.TokenKeyType)//若qq弱登录态带有主登陆态标志，则表示qq为主登陆态
            {
                videocomm::HLoginToken &mainToken = stVideoPacket.getLoginToken()[i];
                ullQq = mainToken.TokenUin;
            }
        }

        return 0;
    }

    /**
     * [GetOmsTokenResult 获取接入层的校验结果]
     * @param  setTokenSucc   [接入层校验登陆成功的登陆类型]
     * @param  stVideoPacket  [videopacket包头]
     */
    static void GetOmsTokenResult(set<int> &setTokenSucc,CVideoPacket &stVideoPacket)
    {
        for(size_t i = 0; i < stVideoPacket.getLoginToken().size() ;i++)
        {
            videocomm::HLoginToken &oHLoginToken = stVideoPacket.getLoginToken()[i];

            if(0 == oHLoginToken.Result)
                setTokenSucc.insert(oHLoginToken.TokenKeyType);
        }

        return;
    }

    static bool JudgeMainLogin(CVideoPacket &stVideoPacket)
    {
        //判断是有有主登陆态
        bool bHasMainLogin = false;
        for(size_t uiIdx = 0; uiIdx < stVideoPacket.getLoginToken().size(); uiIdx++)
        {
            videocomm::HLoginToken &oHLoginToken = stVideoPacket.getLoginToken()[uiIdx];
            if(oHLoginToken.bMainLogin)//主登录态
            {
                bHasMainLogin = true;
                break;
            }
        }

        return bHasMainLogin;
    }

    //获取唯一的用户标志：qq登陆使用qq号，微信登陆使用前缀v+vuserid
    static string GetUserKeyFromVPacket(CVideoPacket &stVideoPacket)
    {
        uint64_t ullUin     = 0;
        uint64_t ullVuserid = 0;
        string   strGuid;
        CUserLoginInfo::GetQqVuidGuidFVPacket(ullUin, ullVuserid, strGuid, stVideoPacket);

        string strUserKey;
        if(ullUin > 1000)
            strUserKey = CNum2Str::Num2Str(ullUin);
        else if(ullVuserid > 0)
            strUserKey = "v" + CNum2Str::Num2Str(ullVuserid);
        else if(strGuid.size() == 32) //guid的长度是固定32个字符
            strUserKey = "GUID" + strGuid;
        else
            strUserKey = "";

        return strUserKey;
    }

    static int SetVPacketByUserKey(CVideoPacket &oOutPacket, const string &strInUserKey)
    {
        oOutPacket.getLoginToken().clear();

        size_t uiPos = strInUserKey.find("v");
        if( uiPos != string::npos)//wx登陆态
        {
            int64_t llUserid = atoll(strInUserKey.substr(uiPos+1).c_str());

            videocomm::HLoginToken token;
            token.TokenKeyType = 9;
            token.TokenUin     = llUserid;
            token.bMainLogin   = false;
            oOutPacket.setLoginToken(token);

            videocomm::HLoginToken token_wx;
            token_wx.TokenKeyType = 100;
            token_wx.bMainLogin   = true;
            oOutPacket.setLoginToken(token_wx);
        }
        else
        {
            int64_t llUin = atoll(strInUserKey.c_str());

            videocomm::HLoginToken token;
            token.TokenKeyType = 1;
            token.TokenUin     = llUin;
            token.bMainLogin   = true;
            oOutPacket.setLoginToken(token);
        }

        return 0;
    }

    //获取vuid
    static int64_t GetVuidFVPacket(CVideoPacket &stVideoPacket)
    {
        int64_t llVuserid = 0;

        for(size_t i = 0; i < stVideoPacket.getLoginToken().size() ;i++)
        {
            videocomm::HLoginToken &oHLoginToken = stVideoPacket.getLoginToken()[i];
            if(LOGIN_VUSER_TYPE == oHLoginToken.TokenKeyType)
            {
                llVuserid = oHLoginToken.TokenUin;
            }
        }

        return llVuserid;
    }

    //获取qq
    static int64_t GetQqFVPacket(CVideoPacket &stVideoPacket)
    {
        int64_t llQq = 0;

        for(size_t i = 0; i < stVideoPacket.getLoginToken().size() ;i++)
        {
            videocomm::HLoginToken &oHLoginToken = stVideoPacket.getLoginToken()[i];
            if(oHLoginToken.bMainLogin && LOGIN_STRONG_TYPE == oHLoginToken.TokenKeyType ) //若qq强登录态带有主登陆态标志，则表示qq为主登陆态
            {
                llQq = oHLoginToken.TokenUin;
                break;
            }
            else if(oHLoginToken.bMainLogin && LOGIN_WEAK_TYPE == oHLoginToken.TokenKeyType )//若qq弱登录态带有主登陆态标志，则表示qq为主登陆态
            {
                llQq = oHLoginToken.TokenUin;
                break;
            }
        }

        return llQq;
    }

    //======Cookie相关======
    static string GetUserKeyFromMapCookie(map<string,string> &mapCookie)
    {
        if(mapCookie.empty())
            return "";

        const string &strMainLogin = mapCookie["main_login"];

        if(strMainLogin.empty() || "qq" == strMainLogin) //qq登陆
        {
            if(!mapCookie["uin"].empty())
                return CSpeWork::GetStrUinFromCookie(mapCookie["uin"]);
            else if(!mapCookie["luin"].empty())
                return CSpeWork::GetStrUinFromCookie(mapCookie["luin"]);
        }
        else if("wx" == strMainLogin)
        {
            if(!mapCookie["vuserid"].empty())
                return "v" + mapCookie["vuserid"];
        }

        return "";
    }

    static string GetAllUserIdFromMapCookie(map<string,string> &mapCookie, const string &strExtData)
    {
        if(mapCookie.empty() && strExtData.empty())
        {
            return "";
        }

        stringstream ssTemp;

        const string &strMainLogin = mapCookie["main_login"];

        if(!strMainLogin.empty())
            ssTemp<<"main_login="<<strMainLogin<<"&";

        if(strMainLogin.empty() || "qq" == strMainLogin) //qq登陆
        {
            if(!mapCookie["uin"].empty())
            {
                ssTemp<<"uin="<<CSpeWork::GetStrUinFromCookie(mapCookie["uin"])<<"&"
                    <<"skey="<<mapCookie["skey"]<<"&";
            }

            if(!mapCookie["luin"].empty())
            {
                ssTemp<<"luin="<<CSpeWork::GetStrUinFromCookie(mapCookie["luin"])<<"&"
                    <<"lskey="<<mapCookie["lskey"]<<"&";
            }
        }
        else if("wx" == strMainLogin)
        {
            if(!mapCookie["vuserid"].empty())
            {
                ssTemp<<"vuserid="<<mapCookie["vuserid"]<<"&"
                    <<"vusession="<<mapCookie["vusession"]<<"&"
                    <<"appid="<<mapCookie["appid"]<<"&"
                    <<"openid="<<mapCookie["openid"]<<"&"
                    <<"access_token="<<mapCookie["access_token"]<<"&";
            }
        }

        if(!mapCookie["guid"].empty())
            ssTemp<<"guid="<<mapCookie["guid"]<<"&";

        if(!strExtData.empty())
            ssTemp<<strExtData;

        return ssTemp.str();
    }

    static void SetVideoPacketTokenByCookie(CVideoPacket &stVideoPacket, map<string, string> &mapReqCookie)
    {
        stVideoPacket.getLoginToken().clear();

        //从cookie中获取账号信息
        string strMainLogin = mapReqCookie["main_login"];
        if("qq" == strMainLogin || strMainLogin.empty())
        {
            //qq强登陆
            string strUin  = CSpeWork::GetStrUinFromCookie(mapReqCookie["uin"]);
            string strUKey = mapReqCookie["skey"];
            if(!strUin.empty() && !strUKey.empty())
            {
                videocomm::HLoginToken stToken;
                stToken.TokenUin     = atoll(strUin.c_str());
                stToken.TokenAppID   = "3000501";
                stToken.TokenValue   = strUKey;
                stToken.TokenKeyType = LOGIN_STRONG_TYPE;
                stToken.Result       = 1; //0 == 校验成功，1 == 未校验，2 == 校验超时，3 == 校验不通过
                stToken.bMainLogin   = true;

                stVideoPacket.setLoginToken(stToken);
            }

            //qq弱登陆
            strUin  = CSpeWork::GetStrUinFromCookie(mapReqCookie["luin"]);
            strUKey = mapReqCookie["lskey"];
            if(!strUin.empty() && !strUKey.empty())
            {
                videocomm::HLoginToken stToken;
                stToken.TokenUin     = atoll(strUin.c_str());
                stToken.TokenAppID   = "3000501";
                stToken.TokenValue   = strUKey;
                stToken.TokenKeyType = LOGIN_WEAK_TYPE;
                stToken.Result       = 1; //0 == 校验成功，1 == 未校验，2 == 校验超时，3 == 校验不通过
                stToken.bMainLogin   = true;

                stVideoPacket.setLoginToken(stToken);
            }
        }
        else if("wx" == strMainLogin)
        {
            string strOpenId      = mapReqCookie["openid"];
            string strAccessToken = mapReqCookie["access_token"];
            string strAppId       = mapReqCookie["appid"];
            if (!strOpenId.empty() && !strAccessToken.c_str() && !strAppId.empty())
            {
                videocomm::HLoginToken stToken;
                stToken.TokenUin     = 0;
                stToken.TokenAppID   = strAppId;
                stToken.TokenValue   = strAccessToken;
                stToken.TokenID      = strOpenId;
                stToken.TokenKeyType = LOGIN_WEIXIN_TYPE;
                stToken.Result       = 1; //0 == 校验成功，1 == 未校验，2 == 校验超时，3 == 校验不通过
                stToken.bMainLogin   = true;

                stVideoPacket.setLoginToken(stToken);
            }
        }

        //无论qq登陆还是wx登陆，统一带上vuserid的信息
        string strVuid      = mapReqCookie["vuserid"];
        string strVusession = mapReqCookie["vusession"];
        if(!strVuid.empty() && !strVusession.empty())
        {
            videocomm::HLoginToken stToken;
            stToken.TokenUin     = atoll(strVuid.c_str());
            stToken.TokenAppID   = "";
            stToken.TokenValue   = strVusession;
            stToken.TokenKeyType = LOGIN_VUSER_TYPE;
            stToken.Result       = 1; //0 == 校验成功，1 == 未校验，2 == 校验超时，3 == 校验不通过
            stToken.bMainLogin   = false;

            stVideoPacket.setLoginToken(stToken);
        }
    }


};


}

#endif  /*__MY_UTILS_TVIDEO_H__*/
