// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 3.1.1.2 by WSRD Tencent.
// Generated from `qqvideo_vrss.jce'
// **********************************************************************

#ifndef __QQVIDEO_VRSS_H_
#define __QQVIDEO_VRSS_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;
#include "qqvideo_common.h"


namespace jce
{
    struct ONAVRSSFeed : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.ONAVRSSFeed";
        }
        static string MD5()
        {
            return "ca8479981a2e29daeb57a4b261c2c146";
        }
        ONAVRSSFeed()
        :rankFlag("")
        {
        }
        void resetDefautlt()
        {
            rankFlag = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(rssItem, 0);
            _os.write(rankFlag, 1);
            _os.write(posterList, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(rssItem, 0, true);
            _is.read(rankFlag, 1, false);
            _is.read(posterList, 2, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(rssItem,"rssItem");
            _ds.display(rankFlag,"rankFlag");
            _ds.display(posterList,"posterList");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(rssItem, true);
            _ds.displaySimple(rankFlag, true);
            _ds.displaySimple(posterList, false);
            return _os;
        }
    public:
        jce::VRSSItem rssItem;
        std::string rankFlag;
        vector<jce::Poster> posterList;
    };
    inline bool operator==(const ONAVRSSFeed&l, const ONAVRSSFeed&r)
    {
        return l.rssItem == r.rssItem && l.rankFlag == r.rankFlag && l.posterList == r.posterList;
    }
    inline bool operator!=(const ONAVRSSFeed&l, const ONAVRSSFeed&r)
    {
        return !(l == r);
    }

    struct ONAVRSSGroup : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.ONAVRSSGroup";
        }
        static string MD5()
        {
            return "44ec9145cb036a6ed65d48767e54c303";
        }
        ONAVRSSGroup()
        :groupTitle(""),reportParams(""),subTitle(""),reportKey("")
        {
        }
        void resetDefautlt()
        {
            groupTitle = "";
            reportParams = "";
            subTitle = "";
            reportKey = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(rssFeeds, 0);
            _os.write(groupTitle, 1);
            _os.write(reportParams, 2);
            _os.write(markLabelList, 3);
            _os.write(titleAction, 4);
            _os.write(subTitle, 5);
            _os.write(reportKey, 6);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(rssFeeds, 0, true);
            _is.read(groupTitle, 1, false);
            _is.read(reportParams, 2, false);
            _is.read(markLabelList, 3, false);
            _is.read(titleAction, 4, false);
            _is.read(subTitle, 5, false);
            _is.read(reportKey, 6, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(rssFeeds,"rssFeeds");
            _ds.display(groupTitle,"groupTitle");
            _ds.display(reportParams,"reportParams");
            _ds.display(markLabelList,"markLabelList");
            _ds.display(titleAction,"titleAction");
            _ds.display(subTitle,"subTitle");
            _ds.display(reportKey,"reportKey");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(rssFeeds, true);
            _ds.displaySimple(groupTitle, true);
            _ds.displaySimple(reportParams, true);
            _ds.displaySimple(markLabelList, true);
            _ds.displaySimple(titleAction, true);
            _ds.displaySimple(subTitle, true);
            _ds.displaySimple(reportKey, false);
            return _os;
        }
    public:
        vector<jce::ONAVRSSFeed> rssFeeds;
        std::string groupTitle;
        std::string reportParams;
        vector<jce::MarkLabel> markLabelList;
        jce::Action titleAction;
        std::string subTitle;
        std::string reportKey;
    };
    inline bool operator==(const ONAVRSSGroup&l, const ONAVRSSGroup&r)
    {
        return l.rssFeeds == r.rssFeeds && l.groupTitle == r.groupTitle && l.reportParams == r.reportParams && l.markLabelList == r.markLabelList && l.titleAction == r.titleAction && l.subTitle == r.subTitle && l.reportKey == r.reportKey;
    }
    inline bool operator!=(const ONAVRSSGroup&l, const ONAVRSSGroup&r)
    {
        return !(l == r);
    }

    struct VRSSHomeRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.VRSSHomeRequest";
        }
        static string MD5()
        {
            return "8d61b639cda77ea57102c6b9cbb5aca2";
        }
        VRSSHomeRequest()
        :rssId(""),pageContext(""),requestType(0)
        {
        }
        void resetDefautlt()
        {
            rssId = "";
            pageContext = "";
            requestType = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(rssId, 0);
            _os.write(pageContext, 1);
            _os.write(requestType, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(rssId, 0, true);
            _is.read(pageContext, 1, false);
            _is.read(requestType, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(rssId,"rssId");
            _ds.display(pageContext,"pageContext");
            _ds.display(requestType,"requestType");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(rssId, true);
            _ds.displaySimple(pageContext, true);
            _ds.displaySimple(requestType, false);
            return _os;
        }
    public:
        std::string rssId;
        std::string pageContext;
        taf::Int32 requestType;
    };
    inline bool operator==(const VRSSHomeRequest&l, const VRSSHomeRequest&r)
    {
        return l.rssId == r.rssId && l.pageContext == r.pageContext && l.requestType == r.requestType;
    }
    inline bool operator!=(const VRSSHomeRequest&l, const VRSSHomeRequest&r)
    {
        return !(l == r);
    }

    struct VRSSHomeResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.VRSSHomeResponse";
        }
        static string MD5()
        {
            return "2d432e2dda66d3835d9746ffc029d71b";
        }
        VRSSHomeResponse()
        :errCode(0),pageContext(""),isHaveNextPage(true),rollIndex(0)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            pageContext = "";
            isHaveNextPage = true;
            rollIndex = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(rssItem, 1);
            _os.write(uiData, 2);
            _os.write(coverDataMap, 3);
            _os.write(actorDataMap, 4);
            _os.write(shareItem, 5);
            _os.write(pageContext, 6);
            _os.write(isHaveNextPage, 7);
            _os.write(tabModuleList, 8);
            _os.write(rollIndex, 9);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(rssItem, 1, false);
            _is.read(uiData, 2, false);
            _is.read(coverDataMap, 3, false);
            _is.read(actorDataMap, 4, false);
            _is.read(shareItem, 5, false);
            _is.read(pageContext, 6, false);
            _is.read(isHaveNextPage, 7, false);
            _is.read(tabModuleList, 8, false);
            _is.read(rollIndex, 9, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(rssItem,"rssItem");
            _ds.display(uiData,"uiData");
            _ds.display(coverDataMap,"coverDataMap");
            _ds.display(actorDataMap,"actorDataMap");
            _ds.display(shareItem,"shareItem");
            _ds.display(pageContext,"pageContext");
            _ds.display(isHaveNextPage,"isHaveNextPage");
            _ds.display(tabModuleList,"tabModuleList");
            _ds.display(rollIndex,"rollIndex");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(rssItem, true);
            _ds.displaySimple(uiData, true);
            _ds.displaySimple(coverDataMap, true);
            _ds.displaySimple(actorDataMap, true);
            _ds.displaySimple(shareItem, true);
            _ds.displaySimple(pageContext, true);
            _ds.displaySimple(isHaveNextPage, true);
            _ds.displaySimple(tabModuleList, true);
            _ds.displaySimple(rollIndex, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        jce::VRSSItem rssItem;
        vector<jce::TempletLine> uiData;
        map<std::string, jce::CoverDataList> coverDataMap;
        map<std::string, jce::ActorList> actorDataMap;
        jce::ShareItem shareItem;
        std::string pageContext;
        taf::Bool isHaveNextPage;
        vector<jce::LiveTabModuleInfo> tabModuleList;
        taf::Int32 rollIndex;
    };
    inline bool operator==(const VRSSHomeResponse&l, const VRSSHomeResponse&r)
    {
        return l.errCode == r.errCode && l.rssItem == r.rssItem && l.uiData == r.uiData && l.coverDataMap == r.coverDataMap && l.actorDataMap == r.actorDataMap && l.shareItem == r.shareItem && l.pageContext == r.pageContext && l.isHaveNextPage == r.isHaveNextPage && l.tabModuleList == r.tabModuleList && l.rollIndex == r.rollIndex;
    }
    inline bool operator!=(const VRSSHomeResponse&l, const VRSSHomeResponse&r)
    {
        return !(l == r);
    }

    struct VRssSubscribeRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.VRssSubscribeRequest";
        }
        static string MD5()
        {
            return "b74c0ec0c5edbbf0b58b80ed227666ee";
        }
        VRssSubscribeRequest()
        :option(0)
        {
        }
        void resetDefautlt()
        {
            option = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(option, 0);
            _os.write(vRssFeed, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(option, 0, true);
            _is.read(vRssFeed, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(option,"option");
            _ds.display(vRssFeed,"vRssFeed");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(option, true);
            _ds.displaySimple(vRssFeed, false);
            return _os;
        }
    public:
        taf::Char option;
        jce::ONAVRSSFeed vRssFeed;
    };
    inline bool operator==(const VRssSubscribeRequest&l, const VRssSubscribeRequest&r)
    {
        return l.option == r.option && l.vRssFeed == r.vRssFeed;
    }
    inline bool operator!=(const VRssSubscribeRequest&l, const VRssSubscribeRequest&r)
    {
        return !(l == r);
    }

    struct VRssSubscribeResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.VRssSubscribeResponse";
        }
        static string MD5()
        {
            return "7970ef8b17095f3f4395dd985afb97df";
        }
        VRssSubscribeResponse()
        :retCode(0)
        {
        }
        void resetDefautlt()
        {
            retCode = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(retCode, 0);
            _os.write(vRssFeed, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(retCode, 0, true);
            _is.read(vRssFeed, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(retCode,"retCode");
            _ds.display(vRssFeed,"vRssFeed");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(retCode, true);
            _ds.displaySimple(vRssFeed, false);
            return _os;
        }
    public:
        taf::Int32 retCode;
        jce::ONAVRSSFeed vRssFeed;
    };
    inline bool operator==(const VRssSubscribeResponse&l, const VRssSubscribeResponse&r)
    {
        return l.retCode == r.retCode && l.vRssFeed == r.vRssFeed;
    }
    inline bool operator!=(const VRssSubscribeResponse&l, const VRssSubscribeResponse&r)
    {
        return !(l == r);
    }

    struct GetVRssSubscribeRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.GetVRssSubscribeRequest";
        }
        static string MD5()
        {
            return "59ec633a0479e0c47e29ee571b0dc24d";
        }
        GetVRssSubscribeRequest()
        :dataVersionReq(0),dataVersionAccount(0),dataVersionVideo(0)
        {
        }
        void resetDefautlt()
        {
            dataVersionReq = 0;
            dataVersionAccount = 0;
            dataVersionVideo = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(dataVersionReq, 0);
            _os.write(dataVersionAccount, 1);
            _os.write(dataVersionVideo, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(dataVersionReq, 0, false);
            _is.read(dataVersionAccount, 1, false);
            _is.read(dataVersionVideo, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(dataVersionReq,"dataVersionReq");
            _ds.display(dataVersionAccount,"dataVersionAccount");
            _ds.display(dataVersionVideo,"dataVersionVideo");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(dataVersionReq, true);
            _ds.displaySimple(dataVersionAccount, true);
            _ds.displaySimple(dataVersionVideo, false);
            return _os;
        }
    public:
        taf::Int64 dataVersionReq;
        taf::Int64 dataVersionAccount;
        taf::Int64 dataVersionVideo;
    };
    inline bool operator==(const GetVRssSubscribeRequest&l, const GetVRssSubscribeRequest&r)
    {
        return l.dataVersionReq == r.dataVersionReq && l.dataVersionAccount == r.dataVersionAccount && l.dataVersionVideo == r.dataVersionVideo;
    }
    inline bool operator!=(const GetVRssSubscribeRequest&l, const GetVRssSubscribeRequest&r)
    {
        return !(l == r);
    }

    struct GetVRssSubscribeResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.GetVRssSubscribeResponse";
        }
        static string MD5()
        {
            return "627270e57f1b849143a0ad79a58e576c";
        }
        GetVRssSubscribeResponse()
        :retCode(0),dataVersionReq(0),dataVersionAccount(0),dataVersionVideo(0),updateFlag(0)
        {
        }
        void resetDefautlt()
        {
            retCode = 0;
            dataVersionReq = 0;
            dataVersionAccount = 0;
            dataVersionVideo = 0;
            updateFlag = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(retCode, 0);
            _os.write(dataVersionReq, 1);
            _os.write(dataVersionAccount, 2);
            _os.write(dataVersionVideo, 3);
            _os.write(updateFlag, 4);
            _os.write(gvRssFeedList, 5);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(retCode, 0, true);
            _is.read(dataVersionReq, 1, false);
            _is.read(dataVersionAccount, 2, false);
            _is.read(dataVersionVideo, 3, false);
            _is.read(updateFlag, 4, false);
            _is.read(gvRssFeedList, 5, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(retCode,"retCode");
            _ds.display(dataVersionReq,"dataVersionReq");
            _ds.display(dataVersionAccount,"dataVersionAccount");
            _ds.display(dataVersionVideo,"dataVersionVideo");
            _ds.display(updateFlag,"updateFlag");
            _ds.display(gvRssFeedList,"gvRssFeedList");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(retCode, true);
            _ds.displaySimple(dataVersionReq, true);
            _ds.displaySimple(dataVersionAccount, true);
            _ds.displaySimple(dataVersionVideo, true);
            _ds.displaySimple(updateFlag, true);
            _ds.displaySimple(gvRssFeedList, false);
            return _os;
        }
    public:
        taf::Int32 retCode;
        taf::Int64 dataVersionReq;
        taf::Int64 dataVersionAccount;
        taf::Int64 dataVersionVideo;
        taf::Int32 updateFlag;
        vector<jce::ONAVRSSFeed> gvRssFeedList;
    };
    inline bool operator==(const GetVRssSubscribeResponse&l, const GetVRssSubscribeResponse&r)
    {
        return l.retCode == r.retCode && l.dataVersionReq == r.dataVersionReq && l.dataVersionAccount == r.dataVersionAccount && l.dataVersionVideo == r.dataVersionVideo && l.updateFlag == r.updateFlag && l.gvRssFeedList == r.gvRssFeedList;
    }
    inline bool operator!=(const GetVRssSubscribeResponse&l, const GetVRssSubscribeResponse&r)
    {
        return !(l == r);
    }

    struct ONAVRSSInfoFeed : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.ONAVRSSInfoFeed";
        }
        static string MD5()
        {
            return "7970ef8b17095f3f4395dd985afb97df";
        }
        ONAVRSSInfoFeed()
        :uiType(0)
        {
        }
        void resetDefautlt()
        {
            uiType = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(uiType, 0);
            _os.write(feed, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(uiType, 0, true);
            _is.read(feed, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(uiType,"uiType");
            _ds.display(feed,"feed");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(uiType, true);
            _ds.displaySimple(feed, false);
            return _os;
        }
    public:
        taf::Int32 uiType;
        jce::ONAVRSSFeed feed;
    };
    inline bool operator==(const ONAVRSSInfoFeed&l, const ONAVRSSInfoFeed&r)
    {
        return l.uiType == r.uiType && l.feed == r.feed;
    }
    inline bool operator!=(const ONAVRSSInfoFeed&l, const ONAVRSSInfoFeed&r)
    {
        return !(l == r);
    }


}

#define jce_ONAVRSSFeed_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.rssItem,b.rssItem);jce_copy_struct(a.rankFlag,b.rankFlag);jce_copy_struct(a.posterList,b.posterList);

#define jce_ONAVRSSGroup_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.rssFeeds,b.rssFeeds);jce_copy_struct(a.groupTitle,b.groupTitle);jce_copy_struct(a.reportParams,b.reportParams);jce_copy_struct(a.markLabelList,b.markLabelList);jce_copy_struct(a.titleAction,b.titleAction);jce_copy_struct(a.subTitle,b.subTitle);jce_copy_struct(a.reportKey,b.reportKey);

#define jce_VRSSHomeRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.rssId,b.rssId);jce_copy_struct(a.pageContext,b.pageContext);jce_copy_struct(a.requestType,b.requestType);

#define jce_VRSSHomeResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.rssItem,b.rssItem);jce_copy_struct(a.uiData,b.uiData);jce_copy_struct(a.coverDataMap,b.coverDataMap);jce_copy_struct(a.actorDataMap,b.actorDataMap);jce_copy_struct(a.shareItem,b.shareItem);jce_copy_struct(a.pageContext,b.pageContext);jce_copy_struct(a.isHaveNextPage,b.isHaveNextPage);jce_copy_struct(a.tabModuleList,b.tabModuleList);jce_copy_struct(a.rollIndex,b.rollIndex);

#define jce_VRssSubscribeRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.option,b.option);jce_copy_struct(a.vRssFeed,b.vRssFeed);

#define jce_VRssSubscribeResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.retCode,b.retCode);jce_copy_struct(a.vRssFeed,b.vRssFeed);

#define jce_GetVRssSubscribeRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.dataVersionReq,b.dataVersionReq);jce_copy_struct(a.dataVersionAccount,b.dataVersionAccount);jce_copy_struct(a.dataVersionVideo,b.dataVersionVideo);

#define jce_GetVRssSubscribeResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.retCode,b.retCode);jce_copy_struct(a.dataVersionReq,b.dataVersionReq);jce_copy_struct(a.dataVersionAccount,b.dataVersionAccount);jce_copy_struct(a.dataVersionVideo,b.dataVersionVideo);jce_copy_struct(a.updateFlag,b.updateFlag);jce_copy_struct(a.gvRssFeedList,b.gvRssFeedList);

#define jce_ONAVRSSInfoFeed_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.uiType,b.uiType);jce_copy_struct(a.feed,b.feed);



#endif
