// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 3.1.1.2 by WSRD Tencent.
// Generated from `snowball.jce'
// **********************************************************************

#ifndef __SNOWBALL_H_
#define __SNOWBALL_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;


namespace jce
{
    struct stSnowBallRsp : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.stSnowBallRsp";
        }
        static string MD5()
        {
            return "26159cf77dcb9fe1c64b57dae580ccae";
        }
        stSnowBallRsp()
        :errcode(0),errmsg("")
        {
        }
        void resetDefautlt()
        {
            errcode = 0;
            errmsg = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errcode, 0);
            _os.write(errmsg, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errcode, 0, false);
            _is.read(errmsg, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errcode,"errcode");
            _ds.display(errmsg,"errmsg");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errcode, true);
            _ds.displaySimple(errmsg, false);
            return _os;
        }
    public:
        taf::Int32 errcode;
        std::string errmsg;
    };
    inline bool operator==(const stSnowBallRsp&l, const stSnowBallRsp&r)
    {
        return l.errcode == r.errcode && l.errmsg == r.errmsg;
    }
    inline bool operator!=(const stSnowBallRsp&l, const stSnowBallRsp&r)
    {
        return !(l == r);
    }


}

#define jce_stSnowBallRsp_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errcode,b.errcode);jce_copy_struct(a.errmsg,b.errmsg);



#endif