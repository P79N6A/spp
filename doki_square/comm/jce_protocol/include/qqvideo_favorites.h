// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 3.1.1.2 by WSRD Tencent.
// Generated from `qqvideo_favorites.jce'
// **********************************************************************

#ifndef __QQVIDEO_FAVORITES_H_
#define __QQVIDEO_FAVORITES_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;
#include "qqvideo_common.h"


namespace jce
{
    struct FavoritesTopBarItem : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesTopBarItem";
        }
        static string MD5()
        {
            return "3902a9008ef0d1036dabf9e3f6c2ba7d";
        }
        FavoritesTopBarItem()
        :type(0),dataKey(""),title("")
        {
        }
        void resetDefautlt()
        {
            type = 0;
            dataKey = "";
            title = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(type, 0);
            _os.write(dataKey, 1);
            _os.write(title, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(type, 0, false);
            _is.read(dataKey, 1, false);
            _is.read(title, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(type,"type");
            _ds.display(dataKey,"dataKey");
            _ds.display(title,"title");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(type, true);
            _ds.displaySimple(dataKey, true);
            _ds.displaySimple(title, false);
            return _os;
        }
    public:
        taf::Char type;
        std::string dataKey;
        std::string title;
    };
    inline bool operator==(const FavoritesTopBarItem&l, const FavoritesTopBarItem&r)
    {
        return l.type == r.type && l.dataKey == r.dataKey && l.title == r.title;
    }
    inline bool operator!=(const FavoritesTopBarItem&l, const FavoritesTopBarItem&r)
    {
        return !(l == r);
    }

    struct FavoritesTopBarRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesTopBarRequest";
        }
        static string MD5()
        {
            return "2e45701425f70f0e5c722cbe2c3f508e";
        }
        FavoritesTopBarRequest()
        :userId("")
        {
        }
        void resetDefautlt()
        {
            userId = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(userId, 0);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(userId, 0, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(userId,"userId");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(userId, false);
            return _os;
        }
    public:
        std::string userId;
    };
    inline bool operator==(const FavoritesTopBarRequest&l, const FavoritesTopBarRequest&r)
    {
        return l.userId == r.userId;
    }
    inline bool operator!=(const FavoritesTopBarRequest&l, const FavoritesTopBarRequest&r)
    {
        return !(l == r);
    }

    struct FavoritesTopBarResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesTopBarResponse";
        }
        static string MD5()
        {
            return "35d86aaba9684d1d506beb76c9a2928f";
        }
        FavoritesTopBarResponse()
        :errCode(0)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(itemList, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(itemList, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(itemList,"itemList");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(itemList, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        vector<jce::FavoritesTopBarItem> itemList;
    };
    inline bool operator==(const FavoritesTopBarResponse&l, const FavoritesTopBarResponse&r)
    {
        return l.errCode == r.errCode && l.itemList == r.itemList;
    }
    inline bool operator!=(const FavoritesTopBarResponse&l, const FavoritesTopBarResponse&r)
    {
        return !(l == r);
    }

    struct FavoritesQAItem : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesQAItem";
        }
        static string MD5()
        {
            return "11edce0134db30e66b0eaf58c3aabb76";
        }
        FavoritesQAItem()
        :favoritesTime(0)
        {
        }
        void resetDefautlt()
        {
            favoritesTime = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(favoritesTime, 0);
            _os.write(qaPrimaryFeed, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(favoritesTime, 0, false);
            _is.read(qaPrimaryFeed, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(favoritesTime,"favoritesTime");
            _ds.display(qaPrimaryFeed,"qaPrimaryFeed");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(favoritesTime, true);
            _ds.displaySimple(qaPrimaryFeed, false);
            return _os;
        }
    public:
        taf::Int64 favoritesTime;
        jce::QAPrimaryFeed qaPrimaryFeed;
    };
    inline bool operator==(const FavoritesQAItem&l, const FavoritesQAItem&r)
    {
        return l.favoritesTime == r.favoritesTime && l.qaPrimaryFeed == r.qaPrimaryFeed;
    }
    inline bool operator!=(const FavoritesQAItem&l, const FavoritesQAItem&r)
    {
        return !(l == r);
    }

    struct FavoritesQAListRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesQAListRequest";
        }
        static string MD5()
        {
            return "325d87d477a8cf7a6468ed6bb39da964";
        }
        FavoritesQAListRequest()
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
            _is.read(pageContext, 1, false);
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
    inline bool operator==(const FavoritesQAListRequest&l, const FavoritesQAListRequest&r)
    {
        return l.dataKey == r.dataKey && l.pageContext == r.pageContext;
    }
    inline bool operator!=(const FavoritesQAListRequest&l, const FavoritesQAListRequest&r)
    {
        return !(l == r);
    }

    struct FavoritesQAListResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesQAListResponse";
        }
        static string MD5()
        {
            return "b80f8e351ccf809b2c7d124e5c43d5c5";
        }
        FavoritesQAListResponse()
        :errCode(0),pageContext(""),hasNextPage(true),qaFavoritesCount(0)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            pageContext = "";
            hasNextPage = true;
            qaFavoritesCount = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(itemList, 1);
            _os.write(pageContext, 2);
            _os.write(hasNextPage, 3);
            _os.write(qaFavoritesCount, 4);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(itemList, 1, false);
            _is.read(pageContext, 2, false);
            _is.read(hasNextPage, 3, false);
            _is.read(qaFavoritesCount, 4, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(itemList,"itemList");
            _ds.display(pageContext,"pageContext");
            _ds.display(hasNextPage,"hasNextPage");
            _ds.display(qaFavoritesCount,"qaFavoritesCount");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(itemList, true);
            _ds.displaySimple(pageContext, true);
            _ds.displaySimple(hasNextPage, true);
            _ds.displaySimple(qaFavoritesCount, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        vector<jce::FavoritesQAItem> itemList;
        std::string pageContext;
        taf::Bool hasNextPage;
        taf::Int32 qaFavoritesCount;
    };
    inline bool operator==(const FavoritesQAListResponse&l, const FavoritesQAListResponse&r)
    {
        return l.errCode == r.errCode && l.itemList == r.itemList && l.pageContext == r.pageContext && l.hasNextPage == r.hasNextPage && l.qaFavoritesCount == r.qaFavoritesCount;
    }
    inline bool operator!=(const FavoritesQAListResponse&l, const FavoritesQAListResponse&r)
    {
        return !(l == r);
    }

    struct FavoritesFeedItem : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesFeedItem";
        }
        static string MD5()
        {
            return "53f7a30d5af520ecf3b3e50223081b2c";
        }
        FavoritesFeedItem()
        :favoritesTime(0)
        {
        }
        void resetDefautlt()
        {
            favoritesTime = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(favoritesTime, 0);
            _os.write(primaryFeed, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(favoritesTime, 0, false);
            _is.read(primaryFeed, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(favoritesTime,"favoritesTime");
            _ds.display(primaryFeed,"primaryFeed");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(favoritesTime, true);
            _ds.displaySimple(primaryFeed, false);
            return _os;
        }
    public:
        taf::Int64 favoritesTime;
        jce::CirclePrimaryFeed primaryFeed;
    };
    inline bool operator==(const FavoritesFeedItem&l, const FavoritesFeedItem&r)
    {
        return l.favoritesTime == r.favoritesTime && l.primaryFeed == r.primaryFeed;
    }
    inline bool operator!=(const FavoritesFeedItem&l, const FavoritesFeedItem&r)
    {
        return !(l == r);
    }

    struct FavoritesFeedListRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesFeedListRequest";
        }
        static string MD5()
        {
            return "325d87d477a8cf7a6468ed6bb39da964";
        }
        FavoritesFeedListRequest()
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
            _is.read(pageContext, 1, false);
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
    inline bool operator==(const FavoritesFeedListRequest&l, const FavoritesFeedListRequest&r)
    {
        return l.dataKey == r.dataKey && l.pageContext == r.pageContext;
    }
    inline bool operator!=(const FavoritesFeedListRequest&l, const FavoritesFeedListRequest&r)
    {
        return !(l == r);
    }

    struct FavoritesFeedListResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesFeedListResponse";
        }
        static string MD5()
        {
            return "49c0256cf39b25fad3c08160f64f5f2b";
        }
        FavoritesFeedListResponse()
        :errCode(0),pageContext(""),hasNextPage(true),feedFavoritesCount(0)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            pageContext = "";
            hasNextPage = true;
            feedFavoritesCount = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(itemList, 1);
            _os.write(pageContext, 2);
            _os.write(hasNextPage, 3);
            _os.write(feedFavoritesCount, 4);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(itemList, 1, false);
            _is.read(pageContext, 2, false);
            _is.read(hasNextPage, 3, false);
            _is.read(feedFavoritesCount, 4, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(itemList,"itemList");
            _ds.display(pageContext,"pageContext");
            _ds.display(hasNextPage,"hasNextPage");
            _ds.display(feedFavoritesCount,"feedFavoritesCount");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(itemList, true);
            _ds.displaySimple(pageContext, true);
            _ds.displaySimple(hasNextPage, true);
            _ds.displaySimple(feedFavoritesCount, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        vector<jce::FavoritesFeedItem> itemList;
        std::string pageContext;
        taf::Bool hasNextPage;
        taf::Int32 feedFavoritesCount;
    };
    inline bool operator==(const FavoritesFeedListResponse&l, const FavoritesFeedListResponse&r)
    {
        return l.errCode == r.errCode && l.itemList == r.itemList && l.pageContext == r.pageContext && l.hasNextPage == r.hasNextPage && l.feedFavoritesCount == r.feedFavoritesCount;
    }
    inline bool operator!=(const FavoritesFeedListResponse&l, const FavoritesFeedListResponse&r)
    {
        return !(l == r);
    }

    struct FavoritesOperationRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesOperationRequest";
        }
        static string MD5()
        {
            return "6d79bba47610a82c305f328c3a1aa698";
        }
        FavoritesOperationRequest()
        :feedType(0),operationType(0)
        {
        }
        void resetDefautlt()
        {
            feedType = 0;
            operationType = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(feedIdList, 0);
            _os.write(feedType, 1);
            _os.write(operationType, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(feedIdList, 0, false);
            _is.read(feedType, 1, false);
            _is.read(operationType, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(feedIdList,"feedIdList");
            _ds.display(feedType,"feedType");
            _ds.display(operationType,"operationType");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(feedIdList, true);
            _ds.displaySimple(feedType, true);
            _ds.displaySimple(operationType, false);
            return _os;
        }
    public:
        vector<std::string> feedIdList;
        taf::Char feedType;
        taf::Char operationType;
    };
    inline bool operator==(const FavoritesOperationRequest&l, const FavoritesOperationRequest&r)
    {
        return l.feedIdList == r.feedIdList && l.feedType == r.feedType && l.operationType == r.operationType;
    }
    inline bool operator!=(const FavoritesOperationRequest&l, const FavoritesOperationRequest&r)
    {
        return !(l == r);
    }

    struct FavoritesOperationResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FavoritesOperationResponse";
        }
        static string MD5()
        {
            return "6b3e9186b745c8628350bb5b8e06bf54";
        }
        FavoritesOperationResponse()
        :errCode(0),errMsg(""),qaFavoritesCount(0),feedFavoritesCount(0),maxQAFavoritesCount(0),maxFeedFavoritesCount(0)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            errMsg = "";
            qaFavoritesCount = 0;
            feedFavoritesCount = 0;
            maxQAFavoritesCount = 0;
            maxFeedFavoritesCount = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(errMsg, 1);
            _os.write(qaFavoritesCount, 2);
            _os.write(feedFavoritesCount, 3);
            _os.write(maxQAFavoritesCount, 4);
            _os.write(maxFeedFavoritesCount, 5);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(errMsg, 1, false);
            _is.read(qaFavoritesCount, 2, false);
            _is.read(feedFavoritesCount, 3, false);
            _is.read(maxQAFavoritesCount, 4, false);
            _is.read(maxFeedFavoritesCount, 5, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(errMsg,"errMsg");
            _ds.display(qaFavoritesCount,"qaFavoritesCount");
            _ds.display(feedFavoritesCount,"feedFavoritesCount");
            _ds.display(maxQAFavoritesCount,"maxQAFavoritesCount");
            _ds.display(maxFeedFavoritesCount,"maxFeedFavoritesCount");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(errMsg, true);
            _ds.displaySimple(qaFavoritesCount, true);
            _ds.displaySimple(feedFavoritesCount, true);
            _ds.displaySimple(maxQAFavoritesCount, true);
            _ds.displaySimple(maxFeedFavoritesCount, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        std::string errMsg;
        taf::Int32 qaFavoritesCount;
        taf::Int32 feedFavoritesCount;
        taf::Int32 maxQAFavoritesCount;
        taf::Int32 maxFeedFavoritesCount;
    };
    inline bool operator==(const FavoritesOperationResponse&l, const FavoritesOperationResponse&r)
    {
        return l.errCode == r.errCode && l.errMsg == r.errMsg && l.qaFavoritesCount == r.qaFavoritesCount && l.feedFavoritesCount == r.feedFavoritesCount && l.maxQAFavoritesCount == r.maxQAFavoritesCount && l.maxFeedFavoritesCount == r.maxFeedFavoritesCount;
    }
    inline bool operator!=(const FavoritesOperationResponse&l, const FavoritesOperationResponse&r)
    {
        return !(l == r);
    }


}

#define jce_FavoritesTopBarItem_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.type,b.type);jce_copy_struct(a.dataKey,b.dataKey);jce_copy_struct(a.title,b.title);

#define jce_FavoritesTopBarRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.userId,b.userId);

#define jce_FavoritesTopBarResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.itemList,b.itemList);

#define jce_FavoritesQAItem_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.favoritesTime,b.favoritesTime);jce_copy_struct(a.qaPrimaryFeed,b.qaPrimaryFeed);

#define jce_FavoritesQAListRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.dataKey,b.dataKey);jce_copy_struct(a.pageContext,b.pageContext);

#define jce_FavoritesQAListResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.itemList,b.itemList);jce_copy_struct(a.pageContext,b.pageContext);jce_copy_struct(a.hasNextPage,b.hasNextPage);jce_copy_struct(a.qaFavoritesCount,b.qaFavoritesCount);

#define jce_FavoritesFeedItem_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.favoritesTime,b.favoritesTime);jce_copy_struct(a.primaryFeed,b.primaryFeed);

#define jce_FavoritesFeedListRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.dataKey,b.dataKey);jce_copy_struct(a.pageContext,b.pageContext);

#define jce_FavoritesFeedListResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.itemList,b.itemList);jce_copy_struct(a.pageContext,b.pageContext);jce_copy_struct(a.hasNextPage,b.hasNextPage);jce_copy_struct(a.feedFavoritesCount,b.feedFavoritesCount);

#define jce_FavoritesOperationRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.feedIdList,b.feedIdList);jce_copy_struct(a.feedType,b.feedType);jce_copy_struct(a.operationType,b.operationType);

#define jce_FavoritesOperationResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.errMsg,b.errMsg);jce_copy_struct(a.qaFavoritesCount,b.qaFavoritesCount);jce_copy_struct(a.feedFavoritesCount,b.feedFavoritesCount);jce_copy_struct(a.maxQAFavoritesCount,b.maxQAFavoritesCount);jce_copy_struct(a.maxFeedFavoritesCount,b.maxFeedFavoritesCount);



#endif
