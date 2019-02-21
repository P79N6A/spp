// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 3.1.1.2 by WSRD Tencent.
// Generated from `starCard.jce'
// **********************************************************************

#ifndef __STARCARD_H_
#define __STARCARD_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;
#include "qqvideo_fantuan.h"


namespace jce
{
    struct GetStarCardDyStateRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.GetStarCardDyStateRequest";
        }
        static string MD5()
        {
            return "2e45701425f70f0e5c722cbe2c3f508e";
        }
        GetStarCardDyStateRequest()
        :strDataKey("")
        {
        }
        void resetDefautlt()
        {
            strDataKey = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(strDataKey, 0);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(strDataKey, 0, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(strDataKey,"strDataKey");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(strDataKey, false);
            return _os;
        }
    public:
        std::string strDataKey;
    };
    inline bool operator==(const GetStarCardDyStateRequest&l, const GetStarCardDyStateRequest&r)
    {
        return l.strDataKey == r.strDataKey;
    }
    inline bool operator!=(const GetStarCardDyStateRequest&l, const GetStarCardDyStateRequest&r)
    {
        return !(l == r);
    }

    struct GetStarCardDyStateResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.GetStarCardDyStateResponse";
        }
        static string MD5()
        {
            return "069057875cfa3e670bf112127ddcb024";
        }
        GetStarCardDyStateResponse()
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
            _os.write(starCardList, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(starCardList, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(starCardList,"starCardList");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(starCardList, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        jce::ONADokiMovementCardList starCardList;
    };
    inline bool operator==(const GetStarCardDyStateResponse&l, const GetStarCardDyStateResponse&r)
    {
        return l.errCode == r.errCode && l.starCardList == r.starCardList;
    }
    inline bool operator!=(const GetStarCardDyStateResponse&l, const GetStarCardDyStateResponse&r)
    {
        return !(l == r);
    }


}

#define jce_GetStarCardDyStateRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.strDataKey,b.strDataKey);

#define jce_GetStarCardDyStateResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.starCardList,b.starCardList);



#endif
