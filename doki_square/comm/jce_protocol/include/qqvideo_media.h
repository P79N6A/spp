// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 3.1.1.2 by WSRD Tencent.
// Generated from `qqvideo_media.jce'
// **********************************************************************

#ifndef __QQVIDEO_MEDIA_H_
#define __QQVIDEO_MEDIA_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;
#include "qqvideo_common.h"
#include "qqvideo_ona.h"
#include "qqvideo_circle.h"


namespace jce
{
    struct MediaPosterTopInfo : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.MediaPosterTopInfo";
        }
        static string MD5()
        {
            return "ecb4249593bb8f63581a21515ef30552";
        }
        MediaPosterTopInfo()
        :topStarTitle(""),topStarSubTitle("")
        {
        }
        void resetDefautlt()
        {
            topStarTitle = "";
            topStarSubTitle = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(topTagText, 0);
            _os.write(topStarList, 1);
            _os.write(topStarTitle, 2);
            _os.write(topStarSubTitle, 3);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(topTagText, 0, false);
            _is.read(topStarList, 1, false);
            _is.read(topStarTitle, 2, false);
            _is.read(topStarSubTitle, 3, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(topTagText,"topTagText");
            _ds.display(topStarList,"topStarList");
            _ds.display(topStarTitle,"topStarTitle");
            _ds.display(topStarSubTitle,"topStarSubTitle");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(topTagText, true);
            _ds.displaySimple(topStarList, true);
            _ds.displaySimple(topStarTitle, true);
            _ds.displaySimple(topStarSubTitle, false);
            return _os;
        }
    public:
        jce::IconTagText topTagText;
        vector<jce::ActorInfo> topStarList;
        std::string topStarTitle;
        std::string topStarSubTitle;
    };
    inline bool operator==(const MediaPosterTopInfo&l, const MediaPosterTopInfo&r)
    {
        return l.topTagText == r.topTagText && l.topStarList == r.topStarList && l.topStarTitle == r.topStarTitle && l.topStarSubTitle == r.topStarSubTitle;
    }
    inline bool operator!=(const MediaPosterTopInfo&l, const MediaPosterTopInfo&r)
    {
        return !(l == r);
    }

    struct MediaPosterMiddleInfo : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.MediaPosterMiddleInfo";
        }
        static string MD5()
        {
            return "192d6e06f00e93e9610e02779cf99be5";
        }
        MediaPosterMiddleInfo()
        :mediaType(0)
        {
        }
        void resetDefautlt()
        {
            mediaType = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(mediaType, 0);
            _os.write(poster, 1);
            _os.write(bulletinBoard, 2);
            _os.write(voteInfo, 3);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(mediaType, 0, true);
            _is.read(poster, 1, false);
            _is.read(bulletinBoard, 2, false);
            _is.read(voteInfo, 3, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(mediaType,"mediaType");
            _ds.display(poster,"poster");
            _ds.display(bulletinBoard,"bulletinBoard");
            _ds.display(voteInfo,"voteInfo");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(mediaType, true);
            _ds.displaySimple(poster, true);
            _ds.displaySimple(bulletinBoard, true);
            _ds.displaySimple(voteInfo, false);
            return _os;
        }
    public:
        taf::Int32 mediaType;
        jce::MediaPoster poster;
        jce::ONABulletinBoardV2 bulletinBoard;
        jce::FeedVoteInfo voteInfo;
    };
    inline bool operator==(const MediaPosterMiddleInfo&l, const MediaPosterMiddleInfo&r)
    {
        return l.mediaType == r.mediaType && l.poster == r.poster && l.bulletinBoard == r.bulletinBoard && l.voteInfo == r.voteInfo;
    }
    inline bool operator!=(const MediaPosterMiddleInfo&l, const MediaPosterMiddleInfo&r)
    {
        return !(l == r);
    }

    struct MediaPosterBottomInfo : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.MediaPosterBottomInfo";
        }
        static string MD5()
        {
            return "8bef83ce8789476aa8f6b6049f16db38";
        }
        MediaPosterBottomInfo()
        :hasMoreButton(0)
        {
        }
        void resetDefautlt()
        {
            hasMoreButton = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(tagList, 0);
            _os.write(commonTagText, 1);
            _os.write(hasMoreButton, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(tagList, 0, false);
            _is.read(commonTagText, 1, false);
            _is.read(hasMoreButton, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(tagList,"tagList");
            _ds.display(commonTagText,"commonTagText");
            _ds.display(hasMoreButton,"hasMoreButton");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(tagList, true);
            _ds.displaySimple(commonTagText, true);
            _ds.displaySimple(hasMoreButton, false);
            return _os;
        }
    public:
        vector<jce::IconTagText> tagList;
        jce::IconTagText commonTagText;
        taf::Int32 hasMoreButton;
    };
    inline bool operator==(const MediaPosterBottomInfo&l, const MediaPosterBottomInfo&r)
    {
        return l.tagList == r.tagList && l.commonTagText == r.commonTagText && l.hasMoreButton == r.hasMoreButton;
    }
    inline bool operator!=(const MediaPosterBottomInfo&l, const MediaPosterBottomInfo&r)
    {
        return !(l == r);
    }

    struct MultiMediaPosterMiddleInfo : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.MultiMediaPosterMiddleInfo";
        }
        static string MD5()
        {
            return "e685802365bbbc31d00fb00b5dc7d8ca";
        }
        MultiMediaPosterMiddleInfo()
        {
        }
        void resetDefautlt()
        {
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(photoList, 0);
            _os.write(miniVideos, 1);
            _os.write(voiceList, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(photoList, 0, false);
            _is.read(miniVideos, 1, false);
            _is.read(voiceList, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(photoList,"photoList");
            _ds.display(miniVideos,"miniVideos");
            _ds.display(voiceList,"voiceList");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(photoList, true);
            _ds.displaySimple(miniVideos, true);
            _ds.displaySimple(voiceList, false);
            return _os;
        }
    public:
        vector<jce::CommonDownloadImageData> photoList;
        vector<jce::CircleShortVideoUrl> miniVideos;
        vector<jce::ApolloVoiceData> voiceList;
    };
    inline bool operator==(const MultiMediaPosterMiddleInfo&l, const MultiMediaPosterMiddleInfo&r)
    {
        return l.photoList == r.photoList && l.miniVideos == r.miniVideos && l.voiceList == r.voiceList;
    }
    inline bool operator!=(const MultiMediaPosterMiddleInfo&l, const MultiMediaPosterMiddleInfo&r)
    {
        return !(l == r);
    }

    struct ONAMediaPoster : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.ONAMediaPoster";
        }
        static string MD5()
        {
            return "56bf3b56e5fe1318e75265e8e6688db9";
        }
        ONAMediaPoster()
        :contentText(""),reportKey(""),reportParams("")
        {
        }
        void resetDefautlt()
        {
            contentText = "";
            reportKey = "";
            reportParams = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(middleInfo, 0);
            _os.write(topInfo, 1);
            _os.write(bottomInfo, 2);
            _os.write(contentText, 3);
            _os.write(shareItem, 4);
            _os.write(attentItem, 5);
            _os.write(reportKey, 6);
            _os.write(reportParams, 7);
            _os.write(action, 8);
            _os.write(feedInfo, 9);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(middleInfo, 0, false);
            _is.read(topInfo, 1, false);
            _is.read(bottomInfo, 2, false);
            _is.read(contentText, 3, false);
            _is.read(shareItem, 4, false);
            _is.read(attentItem, 5, false);
            _is.read(reportKey, 6, false);
            _is.read(reportParams, 7, false);
            _is.read(action, 8, false);
            _is.read(feedInfo, 9, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(middleInfo,"middleInfo");
            _ds.display(topInfo,"topInfo");
            _ds.display(bottomInfo,"bottomInfo");
            _ds.display(contentText,"contentText");
            _ds.display(shareItem,"shareItem");
            _ds.display(attentItem,"attentItem");
            _ds.display(reportKey,"reportKey");
            _ds.display(reportParams,"reportParams");
            _ds.display(action,"action");
            _ds.display(feedInfo,"feedInfo");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(middleInfo, true);
            _ds.displaySimple(topInfo, true);
            _ds.displaySimple(bottomInfo, true);
            _ds.displaySimple(contentText, true);
            _ds.displaySimple(shareItem, true);
            _ds.displaySimple(attentItem, true);
            _ds.displaySimple(reportKey, true);
            _ds.displaySimple(reportParams, true);
            _ds.displaySimple(action, true);
            _ds.displaySimple(feedInfo, false);
            return _os;
        }
    public:
        jce::MediaPosterMiddleInfo middleInfo;
        jce::MediaPosterTopInfo topInfo;
        jce::MediaPosterBottomInfo bottomInfo;
        std::string contentText;
        jce::ShareItem shareItem;
        jce::VideoAttentItem attentItem;
        std::string reportKey;
        std::string reportParams;
        jce::Action action;
        jce::FeedInfo feedInfo;
    };
    inline bool operator==(const ONAMediaPoster&l, const ONAMediaPoster&r)
    {
        return l.middleInfo == r.middleInfo && l.topInfo == r.topInfo && l.bottomInfo == r.bottomInfo && l.contentText == r.contentText && l.shareItem == r.shareItem && l.attentItem == r.attentItem && l.reportKey == r.reportKey && l.reportParams == r.reportParams && l.action == r.action && l.feedInfo == r.feedInfo;
    }
    inline bool operator!=(const ONAMediaPoster&l, const ONAMediaPoster&r)
    {
        return !(l == r);
    }

    struct ONAMultiMediaPoster : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.ONAMultiMediaPoster";
        }
        static string MD5()
        {
            return "13be0692820decbb01fa7fb95a8cd069";
        }
        ONAMultiMediaPoster()
        :contentText(""),reportKey(""),reportParams("")
        {
        }
        void resetDefautlt()
        {
            contentText = "";
            reportKey = "";
            reportParams = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(middleInfo, 0);
            _os.write(topInfo, 1);
            _os.write(bottomInfo, 2);
            _os.write(contentText, 3);
            _os.write(shareItem, 4);
            _os.write(attentItem, 5);
            _os.write(reportKey, 6);
            _os.write(reportParams, 7);
            _os.write(action, 8);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(middleInfo, 0, false);
            _is.read(topInfo, 1, false);
            _is.read(bottomInfo, 2, false);
            _is.read(contentText, 3, false);
            _is.read(shareItem, 4, false);
            _is.read(attentItem, 5, false);
            _is.read(reportKey, 6, false);
            _is.read(reportParams, 7, false);
            _is.read(action, 8, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(middleInfo,"middleInfo");
            _ds.display(topInfo,"topInfo");
            _ds.display(bottomInfo,"bottomInfo");
            _ds.display(contentText,"contentText");
            _ds.display(shareItem,"shareItem");
            _ds.display(attentItem,"attentItem");
            _ds.display(reportKey,"reportKey");
            _ds.display(reportParams,"reportParams");
            _ds.display(action,"action");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(middleInfo, true);
            _ds.displaySimple(topInfo, true);
            _ds.displaySimple(bottomInfo, true);
            _ds.displaySimple(contentText, true);
            _ds.displaySimple(shareItem, true);
            _ds.displaySimple(attentItem, true);
            _ds.displaySimple(reportKey, true);
            _ds.displaySimple(reportParams, true);
            _ds.displaySimple(action, false);
            return _os;
        }
    public:
        jce::MultiMediaPosterMiddleInfo middleInfo;
        jce::MediaPosterTopInfo topInfo;
        jce::MediaPosterBottomInfo bottomInfo;
        std::string contentText;
        jce::ShareItem shareItem;
        jce::VideoAttentItem attentItem;
        std::string reportKey;
        std::string reportParams;
        jce::Action action;
    };
    inline bool operator==(const ONAMultiMediaPoster&l, const ONAMultiMediaPoster&r)
    {
        return l.middleInfo == r.middleInfo && l.topInfo == r.topInfo && l.bottomInfo == r.bottomInfo && l.contentText == r.contentText && l.shareItem == r.shareItem && l.attentItem == r.attentItem && l.reportKey == r.reportKey && l.reportParams == r.reportParams && l.action == r.action;
    }
    inline bool operator!=(const ONAMultiMediaPoster&l, const ONAMultiMediaPoster&r)
    {
        return !(l == r);
    }

    struct ONAStarCommentMediaPoster : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.ONAStarCommentMediaPoster";
        }
        static string MD5()
        {
            return "2900a96aebf68ac987b45008babd5793";
        }
        ONAStarCommentMediaPoster()
        :feedId(""),feedType(0),commentCount(0),likeCount(0),reportKey(""),reportParams(""),contentText(""),isLike(0)
        {
        }
        void resetDefautlt()
        {
            feedId = "";
            feedType = 0;
            commentCount = 0;
            likeCount = 0;
            reportKey = "";
            reportParams = "";
            contentText = "";
            isLike = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(feedId, 0);
            _os.write(feedType, 1);
            _os.write(commentCount, 2);
            _os.write(likeCount, 3);
            _os.write(middleInfo, 4);
            _os.write(topInfo, 5);
            _os.write(bottomInfo, 6);
            _os.write(contentInfo, 7);
            _os.write(shareItem, 8);
            _os.write(commentList, 9);
            _os.write(actionBar, 10);
            _os.write(reportKey, 11);
            _os.write(reportParams, 12);
            _os.write(contentText, 13);
            _os.write(action, 14);
            _os.write(isLike, 15);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(feedId, 0, true);
            _is.read(feedType, 1, true);
            _is.read(commentCount, 2, false);
            _is.read(likeCount, 3, false);
            _is.read(middleInfo, 4, false);
            _is.read(topInfo, 5, false);
            _is.read(bottomInfo, 6, false);
            _is.read(contentInfo, 7, false);
            _is.read(shareItem, 8, false);
            _is.read(commentList, 9, false);
            _is.read(actionBar, 10, false);
            _is.read(reportKey, 11, false);
            _is.read(reportParams, 12, false);
            _is.read(contentText, 13, false);
            _is.read(action, 14, false);
            _is.read(isLike, 15, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(feedId,"feedId");
            _ds.display(feedType,"feedType");
            _ds.display(commentCount,"commentCount");
            _ds.display(likeCount,"likeCount");
            _ds.display(middleInfo,"middleInfo");
            _ds.display(topInfo,"topInfo");
            _ds.display(bottomInfo,"bottomInfo");
            _ds.display(contentInfo,"contentInfo");
            _ds.display(shareItem,"shareItem");
            _ds.display(commentList,"commentList");
            _ds.display(actionBar,"actionBar");
            _ds.display(reportKey,"reportKey");
            _ds.display(reportParams,"reportParams");
            _ds.display(contentText,"contentText");
            _ds.display(action,"action");
            _ds.display(isLike,"isLike");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(feedId, true);
            _ds.displaySimple(feedType, true);
            _ds.displaySimple(commentCount, true);
            _ds.displaySimple(likeCount, true);
            _ds.displaySimple(middleInfo, true);
            _ds.displaySimple(topInfo, true);
            _ds.displaySimple(bottomInfo, true);
            _ds.displaySimple(contentInfo, true);
            _ds.displaySimple(shareItem, true);
            _ds.displaySimple(commentList, true);
            _ds.displaySimple(actionBar, true);
            _ds.displaySimple(reportKey, true);
            _ds.displaySimple(reportParams, true);
            _ds.displaySimple(contentText, true);
            _ds.displaySimple(action, true);
            _ds.displaySimple(isLike, false);
            return _os;
        }
    public:
        std::string feedId;
        taf::Int32 feedType;
        taf::Int32 commentCount;
        taf::Int32 likeCount;
        jce::MultiMediaPosterMiddleInfo middleInfo;
        jce::MediaPosterTopInfo topInfo;
        jce::MediaPosterBottomInfo bottomInfo;
        jce::CircleCommentFeed contentInfo;
        jce::ShareItem shareItem;
        vector<jce::CircleCommentFeed> commentList;
        jce::AttentActionBar actionBar;
        std::string reportKey;
        std::string reportParams;
        std::string contentText;
        jce::Action action;
        taf::Char isLike;
    };
    inline bool operator==(const ONAStarCommentMediaPoster&l, const ONAStarCommentMediaPoster&r)
    {
        return l.feedId == r.feedId && l.feedType == r.feedType && l.commentCount == r.commentCount && l.likeCount == r.likeCount && l.middleInfo == r.middleInfo && l.topInfo == r.topInfo && l.bottomInfo == r.bottomInfo && l.contentInfo == r.contentInfo && l.shareItem == r.shareItem && l.commentList == r.commentList && l.actionBar == r.actionBar && l.reportKey == r.reportKey && l.reportParams == r.reportParams && l.contentText == r.contentText && l.action == r.action && l.isLike == r.isLike;
    }
    inline bool operator!=(const ONAStarCommentMediaPoster&l, const ONAStarCommentMediaPoster&r)
    {
        return !(l == r);
    }


}

#define jce_MediaPosterTopInfo_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.topTagText,b.topTagText);jce_copy_struct(a.topStarList,b.topStarList);jce_copy_struct(a.topStarTitle,b.topStarTitle);jce_copy_struct(a.topStarSubTitle,b.topStarSubTitle);

#define jce_MediaPosterMiddleInfo_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.mediaType,b.mediaType);jce_copy_struct(a.poster,b.poster);jce_copy_struct(a.bulletinBoard,b.bulletinBoard);jce_copy_struct(a.voteInfo,b.voteInfo);

#define jce_MediaPosterBottomInfo_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.tagList,b.tagList);jce_copy_struct(a.commonTagText,b.commonTagText);jce_copy_struct(a.hasMoreButton,b.hasMoreButton);

#define jce_MultiMediaPosterMiddleInfo_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.photoList,b.photoList);jce_copy_struct(a.miniVideos,b.miniVideos);jce_copy_struct(a.voiceList,b.voiceList);

#define jce_ONAMediaPoster_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.middleInfo,b.middleInfo);jce_copy_struct(a.topInfo,b.topInfo);jce_copy_struct(a.bottomInfo,b.bottomInfo);jce_copy_struct(a.contentText,b.contentText);jce_copy_struct(a.shareItem,b.shareItem);jce_copy_struct(a.attentItem,b.attentItem);jce_copy_struct(a.reportKey,b.reportKey);jce_copy_struct(a.reportParams,b.reportParams);jce_copy_struct(a.action,b.action);jce_copy_struct(a.feedInfo,b.feedInfo);

#define jce_ONAMultiMediaPoster_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.middleInfo,b.middleInfo);jce_copy_struct(a.topInfo,b.topInfo);jce_copy_struct(a.bottomInfo,b.bottomInfo);jce_copy_struct(a.contentText,b.contentText);jce_copy_struct(a.shareItem,b.shareItem);jce_copy_struct(a.attentItem,b.attentItem);jce_copy_struct(a.reportKey,b.reportKey);jce_copy_struct(a.reportParams,b.reportParams);jce_copy_struct(a.action,b.action);

#define jce_ONAStarCommentMediaPoster_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.feedId,b.feedId);jce_copy_struct(a.feedType,b.feedType);jce_copy_struct(a.commentCount,b.commentCount);jce_copy_struct(a.likeCount,b.likeCount);jce_copy_struct(a.middleInfo,b.middleInfo);jce_copy_struct(a.topInfo,b.topInfo);jce_copy_struct(a.bottomInfo,b.bottomInfo);jce_copy_struct(a.contentInfo,b.contentInfo);jce_copy_struct(a.shareItem,b.shareItem);jce_copy_struct(a.commentList,b.commentList);jce_copy_struct(a.actionBar,b.actionBar);jce_copy_struct(a.reportKey,b.reportKey);jce_copy_struct(a.reportParams,b.reportParams);jce_copy_struct(a.contentText,b.contentText);jce_copy_struct(a.action,b.action);jce_copy_struct(a.isLike,b.isLike);



#endif
