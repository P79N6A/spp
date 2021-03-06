// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 3.1.1.2 by WSRD Tencent.
// Generated from `rank_101.jce'
// **********************************************************************

#ifndef __RANK_101_H_
#define __RANK_101_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;


namespace m101Rank
{
    struct stVoteRecord : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.stVoteRecord";
        }
        static string MD5()
        {
            return "a0f7f1c70b82271b1f084e61faf77494";
        }
        stVoteRecord()
        :lStarId(0),lVoteTime(0)
        {
        }
        void resetDefautlt()
        {
            lStarId = 0;
            lVoteTime = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(lStarId, 0);
            _os.write(lVoteTime, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(lStarId, 0, true);
            _is.read(lVoteTime, 1, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(lStarId,"lStarId");
            _ds.display(lVoteTime,"lVoteTime");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(lStarId, true);
            _ds.displaySimple(lVoteTime, false);
            return _os;
        }
    public:
        taf::Int64 lStarId;
        taf::Int64 lVoteTime;
    };
    inline bool operator==(const stVoteRecord&l, const stVoteRecord&r)
    {
        return l.lStarId == r.lStarId && l.lVoteTime == r.lVoteTime;
    }
    inline bool operator!=(const stVoteRecord&l, const stVoteRecord&r)
    {
        return !(l == r);
    }

    struct stVoteRecordReq : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.stVoteRecordReq";
        }
        static string MD5()
        {
            return "c5d29a3a3b5e33926ecd936ff3153e6b";
        }
        stVoteRecordReq()
        :lVuid(0)
        {
        }
        void resetDefautlt()
        {
            lVuid = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(lVuid, 0);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(lVuid, 0, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(lVuid,"lVuid");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(lVuid, false);
            return _os;
        }
    public:
        taf::Int64 lVuid;
    };
    inline bool operator==(const stVoteRecordReq&l, const stVoteRecordReq&r)
    {
        return l.lVuid == r.lVuid;
    }
    inline bool operator!=(const stVoteRecordReq&l, const stVoteRecordReq&r)
    {
        return !(l == r);
    }

    struct stVoteRecordRsp : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.stVoteRecordRsp";
        }
        static string MD5()
        {
            return "388aa350e10cd195a7f140602109e247";
        }
        stVoteRecordRsp()
        :iErrCode(0),strErrMsg("")
        {
        }
        void resetDefautlt()
        {
            iErrCode = 0;
            strErrMsg = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(iErrCode, 0);
            _os.write(strErrMsg, 1);
            _os.write(vecVoteRecord, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(iErrCode, 0, true);
            _is.read(strErrMsg, 1, false);
            _is.read(vecVoteRecord, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(iErrCode,"iErrCode");
            _ds.display(strErrMsg,"strErrMsg");
            _ds.display(vecVoteRecord,"vecVoteRecord");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(iErrCode, true);
            _ds.displaySimple(strErrMsg, true);
            _ds.displaySimple(vecVoteRecord, false);
            return _os;
        }
    public:
        taf::Int32 iErrCode;
        std::string strErrMsg;
        vector<m101Rank::stVoteRecord> vecVoteRecord;
    };
    inline bool operator==(const stVoteRecordRsp&l, const stVoteRecordRsp&r)
    {
        return l.iErrCode == r.iErrCode && l.strErrMsg == r.strErrMsg && l.vecVoteRecord == r.vecVoteRecord;
    }
    inline bool operator!=(const stVoteRecordRsp&l, const stVoteRecordRsp&r)
    {
        return !(l == r);
    }

    struct BannerImg : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.BannerImg";
        }
        static string MD5()
        {
            return "0c1d945735360d4e3752e1b2d448ba0c";
        }
        BannerImg()
        :strStarBg(""),strRankBg(""),strRankName("")
        {
        }
        void resetDefautlt()
        {
            strStarBg = "";
            strRankBg = "";
            strRankName = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(strStarBg, 0);
            _os.write(strRankBg, 1);
            _os.write(strRankName, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(strStarBg, 0, true);
            _is.read(strRankBg, 1, true);
            _is.read(strRankName, 2, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(strStarBg,"strStarBg");
            _ds.display(strRankBg,"strRankBg");
            _ds.display(strRankName,"strRankName");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(strStarBg, true);
            _ds.displaySimple(strRankBg, true);
            _ds.displaySimple(strRankName, false);
            return _os;
        }
    public:
        std::string strStarBg;
        std::string strRankBg;
        std::string strRankName;
    };
    inline bool operator==(const BannerImg&l, const BannerImg&r)
    {
        return l.strStarBg == r.strStarBg && l.strRankBg == r.strRankBg && l.strRankName == r.strRankName;
    }
    inline bool operator!=(const BannerImg&l, const BannerImg&r)
    {
        return !(l == r);
    }

    struct stStarInfo : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.stStarInfo";
        }
        static string MD5()
        {
            return "27a952e7888885d53794114c0f8d5c13";
        }
        stStarInfo()
        :lStarId(0),lDokiId(0),strStarName(""),strFaceImgUrl("")
        {
        }
        void resetDefautlt()
        {
            lStarId = 0;
            lDokiId = 0;
            strStarName = "";
            strFaceImgUrl = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(lStarId, 0);
            _os.write(lDokiId, 1);
            _os.write(strStarName, 2);
            _os.write(strFaceImgUrl, 3);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(lStarId, 0, true);
            _is.read(lDokiId, 1, true);
            _is.read(strStarName, 2, true);
            _is.read(strFaceImgUrl, 3, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(lStarId,"lStarId");
            _ds.display(lDokiId,"lDokiId");
            _ds.display(strStarName,"strStarName");
            _ds.display(strFaceImgUrl,"strFaceImgUrl");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(lStarId, true);
            _ds.displaySimple(lDokiId, true);
            _ds.displaySimple(strStarName, true);
            _ds.displaySimple(strFaceImgUrl, false);
            return _os;
        }
    public:
        taf::Int64 lStarId;
        taf::Int64 lDokiId;
        std::string strStarName;
        std::string strFaceImgUrl;
    };
    inline bool operator==(const stStarInfo&l, const stStarInfo&r)
    {
        return l.lStarId == r.lStarId && l.lDokiId == r.lDokiId && l.strStarName == r.strStarName && l.strFaceImgUrl == r.strFaceImgUrl;
    }
    inline bool operator!=(const stStarInfo&l, const stStarInfo&r)
    {
        return !(l == r);
    }

    struct STPersonData : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.STPersonData";
        }
        static string MD5()
        {
            return "a2b85a9e0f7b72a3b6cbdd12ac2e449e";
        }
        STPersonData()
        :lUserid(0),strNick(""),strHead(""),strCity(""),strCountry(""),strSex(""),cFrom(0),dwCreateTime(0),dwStatus(0),lGiftPoint(0)
        {
        }
        void resetDefautlt()
        {
            lUserid = 0;
            strNick = "";
            strHead = "";
            strCity = "";
            strCountry = "";
            strSex = "";
            cFrom = 0;
            dwCreateTime = 0;
            dwStatus = 0;
            lGiftPoint = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(lUserid, 0);
            _os.write(strNick, 1);
            _os.write(strHead, 2);
            _os.write(strCity, 3);
            _os.write(strCountry, 4);
            _os.write(strSex, 5);
            _os.write(cFrom, 6);
            _os.write(dwCreateTime, 7);
            _os.write(dwStatus, 8);
            _os.write(lGiftPoint, 9);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(lUserid, 0, true);
            _is.read(strNick, 1, true);
            _is.read(strHead, 2, true);
            _is.read(strCity, 3, true);
            _is.read(strCountry, 4, true);
            _is.read(strSex, 5, true);
            _is.read(cFrom, 6, true);
            _is.read(dwCreateTime, 7, true);
            _is.read(dwStatus, 8, true);
            _is.read(lGiftPoint, 9, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(lUserid,"lUserid");
            _ds.display(strNick,"strNick");
            _ds.display(strHead,"strHead");
            _ds.display(strCity,"strCity");
            _ds.display(strCountry,"strCountry");
            _ds.display(strSex,"strSex");
            _ds.display(cFrom,"cFrom");
            _ds.display(dwCreateTime,"dwCreateTime");
            _ds.display(dwStatus,"dwStatus");
            _ds.display(lGiftPoint,"lGiftPoint");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(lUserid, true);
            _ds.displaySimple(strNick, true);
            _ds.displaySimple(strHead, true);
            _ds.displaySimple(strCity, true);
            _ds.displaySimple(strCountry, true);
            _ds.displaySimple(strSex, true);
            _ds.displaySimple(cFrom, true);
            _ds.displaySimple(dwCreateTime, true);
            _ds.displaySimple(dwStatus, true);
            _ds.displaySimple(lGiftPoint, false);
            return _os;
        }
    public:
        taf::Int64 lUserid;
        std::string strNick;
        std::string strHead;
        std::string strCity;
        std::string strCountry;
        std::string strSex;
        taf::UInt8 cFrom;
        taf::UInt32 dwCreateTime;
        taf::UInt32 dwStatus;
        taf::Int64 lGiftPoint;
    };
    inline bool operator==(const STPersonData&l, const STPersonData&r)
    {
        return l.lUserid == r.lUserid && l.strNick == r.strNick && l.strHead == r.strHead && l.strCity == r.strCity && l.strCountry == r.strCountry && l.strSex == r.strSex && l.cFrom == r.cFrom && l.dwCreateTime == r.dwCreateTime && l.dwStatus == r.dwStatus && l.lGiftPoint == r.lGiftPoint;
    }
    inline bool operator!=(const STPersonData&l, const STPersonData&r)
    {
        return !(l == r);
    }

    struct STFansInfo : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.STFansInfo";
        }
        static string MD5()
        {
            return "3647774447a095b60bb868e5d1dffce7";
        }
        STFansInfo()
        :iVoteNum(0),iRank(0),strPersonalLink(""),iReceiveNum(0)
        {
        }
        void resetDefautlt()
        {
            iVoteNum = 0;
            iRank = 0;
            strPersonalLink = "";
            iReceiveNum = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(stPersonData, 0);
            _os.write(iVoteNum, 1);
            _os.write(iRank, 2);
            _os.write(strPersonalLink, 3);
            _os.write(stFavoriteStar, 5);
            _os.write(iReceiveNum, 6);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(stPersonData, 0, true);
            _is.read(iVoteNum, 1, true);
            _is.read(iRank, 2, true);
            _is.read(strPersonalLink, 3, false);
            _is.read(stFavoriteStar, 5, false);
            _is.read(iReceiveNum, 6, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(stPersonData,"stPersonData");
            _ds.display(iVoteNum,"iVoteNum");
            _ds.display(iRank,"iRank");
            _ds.display(strPersonalLink,"strPersonalLink");
            _ds.display(stFavoriteStar,"stFavoriteStar");
            _ds.display(iReceiveNum,"iReceiveNum");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(stPersonData, true);
            _ds.displaySimple(iVoteNum, true);
            _ds.displaySimple(iRank, true);
            _ds.displaySimple(strPersonalLink, true);
            _ds.displaySimple(stFavoriteStar, true);
            _ds.displaySimple(iReceiveNum, false);
            return _os;
        }
    public:
        m101Rank::STPersonData stPersonData;
        taf::Int32 iVoteNum;
        taf::Int32 iRank;
        std::string strPersonalLink;
        m101Rank::stStarInfo stFavoriteStar;
        taf::Int32 iReceiveNum;
    };
    inline bool operator==(const STFansInfo&l, const STFansInfo&r)
    {
        return l.stPersonData == r.stPersonData && l.iVoteNum == r.iVoteNum && l.iRank == r.iRank && l.strPersonalLink == r.strPersonalLink && l.stFavoriteStar == r.stFavoriteStar && l.iReceiveNum == r.iReceiveNum;
    }
    inline bool operator!=(const STFansInfo&l, const STFansInfo&r)
    {
        return !(l == r);
    }

    struct st101FansRankMenuReq : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.st101FansRankMenuReq";
        }
        static string MD5()
        {
            return "b39bf628549229f912d60a13af977429";
        }
        st101FansRankMenuReq()
        :iRankType(0),iTimeType(0),lRankTime(0),iReqNum(0),iPage(0),lStarId(0)
        {
        }
        void resetDefautlt()
        {
            iRankType = 0;
            iTimeType = 0;
            lRankTime = 0;
            iReqNum = 0;
            iPage = 0;
            lStarId = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(iRankType, 0);
            _os.write(iTimeType, 1);
            _os.write(lRankTime, 2);
            _os.write(iReqNum, 3);
            _os.write(iPage, 4);
            _os.write(lStarId, 5);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(iRankType, 0, true);
            _is.read(iTimeType, 1, false);
            _is.read(lRankTime, 2, false);
            _is.read(iReqNum, 3, false);
            _is.read(iPage, 4, false);
            _is.read(lStarId, 5, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(iRankType,"iRankType");
            _ds.display(iTimeType,"iTimeType");
            _ds.display(lRankTime,"lRankTime");
            _ds.display(iReqNum,"iReqNum");
            _ds.display(iPage,"iPage");
            _ds.display(lStarId,"lStarId");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(iRankType, true);
            _ds.displaySimple(iTimeType, true);
            _ds.displaySimple(lRankTime, true);
            _ds.displaySimple(iReqNum, true);
            _ds.displaySimple(iPage, true);
            _ds.displaySimple(lStarId, false);
            return _os;
        }
    public:
        taf::Int32 iRankType;
        taf::Int32 iTimeType;
        taf::Int64 lRankTime;
        taf::UInt32 iReqNum;
        taf::UInt32 iPage;
        taf::Int64 lStarId;
    };
    inline bool operator==(const st101FansRankMenuReq&l, const st101FansRankMenuReq&r)
    {
        return l.iRankType == r.iRankType && l.iTimeType == r.iTimeType && l.lRankTime == r.lRankTime && l.iReqNum == r.iReqNum && l.iPage == r.iPage && l.lStarId == r.lStarId;
    }
    inline bool operator!=(const st101FansRankMenuReq&l, const st101FansRankMenuReq&r)
    {
        return !(l == r);
    }

    struct st101FansRankMenuRsp : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.st101FansRankMenuRsp";
        }
        static string MD5()
        {
            return "e3709ae660b9f90cd049544ec4127ffa";
        }
        st101FansRankMenuRsp()
        :iErrCode(0),strErrMsg(""),iPeriod(0),iNextPage(0)
        {
        }
        void resetDefautlt()
        {
            iErrCode = 0;
            strErrMsg = "";
            iPeriod = 0;
            iNextPage = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(iErrCode, 0);
            _os.write(strErrMsg, 1);
            _os.write(iPeriod, 2);
            _os.write(stBannerImg, 3);
            _os.write(vecUserInfo, 4);
            _os.write(iNextPage, 5);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(iErrCode, 0, true);
            _is.read(strErrMsg, 1, false);
            _is.read(iPeriod, 2, false);
            _is.read(stBannerImg, 3, false);
            _is.read(vecUserInfo, 4, false);
            _is.read(iNextPage, 5, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(iErrCode,"iErrCode");
            _ds.display(strErrMsg,"strErrMsg");
            _ds.display(iPeriod,"iPeriod");
            _ds.display(stBannerImg,"stBannerImg");
            _ds.display(vecUserInfo,"vecUserInfo");
            _ds.display(iNextPage,"iNextPage");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(iErrCode, true);
            _ds.displaySimple(strErrMsg, true);
            _ds.displaySimple(iPeriod, true);
            _ds.displaySimple(stBannerImg, true);
            _ds.displaySimple(vecUserInfo, true);
            _ds.displaySimple(iNextPage, false);
            return _os;
        }
    public:
        taf::Int32 iErrCode;
        std::string strErrMsg;
        taf::UInt32 iPeriod;
        m101Rank::BannerImg stBannerImg;
        vector<m101Rank::STFansInfo> vecUserInfo;
        taf::UInt32 iNextPage;
    };
    inline bool operator==(const st101FansRankMenuRsp&l, const st101FansRankMenuRsp&r)
    {
        return l.iErrCode == r.iErrCode && l.strErrMsg == r.strErrMsg && l.iPeriod == r.iPeriod && l.stBannerImg == r.stBannerImg && l.vecUserInfo == r.vecUserInfo && l.iNextPage == r.iNextPage;
    }
    inline bool operator!=(const st101FansRankMenuRsp&l, const st101FansRankMenuRsp&r)
    {
        return !(l == r);
    }

    struct st101ShareFlowReq : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.st101ShareFlowReq";
        }
        static string MD5()
        {
            return "0c1d945735360d4e3752e1b2d448ba0c";
        }
        st101ShareFlowReq()
        :strStarid(""),strSrcVuid(""),strCardId("")
        {
        }
        void resetDefautlt()
        {
            strStarid = "";
            strSrcVuid = "";
            strCardId = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(strStarid, 0);
            _os.write(strSrcVuid, 1);
            _os.write(strCardId, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(strStarid, 0, true);
            _is.read(strSrcVuid, 1, true);
            _is.read(strCardId, 2, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(strStarid,"strStarid");
            _ds.display(strSrcVuid,"strSrcVuid");
            _ds.display(strCardId,"strCardId");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(strStarid, true);
            _ds.displaySimple(strSrcVuid, true);
            _ds.displaySimple(strCardId, false);
            return _os;
        }
    public:
        std::string strStarid;
        std::string strSrcVuid;
        std::string strCardId;
    };
    inline bool operator==(const st101ShareFlowReq&l, const st101ShareFlowReq&r)
    {
        return l.strStarid == r.strStarid && l.strSrcVuid == r.strSrcVuid && l.strCardId == r.strCardId;
    }
    inline bool operator!=(const st101ShareFlowReq&l, const st101ShareFlowReq&r)
    {
        return !(l == r);
    }

    struct st101ShareFlowRsp : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.st101ShareFlowRsp";
        }
        static string MD5()
        {
            return "be3dd307aa0bb3dc538c9a92fea985be";
        }
        st101ShareFlowRsp()
        :iErrCode(0),strErrMsg(""),iTotalSupNum(0)
        {
        }
        void resetDefautlt()
        {
            iErrCode = 0;
            strErrMsg = "";
            iTotalSupNum = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(iErrCode, 0);
            _os.write(strErrMsg, 1);
            _os.write(vecUserInfo, 2);
            _os.write(iTotalSupNum, 3);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(iErrCode, 0, true);
            _is.read(strErrMsg, 1, false);
            _is.read(vecUserInfo, 2, false);
            _is.read(iTotalSupNum, 3, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(iErrCode,"iErrCode");
            _ds.display(strErrMsg,"strErrMsg");
            _ds.display(vecUserInfo,"vecUserInfo");
            _ds.display(iTotalSupNum,"iTotalSupNum");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(iErrCode, true);
            _ds.displaySimple(strErrMsg, true);
            _ds.displaySimple(vecUserInfo, true);
            _ds.displaySimple(iTotalSupNum, false);
            return _os;
        }
    public:
        taf::Int32 iErrCode;
        std::string strErrMsg;
        vector<m101Rank::STFansInfo> vecUserInfo;
        taf::Int32 iTotalSupNum;
    };
    inline bool operator==(const st101ShareFlowRsp&l, const st101ShareFlowRsp&r)
    {
        return l.iErrCode == r.iErrCode && l.strErrMsg == r.strErrMsg && l.vecUserInfo == r.vecUserInfo && l.iTotalSupNum == r.iTotalSupNum;
    }
    inline bool operator!=(const st101ShareFlowRsp&l, const st101ShareFlowRsp&r)
    {
        return !(l == r);
    }

    struct STGetUserInfoReq : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.STGetUserInfoReq";
        }
        static string MD5()
        {
            return "b60948316e6b381c4405fcf043850c7c";
        }
        STGetUserInfoReq()
        :lUserid(0)
        {
        }
        void resetDefautlt()
        {
            lUserid = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(lUserid, 0);
            _os.write(vecUserids, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(lUserid, 0, true);
            _is.read(vecUserids, 1, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(lUserid,"lUserid");
            _ds.display(vecUserids,"vecUserids");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(lUserid, true);
            _ds.displaySimple(vecUserids, false);
            return _os;
        }
    public:
        taf::Int64 lUserid;
        vector<taf::Int64> vecUserids;
    };
    inline bool operator==(const STGetUserInfoReq&l, const STGetUserInfoReq&r)
    {
        return l.lUserid == r.lUserid && l.vecUserids == r.vecUserids;
    }
    inline bool operator!=(const STGetUserInfoReq&l, const STGetUserInfoReq&r)
    {
        return !(l == r);
    }

    struct STGetUserInfoRsp : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "m101Rank.STGetUserInfoRsp";
        }
        static string MD5()
        {
            return "9ac4ff9d01d95149b61f213e8d074de3";
        }
        STGetUserInfoRsp()
        :iErrCode(0),strErrMsg("")
        {
        }
        void resetDefautlt()
        {
            iErrCode = 0;
            strErrMsg = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(iErrCode, 0);
            _os.write(strErrMsg, 1);
            _os.write(mapUserInfo, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(iErrCode, 0, true);
            _is.read(strErrMsg, 1, false);
            _is.read(mapUserInfo, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(iErrCode,"iErrCode");
            _ds.display(strErrMsg,"strErrMsg");
            _ds.display(mapUserInfo,"mapUserInfo");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(iErrCode, true);
            _ds.displaySimple(strErrMsg, true);
            _ds.displaySimple(mapUserInfo, false);
            return _os;
        }
    public:
        taf::Int32 iErrCode;
        std::string strErrMsg;
        map<taf::Int64, m101Rank::STPersonData> mapUserInfo;
    };
    inline bool operator==(const STGetUserInfoRsp&l, const STGetUserInfoRsp&r)
    {
        return l.iErrCode == r.iErrCode && l.strErrMsg == r.strErrMsg && l.mapUserInfo == r.mapUserInfo;
    }
    inline bool operator!=(const STGetUserInfoRsp&l, const STGetUserInfoRsp&r)
    {
        return !(l == r);
    }


}

#define m101Rank_stVoteRecord_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.lStarId,b.lStarId);jce_copy_struct(a.lVoteTime,b.lVoteTime);

#define m101Rank_stVoteRecordReq_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.lVuid,b.lVuid);

#define m101Rank_stVoteRecordRsp_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.iErrCode,b.iErrCode);jce_copy_struct(a.strErrMsg,b.strErrMsg);jce_copy_struct(a.vecVoteRecord,b.vecVoteRecord);

#define m101Rank_BannerImg_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.strStarBg,b.strStarBg);jce_copy_struct(a.strRankBg,b.strRankBg);jce_copy_struct(a.strRankName,b.strRankName);

#define m101Rank_stStarInfo_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.lStarId,b.lStarId);jce_copy_struct(a.lDokiId,b.lDokiId);jce_copy_struct(a.strStarName,b.strStarName);jce_copy_struct(a.strFaceImgUrl,b.strFaceImgUrl);

#define m101Rank_STPersonData_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.lUserid,b.lUserid);jce_copy_struct(a.strNick,b.strNick);jce_copy_struct(a.strHead,b.strHead);jce_copy_struct(a.strCity,b.strCity);jce_copy_struct(a.strCountry,b.strCountry);jce_copy_struct(a.strSex,b.strSex);jce_copy_struct(a.cFrom,b.cFrom);jce_copy_struct(a.dwCreateTime,b.dwCreateTime);jce_copy_struct(a.dwStatus,b.dwStatus);jce_copy_struct(a.lGiftPoint,b.lGiftPoint);

#define m101Rank_STFansInfo_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.stPersonData,b.stPersonData);jce_copy_struct(a.iVoteNum,b.iVoteNum);jce_copy_struct(a.iRank,b.iRank);jce_copy_struct(a.strPersonalLink,b.strPersonalLink);jce_copy_struct(a.stFavoriteStar,b.stFavoriteStar);jce_copy_struct(a.iReceiveNum,b.iReceiveNum);

#define m101Rank_st101FansRankMenuReq_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.iRankType,b.iRankType);jce_copy_struct(a.iTimeType,b.iTimeType);jce_copy_struct(a.lRankTime,b.lRankTime);jce_copy_struct(a.iReqNum,b.iReqNum);jce_copy_struct(a.iPage,b.iPage);jce_copy_struct(a.lStarId,b.lStarId);

#define m101Rank_st101FansRankMenuRsp_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.iErrCode,b.iErrCode);jce_copy_struct(a.strErrMsg,b.strErrMsg);jce_copy_struct(a.iPeriod,b.iPeriod);jce_copy_struct(a.stBannerImg,b.stBannerImg);jce_copy_struct(a.vecUserInfo,b.vecUserInfo);jce_copy_struct(a.iNextPage,b.iNextPage);

#define m101Rank_st101ShareFlowReq_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.strStarid,b.strStarid);jce_copy_struct(a.strSrcVuid,b.strSrcVuid);jce_copy_struct(a.strCardId,b.strCardId);

#define m101Rank_st101ShareFlowRsp_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.iErrCode,b.iErrCode);jce_copy_struct(a.strErrMsg,b.strErrMsg);jce_copy_struct(a.vecUserInfo,b.vecUserInfo);jce_copy_struct(a.iTotalSupNum,b.iTotalSupNum);

#define m101Rank_STGetUserInfoReq_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.lUserid,b.lUserid);jce_copy_struct(a.vecUserids,b.vecUserids);

#define m101Rank_STGetUserInfoRsp_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.iErrCode,b.iErrCode);jce_copy_struct(a.strErrMsg,b.strErrMsg);jce_copy_struct(a.mapUserInfo,b.mapUserInfo);



#endif
