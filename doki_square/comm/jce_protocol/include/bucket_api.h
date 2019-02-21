#ifndef _BUCKET_API_H_
#define _BUCKET_API_H_

// ---------------------------------------------------------------------------------------
//
// 使用说明：
// 1. 在进程初始化时调用 qqvideo::api::BucketApi::Init(); // 若初始化失败，将获取不到策略ID
// 2. 在业务中调用 qqvideo::api::BucketApi::GetStrategyId(a, b); 获取策略ID
//
// ---------------------------------------------------------------------------------------

// 依赖统一配置API库
// http://tc-svn.tencent.com/isd/isd_liveportalvideo_rep/qqliveSvr_proj/trunk/tenvideo_base/video_base_platform_prj/unified_config/api/
#include "UnifiedConfigAPI.h"

#include <string>
#include <vector>
#include <map>

namespace qqvideo
{
namespace api
{

using namespace std;

enum BUCKET_ERR
{
    BUCKET_ERR_CONFIG_NULL = 20,
    BUCKET_ERR_CONFIG_NOT_NULL = 21,
    BUCKET_ERR_CONFIG_EMPTY = 22,
    BUCKET_ERR_CONFIG_ALREADY_INIT = 23,
};

typedef UnifiedConfigAPI* PstUnifiedConfigAPI;

__inline__ __attribute__((always_inline)) unsigned int GetBucketOffset() {
    return 100000000;
}
__inline__ __attribute__((always_inline)) unsigned int GetRealBucketId(unsigned int bucket_id) {
    return bucket_id % GetBucketOffset();
}
__inline__ __attribute__((always_inline)) unsigned int GetBucketFlag(unsigned int bucket_id) {
    return bucket_id / GetBucketOffset();
}
// 基于平台分桶id判断是否需要灰度路由
__inline__ __attribute__((always_inline)) bool IsBucketGray(unsigned int bucket_id) {
    return !!(GetBucketFlag(bucket_id) & 0x01);
}
// @brief 基于平台分桶id判断是否需要打染色日志
// @brief bucket_id获取方法 pstVideoPacket->getQUAInfo().extentData.bucketInfo.bucketId
// @params bucket_id 平台分桶id
// @return true: 需要打染色日志
//         false：不需要打染色日志
__inline__ __attribute__((always_inline)) bool IsBucketColorLog(unsigned int bucket_id) {
    int bi = GetRealBucketId(bucket_id);
    return bi <= 4 && bi > 0;
}

class BucketApi
{
public:
    // @brief 基于桶ID和场景ID获取策略ID
    // @params dwBucketId 桶ID
    // @params dwScenceId 场景ID
    // @params bGray 返回值，是否灰度路由
    // @return dwStrategyId 策略ID，若为0表示默认策略
    static int GetStrategyId(const unsigned int dwBucketId, const unsigned int dwScenceId, bool &bGray)
    {
        // 桶ID的高位用作标记位
        // 先除1个亿，再来按位运算
        //bGray = !!((dwBucketId / 100000000) & 0x01);
        bGray = IsBucketGray(dwBucketId);
        return GetStrategyId(dwBucketId, dwScenceId);
    }
    // @brief 基于桶ID和场景ID获取策略ID
    // @params dwBucketId 桶ID
    // @params dwScenceId 场景ID
    // @return dwStrategyId 策略ID，若为0表示默认策略
    static int GetStrategyId(const unsigned int dwBucketId, const unsigned int dwScenceId)
    {
        int iRet = 0;

        PstUnifiedConfigAPI &pstConfig = Instance();

        // 若未初始化，则初始化
        if (NULL == pstConfig)
        {
            iRet = Init();
        }

        if (0 != iRet)
        {
            Myerrno() = iRet;
            return 0;
        }

        char buf[64];
        snprintf(buf, sizeof(buf), "c_bucket_id=%d&c_scence_id=%d", 
                 GetRealBucketId(dwBucketId), dwScenceId);

        std::vector<std::map<std::string, std::string> > v;
        iRet = pstConfig->get(buf, v);
        
        if (0 != iRet || v.empty())
        {
            Myerrno() = iRet;
            return 0;
        }

        Myerrno() = 0;

        return atoi(v[0]["c_strategy_id"].c_str());
    }

    // @brief 初始化，在业务进程初始化时调用该函数
    // @return 0 成功, !=0 失败
    static int Init()
    {
        // 是否已经初始化，只初始化一次
        static bool bInited = false;
        if (bInited)
            return BUCKET_ERR_CONFIG_ALREADY_INIT;

        bInited = true;

        PstUnifiedConfigAPI &pstConfig = Instance();
        if (NULL != pstConfig)
            return BUCKET_ERR_CONFIG_NOT_NULL;

        pstConfig = new UnifiedConfigAPI;
        return pstConfig->init(99);
    }

    // @breif 保存计算过程中的错误码，非线程安全
    static const int& Errno()
    {
        return Myerrno();
    }

private:
    BucketApi();
    BucketApi(const BucketApi&);
    BucketApi& operator = (const BucketApi&);

    // @breif 单件
    static PstUnifiedConfigAPI& Instance()
    {
        static PstUnifiedConfigAPI pstConfig = NULL;
        return pstConfig;
    }

    // @breif 保存计算过程中的错误码，非线程安全
    static int& Myerrno()
    {
        static int dwErrno = 0;
        return dwErrno;
    }
}; // endof class BucketApi

class StrategyApi
{
public:
    // @brief 基于桶ID和获取所有策略ID
    // @params dwBucketId 桶ID
    // @return strStrategyId 策略ID列表，以“+”分隔，若为空，表示该桶没有映射策略
    static std::string GetStrategyId(const int dwBucketId)
    {
        int iRet = 0;
        PstUnifiedConfigAPI &pstConfig = Instance();

        // 若未初始化，则初始化
        if (NULL == pstConfig)
        {
            iRet = Init();
        }

        if (0 != iRet)
        {
            Myerrno() = iRet;
            return "";
        }

        char buf[32];
        snprintf(buf, sizeof(buf), "c_bucket_id=%d", dwBucketId % 100000000);

        std::vector<std::map<std::string, std::string> > v;
        iRet = pstConfig->get(buf, v);
        
        if (0 != iRet || v.empty())
        {
            Myerrno() = iRet;
            return "";
        }

        Myerrno() = 0;

        return v[0]["c_strategy_id"];
    }

    // @brief 初始化，在业务进程初始化时调用该函数
    // @return 0 成功, !=0 失败
    static int Init()
    {
        // 是否已经初始化，只初始化一次
        static bool bInited = false;
        if (bInited)
            return BUCKET_ERR_CONFIG_ALREADY_INIT;

        bInited = true;

        PstUnifiedConfigAPI &pstConfig = Instance();
        if (NULL != pstConfig)
            return BUCKET_ERR_CONFIG_NOT_NULL;

        pstConfig = new UnifiedConfigAPI;
        return pstConfig->init(124);
    }

    // @breif 保存计算过程中的错误码，非线程安全
    static const int& Errno()
    {
        return Myerrno();
    }

private:
    StrategyApi();
    StrategyApi(const StrategyApi&);
    StrategyApi& operator = (const StrategyApi&);

    // @breif 单件
    static PstUnifiedConfigAPI& Instance()
    {
        static PstUnifiedConfigAPI pstConfig = NULL;
        return pstConfig;
    }

    // @breif 保存计算过程中的错误码，非线程安全
    static int& Myerrno()
    {
        static int dwErrno = 0;
        return dwErrno;
    }
}; // endof class BucketApi


} // endof namespace api
} // endof namesapce qqvideo

#endif
