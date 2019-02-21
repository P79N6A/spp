// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 3.1.1.2 by WSRD Tencent.
// Generated from `qqvideo_fans_live_rank.jce'
// **********************************************************************

#ifndef __QQVIDEO_FANS_LIVE_RANK_H_
#define __QQVIDEO_FANS_LIVE_RANK_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;
#include "qqvideo_common.h"


namespace jce
{
    struct AnchorRankTabRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.AnchorRankTabRequest";
        }
        static string MD5()
        {
            return "325d87d477a8cf7a6468ed6bb39da964";
        }
        AnchorRankTabRequest()
        :type(""),dataKey("")
        {
        }
        void resetDefautlt()
        {
            type = "";
            dataKey = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(type, 0);
            _os.write(dataKey, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(type, 0, false);
            _is.read(dataKey, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(type,"type");
            _ds.display(dataKey,"dataKey");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(type, true);
            _ds.displaySimple(dataKey, false);
            return _os;
        }
    public:
        std::string type;
        std::string dataKey;
    };
    inline bool operator==(const AnchorRankTabRequest&l, const AnchorRankTabRequest&r)
    {
        return l.type == r.type && l.dataKey == r.dataKey;
    }
    inline bool operator!=(const AnchorRankTabRequest&l, const AnchorRankTabRequest&r)
    {
        return !(l == r);
    }

    struct AnchorRankTabResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.AnchorRankTabResponse";
        }
        static string MD5()
        {
            return "060d6a2200dbe92f1e403f8ede0aed3c";
        }
        AnchorRankTabResponse()
        :errCode(0),unit(""),desc("")
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            unit = "";
            desc = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(moduleList, 1);
            _os.write(shareItem, 2);
            _os.write(optFlags, 3);
            _os.write(unit, 4);
            _os.write(desc, 5);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(moduleList, 1, true);
            _is.read(shareItem, 2, false);
            _is.read(optFlags, 3, false);
            _is.read(unit, 4, false);
            _is.read(desc, 5, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(moduleList,"moduleList");
            _ds.display(shareItem,"shareItem");
            _ds.display(optFlags,"optFlags");
            _ds.display(unit,"unit");
            _ds.display(desc,"desc");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(moduleList, true);
            _ds.displaySimple(shareItem, true);
            _ds.displaySimple(optFlags, true);
            _ds.displaySimple(unit, true);
            _ds.displaySimple(desc, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        vector<jce::LiveTabModuleInfo> moduleList;
        jce::ShareItem shareItem;
        vector<std::string> optFlags;
        std::string unit;
        std::string desc;
    };
    inline bool operator==(const AnchorRankTabResponse&l, const AnchorRankTabResponse&r)
    {
        return l.errCode == r.errCode && l.moduleList == r.moduleList && l.shareItem == r.shareItem && l.optFlags == r.optFlags && l.unit == r.unit && l.desc == r.desc;
    }
    inline bool operator!=(const AnchorRankTabResponse&l, const AnchorRankTabResponse&r)
    {
        return !(l == r);
    }

    struct AnchorRankRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.AnchorRankRequest";
        }
        static string MD5()
        {
            return "325d87d477a8cf7a6468ed6bb39da964";
        }
        AnchorRankRequest()
        :dataKey(""),pageContext("")
        {
        }
        void resetDefautlt()
        {
            dataKey = "";
            pageContext = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(dataKey, 0);
            _os.write(pageContext, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(dataKey, 0, true);
            _is.read(pageContext, 1, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(dataKey,"dataKey");
            _ds.display(pageContext,"pageContext");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(dataKey, true);
            _ds.displaySimple(pageContext, false);
            return _os;
        }
    public:
        std::string dataKey;
        std::string pageContext;
    };
    inline bool operator==(const AnchorRankRequest&l, const AnchorRankRequest&r)
    {
        return l.dataKey == r.dataKey && l.pageContext == r.pageContext;
    }
    inline bool operator!=(const AnchorRankRequest&l, const AnchorRankRequest&r)
    {
        return !(l == r);
    }

    struct AnchorRankResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.AnchorRankResponse";
        }
        static string MD5()
        {
            return "3c6c1a65ebc80a868263949502ad6a59";
        }
        AnchorRankResponse()
        :errCode(0),errMsg(""),propsDataKey(""),totalAnchorCount(0),pageContext(""),isHaveNextPage(true)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            errMsg = "";
            propsDataKey = "";
            totalAnchorCount = 0;
            pageContext = "";
            isHaveNextPage = true;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(errMsg, 1);
            _os.write(anchorList, 2);
            _os.write(propsDataKey, 3);
            _os.write(totalAnchorCount, 4);
            _os.write(pageContext, 5);
            _os.write(isHaveNextPage, 6);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(errMsg, 1, false);
            _is.read(anchorList, 2, true);
            _is.read(propsDataKey, 3, false);
            _is.read(totalAnchorCount, 4, false);
            _is.read(pageContext, 5, false);
            _is.read(isHaveNextPage, 6, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(errMsg,"errMsg");
            _ds.display(anchorList,"anchorList");
            _ds.display(propsDataKey,"propsDataKey");
            _ds.display(totalAnchorCount,"totalAnchorCount");
            _ds.display(pageContext,"pageContext");
            _ds.display(isHaveNextPage,"isHaveNextPage");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(errMsg, true);
            _ds.displaySimple(anchorList, true);
            _ds.displaySimple(propsDataKey, true);
            _ds.displaySimple(totalAnchorCount, true);
            _ds.displaySimple(pageContext, true);
            _ds.displaySimple(isHaveNextPage, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        std::string errMsg;
        vector<jce::ActorInfo> anchorList;
        std::string propsDataKey;
        taf::Int32 totalAnchorCount;
        std::string pageContext;
        taf::Bool isHaveNextPage;
    };
    inline bool operator==(const AnchorRankResponse&l, const AnchorRankResponse&r)
    {
        return l.errCode == r.errCode && l.errMsg == r.errMsg && l.anchorList == r.anchorList && l.propsDataKey == r.propsDataKey && l.totalAnchorCount == r.totalAnchorCount && l.pageContext == r.pageContext && l.isHaveNextPage == r.isHaveNextPage;
    }
    inline bool operator!=(const AnchorRankResponse&l, const AnchorRankResponse&r)
    {
        return !(l == r);
    }

    struct AnchorFansItem : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.AnchorFansItem";
        }
        static string MD5()
        {
            return "25adb28543c1b6d0174a8f31c463d0e2";
        }
        AnchorFansItem()
        :rankNum(0),userType(0),userID(""),giftNum(0),nick(""),headUrl(""),giftImgUrl(""),uiType(0)
        {
        }
        void resetDefautlt()
        {
            rankNum = 0;
            userType = 0;
            userID = "";
            giftNum = 0;
            nick = "";
            headUrl = "";
            giftImgUrl = "";
            uiType = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(rankNum, 0);
            _os.write(userType, 1);
            _os.write(userID, 2);
            _os.write(giftNum, 3);
            _os.write(nick, 4);
            _os.write(headUrl, 5);
            _os.write(giftImgUrl, 6);
            _os.write(action, 7);
            _os.write(uiType, 8);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(rankNum, 0, true);
            _is.read(userType, 1, true);
            _is.read(userID, 2, true);
            _is.read(giftNum, 3, true);
            _is.read(nick, 4, false);
            _is.read(headUrl, 5, false);
            _is.read(giftImgUrl, 6, false);
            _is.read(action, 7, false);
            _is.read(uiType, 8, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(rankNum,"rankNum");
            _ds.display(userType,"userType");
            _ds.display(userID,"userID");
            _ds.display(giftNum,"giftNum");
            _ds.display(nick,"nick");
            _ds.display(headUrl,"headUrl");
            _ds.display(giftImgUrl,"giftImgUrl");
            _ds.display(action,"action");
            _ds.display(uiType,"uiType");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(rankNum, true);
            _ds.displaySimple(userType, true);
            _ds.displaySimple(userID, true);
            _ds.displaySimple(giftNum, true);
            _ds.displaySimple(nick, true);
            _ds.displaySimple(headUrl, true);
            _ds.displaySimple(giftImgUrl, true);
            _ds.displaySimple(action, true);
            _ds.displaySimple(uiType, false);
            return _os;
        }
    public:
        taf::Int32 rankNum;
        taf::Int32 userType;
        std::string userID;
        taf::Int32 giftNum;
        std::string nick;
        std::string headUrl;
        std::string giftImgUrl;
        jce::Action action;
        taf::Int32 uiType;
    };
    inline bool operator==(const AnchorFansItem&l, const AnchorFansItem&r)
    {
        return l.rankNum == r.rankNum && l.userType == r.userType && l.userID == r.userID && l.giftNum == r.giftNum && l.nick == r.nick && l.headUrl == r.headUrl && l.giftImgUrl == r.giftImgUrl && l.action == r.action && l.uiType == r.uiType;
    }
    inline bool operator!=(const AnchorFansItem&l, const AnchorFansItem&r)
    {
        return !(l == r);
    }

    struct AnchorFansRankRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.AnchorFansRankRequest";
        }
        static string MD5()
        {
            return "0d2ce2bfc4eeee1ad30063b5f17a7f57";
        }
        AnchorFansRankRequest()
        :dataKey(""),topCount(0)
        {
        }
        void resetDefautlt()
        {
            dataKey = "";
            topCount = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(dataKey, 0);
            _os.write(topCount, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(dataKey, 0, true);
            _is.read(topCount, 1, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(dataKey,"dataKey");
            _ds.display(topCount,"topCount");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(dataKey, true);
            _ds.displaySimple(topCount, false);
            return _os;
        }
    public:
        std::string dataKey;
        taf::Int32 topCount;
    };
    inline bool operator==(const AnchorFansRankRequest&l, const AnchorFansRankRequest&r)
    {
        return l.dataKey == r.dataKey && l.topCount == r.topCount;
    }
    inline bool operator!=(const AnchorFansRankRequest&l, const AnchorFansRankRequest&r)
    {
        return !(l == r);
    }

    struct AnchorFansRankResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.AnchorFansRankResponse";
        }
        static string MD5()
        {
            return "2cbee03259a113d63daa9f672f0d314e";
        }
        AnchorFansRankResponse()
        :errCode(0),errMsg("")
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            errMsg = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(errMsg, 1);
            _os.write(items, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(errMsg, 1, false);
            _is.read(items, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(errMsg,"errMsg");
            _ds.display(items,"items");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(errMsg, true);
            _ds.displaySimple(items, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        std::string errMsg;
        vector<jce::AnchorFansItem> items;
    };
    inline bool operator==(const AnchorFansRankResponse&l, const AnchorFansRankResponse&r)
    {
        return l.errCode == r.errCode && l.errMsg == r.errMsg && l.items == r.items;
    }
    inline bool operator!=(const AnchorFansRankResponse&l, const AnchorFansRankResponse&r)
    {
        return !(l == r);
    }


}

#define jce_AnchorRankTabRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.type,b.type);jce_copy_struct(a.dataKey,b.dataKey);

#define jce_AnchorRankTabResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.moduleList,b.moduleList);jce_copy_struct(a.shareItem,b.shareItem);jce_copy_struct(a.optFlags,b.optFlags);jce_copy_struct(a.unit,b.unit);jce_copy_struct(a.desc,b.desc);

#define jce_AnchorRankRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.dataKey,b.dataKey);jce_copy_struct(a.pageContext,b.pageContext);

#define jce_AnchorRankResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.errMsg,b.errMsg);jce_copy_struct(a.anchorList,b.anchorList);jce_copy_struct(a.propsDataKey,b.propsDataKey);jce_copy_struct(a.totalAnchorCount,b.totalAnchorCount);jce_copy_struct(a.pageContext,b.pageContext);jce_copy_struct(a.isHaveNextPage,b.isHaveNextPage);

#define jce_AnchorFansItem_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.rankNum,b.rankNum);jce_copy_struct(a.userType,b.userType);jce_copy_struct(a.userID,b.userID);jce_copy_struct(a.giftNum,b.giftNum);jce_copy_struct(a.nick,b.nick);jce_copy_struct(a.headUrl,b.headUrl);jce_copy_struct(a.giftImgUrl,b.giftImgUrl);jce_copy_struct(a.action,b.action);jce_copy_struct(a.uiType,b.uiType);

#define jce_AnchorFansRankRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.dataKey,b.dataKey);jce_copy_struct(a.topCount,b.topCount);

#define jce_AnchorFansRankResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.errMsg,b.errMsg);jce_copy_struct(a.items,b.items);



#endif