// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 3.1.1.2 by WSRD Tencent.
// Generated from `qqvideo_follow.jce'
// **********************************************************************

#ifndef __QQVIDEO_FOLLOW_H_
#define __QQVIDEO_FOLLOW_H_

#include <map>
#include <string>
#include <vector>
#include "jce/Jce.h"
using namespace std;
#include "qqvideo_common.h"


namespace jce
{
    struct FollowRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FollowRequest";
        }
        static string MD5()
        {
            return "8d61b639cda77ea57102c6b9cbb5aca2";
        }
        FollowRequest()
        :fromUserId(""),toUserId(""),type(0)
        {
        }
        void resetDefautlt()
        {
            fromUserId = "";
            toUserId = "";
            type = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(fromUserId, 0);
            _os.write(toUserId, 1);
            _os.write(type, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(fromUserId, 0, true);
            _is.read(toUserId, 1, false);
            _is.read(type, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(fromUserId,"fromUserId");
            _ds.display(toUserId,"toUserId");
            _ds.display(type,"type");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(fromUserId, true);
            _ds.displaySimple(toUserId, true);
            _ds.displaySimple(type, false);
            return _os;
        }
    public:
        std::string fromUserId;
        std::string toUserId;
        taf::Int32 type;
    };
    inline bool operator==(const FollowRequest&l, const FollowRequest&r)
    {
        return l.fromUserId == r.fromUserId && l.toUserId == r.toUserId && l.type == r.type;
    }
    inline bool operator!=(const FollowRequest&l, const FollowRequest&r)
    {
        return !(l == r);
    }

    struct FollowResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.FollowResponse";
        }
        static string MD5()
        {
            return "fe3b74a9bc8fe9b14c77c67ae5e1918a";
        }
        FollowResponse()
        :errCode(0),followedNum(0),maxFollowNum(0)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            followedNum = 0;
            maxFollowNum = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(followedNum, 1);
            _os.write(maxFollowNum, 2);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(followedNum, 1, false);
            _is.read(maxFollowNum, 2, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(followedNum,"followedNum");
            _ds.display(maxFollowNum,"maxFollowNum");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(followedNum, true);
            _ds.displaySimple(maxFollowNum, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        taf::Int32 followedNum;
        taf::Int32 maxFollowNum;
    };
    inline bool operator==(const FollowResponse&l, const FollowResponse&r)
    {
        return l.errCode == r.errCode && l.followedNum == r.followedNum && l.maxFollowNum == r.maxFollowNum;
    }
    inline bool operator!=(const FollowResponse&l, const FollowResponse&r)
    {
        return !(l == r);
    }

    struct QueryFollowVppsRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.QueryFollowVppsRequest";
        }
        static string MD5()
        {
            return "325d87d477a8cf7a6468ed6bb39da964";
        }
        QueryFollowVppsRequest()
        :userId(""),pageContext("")
        {
        }
        void resetDefautlt()
        {
            userId = "";
            pageContext = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(userId, 0);
            _os.write(pageContext, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(userId, 0, true);
            _is.read(pageContext, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(userId,"userId");
            _ds.display(pageContext,"pageContext");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(userId, true);
            _ds.displaySimple(pageContext, false);
            return _os;
        }
    public:
        std::string userId;
        std::string pageContext;
    };
    inline bool operator==(const QueryFollowVppsRequest&l, const QueryFollowVppsRequest&r)
    {
        return l.userId == r.userId && l.pageContext == r.pageContext;
    }
    inline bool operator!=(const QueryFollowVppsRequest&l, const QueryFollowVppsRequest&r)
    {
        return !(l == r);
    }

    struct QueryFollowVppsResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.QueryFollowVppsResponse";
        }
        static string MD5()
        {
            return "308a6263b469a9c70d0c80d1256f8ba8";
        }
        QueryFollowVppsResponse()
        :errCode(0),pageContext(""),hasNextPage(true)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            pageContext = "";
            hasNextPage = true;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(vecVppUsers, 1);
            _os.write(pageContext, 2);
            _os.write(hasNextPage, 3);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(vecVppUsers, 1, true);
            _is.read(pageContext, 2, false);
            _is.read(hasNextPage, 3, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(vecVppUsers,"vecVppUsers");
            _ds.display(pageContext,"pageContext");
            _ds.display(hasNextPage,"hasNextPage");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(vecVppUsers, true);
            _ds.displaySimple(pageContext, true);
            _ds.displaySimple(hasNextPage, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        vector<jce::ActorInfo> vecVppUsers;
        std::string pageContext;
        taf::Bool hasNextPage;
    };
    inline bool operator==(const QueryFollowVppsResponse&l, const QueryFollowVppsResponse&r)
    {
        return l.errCode == r.errCode && l.vecVppUsers == r.vecVppUsers && l.pageContext == r.pageContext && l.hasNextPage == r.hasNextPage;
    }
    inline bool operator!=(const QueryFollowVppsResponse&l, const QueryFollowVppsResponse&r)
    {
        return !(l == r);
    }

    struct QueryFansListRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.QueryFansListRequest";
        }
        static string MD5()
        {
            return "325d87d477a8cf7a6468ed6bb39da964";
        }
        QueryFansListRequest()
        :userId(""),pageContext("")
        {
        }
        void resetDefautlt()
        {
            userId = "";
            pageContext = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(userId, 0);
            _os.write(pageContext, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(userId, 0, true);
            _is.read(pageContext, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(userId,"userId");
            _ds.display(pageContext,"pageContext");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(userId, true);
            _ds.displaySimple(pageContext, false);
            return _os;
        }
    public:
        std::string userId;
        std::string pageContext;
    };
    inline bool operator==(const QueryFansListRequest&l, const QueryFansListRequest&r)
    {
        return l.userId == r.userId && l.pageContext == r.pageContext;
    }
    inline bool operator!=(const QueryFansListRequest&l, const QueryFansListRequest&r)
    {
        return !(l == r);
    }

    struct QueryFansListResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.QueryFansListResponse";
        }
        static string MD5()
        {
            return "308a6263b469a9c70d0c80d1256f8ba8";
        }
        QueryFansListResponse()
        :errCode(0),pageContext(""),hasNextPage(true)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            pageContext = "";
            hasNextPage = true;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(vecFollowedUsers, 1);
            _os.write(pageContext, 2);
            _os.write(hasNextPage, 3);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(vecFollowedUsers, 1, true);
            _is.read(pageContext, 2, false);
            _is.read(hasNextPage, 3, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(vecFollowedUsers,"vecFollowedUsers");
            _ds.display(pageContext,"pageContext");
            _ds.display(hasNextPage,"hasNextPage");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(vecFollowedUsers, true);
            _ds.displaySimple(pageContext, true);
            _ds.displaySimple(hasNextPage, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        vector<jce::ActorInfo> vecFollowedUsers;
        std::string pageContext;
        taf::Bool hasNextPage;
    };
    inline bool operator==(const QueryFansListResponse&l, const QueryFansListResponse&r)
    {
        return l.errCode == r.errCode && l.vecFollowedUsers == r.vecFollowedUsers && l.pageContext == r.pageContext && l.hasNextPage == r.hasNextPage;
    }
    inline bool operator!=(const QueryFansListResponse&l, const QueryFansListResponse&r)
    {
        return !(l == r);
    }

    struct CheckFollowRelationRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.CheckFollowRelationRequest";
        }
        static string MD5()
        {
            return "89945aac2d55c98d9c975caff4249a94";
        }
        CheckFollowRelationRequest()
        :fromUserId("")
        {
        }
        void resetDefautlt()
        {
            fromUserId = "";
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(fromUserId, 0);
            _os.write(toUsers, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(fromUserId, 0, true);
            _is.read(toUsers, 1, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(fromUserId,"fromUserId");
            _ds.display(toUsers,"toUsers");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(fromUserId, true);
            _ds.displaySimple(toUsers, false);
            return _os;
        }
    public:
        std::string fromUserId;
        vector<std::string> toUsers;
    };
    inline bool operator==(const CheckFollowRelationRequest&l, const CheckFollowRelationRequest&r)
    {
        return l.fromUserId == r.fromUserId && l.toUsers == r.toUsers;
    }
    inline bool operator!=(const CheckFollowRelationRequest&l, const CheckFollowRelationRequest&r)
    {
        return !(l == r);
    }

    struct CheckFollowRelationResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.CheckFollowRelationResponse";
        }
        static string MD5()
        {
            return "d4f4575804eade845ba31a4986942ab4";
        }
        CheckFollowRelationResponse()
        :errCode(0),followedNum(0),maxFollowNum(0)
        {
        }
        void resetDefautlt()
        {
            errCode = 0;
            followedNum = 0;
            maxFollowNum = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(errCode, 0);
            _os.write(mapVppFollowed, 1);
            _os.write(followedNum, 2);
            _os.write(maxFollowNum, 3);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(mapVppFollowed, 1, true);
            _is.read(followedNum, 2, false);
            _is.read(maxFollowNum, 3, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(mapVppFollowed,"mapVppFollowed");
            _ds.display(followedNum,"followedNum");
            _ds.display(maxFollowNum,"maxFollowNum");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(mapVppFollowed, true);
            _ds.displaySimple(followedNum, true);
            _ds.displaySimple(maxFollowNum, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        map<std::string, taf::Int32> mapVppFollowed;
        taf::Int32 followedNum;
        taf::Int32 maxFollowNum;
    };
    inline bool operator==(const CheckFollowRelationResponse&l, const CheckFollowRelationResponse&r)
    {
        return l.errCode == r.errCode && l.mapVppFollowed == r.mapVppFollowed && l.followedNum == r.followedNum && l.maxFollowNum == r.maxFollowNum;
    }
    inline bool operator!=(const CheckFollowRelationResponse&l, const CheckFollowRelationResponse&r)
    {
        return !(l == r);
    }

    struct MyRelationPageRequest : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.MyRelationPageRequest";
        }
        static string MD5()
        {
            return "0d2ce2bfc4eeee1ad30063b5f17a7f57";
        }
        MyRelationPageRequest()
        :userId(""),scene(0)
        {
        }
        void resetDefautlt()
        {
            userId = "";
            scene = 0;
        }
        template<typename WriterT>
        void writeTo(taf::JceOutputStream<WriterT>& _os) const
        {
            _os.write(userId, 0);
            _os.write(scene, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(userId, 0, true);
            _is.read(scene, 1, false);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(userId,"userId");
            _ds.display(scene,"scene");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(userId, true);
            _ds.displaySimple(scene, false);
            return _os;
        }
    public:
        std::string userId;
        taf::Int32 scene;
    };
    inline bool operator==(const MyRelationPageRequest&l, const MyRelationPageRequest&r)
    {
        return l.userId == r.userId && l.scene == r.scene;
    }
    inline bool operator!=(const MyRelationPageRequest&l, const MyRelationPageRequest&r)
    {
        return !(l == r);
    }

    struct MyRelationPageResponse : public taf::JceStructBase
    {
    public:
        static string className()
        {
            return "jce.MyRelationPageResponse";
        }
        static string MD5()
        {
            return "f0299aa92a2d158f92662542db1d9017";
        }
        MyRelationPageResponse()
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
            _os.write(moduleList, 1);
        }
        template<typename ReaderT>
        void readFrom(taf::JceInputStream<ReaderT>& _is)
        {
            resetDefautlt();
            _is.read(errCode, 0, true);
            _is.read(moduleList, 1, true);
        }
        ostream& display(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.display(errCode,"errCode");
            _ds.display(moduleList,"moduleList");
            return _os;
        }
        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            taf::JceDisplayer _ds(_os, _level);
            _ds.displaySimple(errCode, true);
            _ds.displaySimple(moduleList, false);
            return _os;
        }
    public:
        taf::Int32 errCode;
        vector<jce::LiveTabModuleInfo> moduleList;
    };
    inline bool operator==(const MyRelationPageResponse&l, const MyRelationPageResponse&r)
    {
        return l.errCode == r.errCode && l.moduleList == r.moduleList;
    }
    inline bool operator!=(const MyRelationPageResponse&l, const MyRelationPageResponse&r)
    {
        return !(l == r);
    }


}

#define jce_FollowRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.fromUserId,b.fromUserId);jce_copy_struct(a.toUserId,b.toUserId);jce_copy_struct(a.type,b.type);

#define jce_FollowResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.followedNum,b.followedNum);jce_copy_struct(a.maxFollowNum,b.maxFollowNum);

#define jce_QueryFollowVppsRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.userId,b.userId);jce_copy_struct(a.pageContext,b.pageContext);

#define jce_QueryFollowVppsResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.vecVppUsers,b.vecVppUsers);jce_copy_struct(a.pageContext,b.pageContext);jce_copy_struct(a.hasNextPage,b.hasNextPage);

#define jce_QueryFansListRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.userId,b.userId);jce_copy_struct(a.pageContext,b.pageContext);

#define jce_QueryFansListResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.vecFollowedUsers,b.vecFollowedUsers);jce_copy_struct(a.pageContext,b.pageContext);jce_copy_struct(a.hasNextPage,b.hasNextPage);

#define jce_CheckFollowRelationRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.fromUserId,b.fromUserId);jce_copy_struct(a.toUsers,b.toUsers);

#define jce_CheckFollowRelationResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.mapVppFollowed,b.mapVppFollowed);jce_copy_struct(a.followedNum,b.followedNum);jce_copy_struct(a.maxFollowNum,b.maxFollowNum);

#define jce_MyRelationPageRequest_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.userId,b.userId);jce_copy_struct(a.scene,b.scene);

#define jce_MyRelationPageResponse_JCE_COPY_STRUCT_HELPER   \
        jce_copy_struct(a.errCode,b.errCode);jce_copy_struct(a.moduleList,b.moduleList);



#endif
