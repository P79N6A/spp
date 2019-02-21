/*
 *  Filename: task_operate_redis.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 通用的请求redis数据的task
 *
 */

#ifndef __TASK_OPERATE_REDIS_H__
#define __TASK_OPERATE_REDIS_H__

#include "task_base.h"

extern "C"
{
#include "hiredis.h"
}

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

#define CHECK_PARAM_RETWHENERR(bCheck) \
do{\
    if(bCheck)\
    {\
        SPP_ERROR_LOG("optRedisTask tName[%s]tKey[%s]: paramErr, check[%s]",GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),#bCheck);\
        return ERR_TASK_PARAM;\
    }\
}while(0)

//错误码
enum EM_REDIS_ERROR
{
    ERR_PARAM_INVALID = -1001, //API参数非法
    ERR_CONN_FAILED   = -1002, //连接失败
    ERR_DATA_EMPTY    = -1003, //数据为空
    ERR_DATA_ERROR    = -1004, //获取失败
    ERR_ENCODE_FAILED = -1005, //数据打包失败
    ERR_DECODE_FAILED = -1006, //数据解包失败
};

//redis操作类型（task内部使用）
enum EM_REDIS_OPT_TYPE
{
    OPT_UNDEF_TTYPE                      = 0,

    OPT_STR_SET                          = 101, //STRING 字符串 101-199
    OPT_STR_SETNX                        = 102,
    OPT_STR_SETEX                        = 103,
    OPT_STR_SETEXNX                      = 104, //Redis2.6.12及之后的版本才支持
    OPT_STR_PSETEX                       = 105,
    OPT_STR_GET                          = 106,
    OPT_STR_GETBATCH                     = 107,
    OPT_STR_INCR                         = 108,
    OPT_STR_INCRBY                       = 109,
    OPT_STR_DECR                         = 110,
    OPT_STR_DECRBY                       = 111,

    OPT_LIST_RPUSH                       = 201, //LIST 列表 201-299
    OPT_LIST_LPUSH                       = 202,
    OPT_LIST_RPOP                        = 203,
    OPT_LIST_LPOP                        = 204,
    OPT_LIST_GETLIST_BYRANK              = 205,
    OPT_LIST_GET_BYINDEX                 = 206,
    OPT_LIST_COUNT                       = 207,
    OPT_LIST_RESIZE_TRIM                 = 208,

    OPT_SET_ADD                          = 301, //SET 集合 301-399
    OPT_SET_ADDBATCH                     = 302,
    OPT_SET_COUNT                        = 303,
    OPT_SET_GETMEM_RANDOM                = 304,
    OPT_SET_GETPOP                       = 305,
    OPT_SET_GETALL                       = 306,
    OPT_SET_DELMEM                       = 307,
    OPT_SET_DELMEM_BATCH                 = 308,
    OPT_SET_ISMEM                        = 309,

    OPT_SSET_ADD                         = 401, //SSET 有序集合 401-499
    OPT_SSET_DELMEM                      = 402,
    OPT_SSET_DELMEM_BATCH                = 403,
    OPT_SSET_DELLIST_BYRANK              = 404,
    OPT_SSET_DELLIST_BYSCORE             = 405,
    OPT_SSET_ADDBATCH_SCOREVALUE         = 406,
    OPT_SSET_INCR_MEMSCORE               = 407,
    OPT_SSET_GETLIST_VALUE_BYRANK        = 408,
    OPT_SSET_GETLIST_VALUE_BYSCORE       = 409,
    OPT_SSET_GETLIST_VALUE_BYSCORE_LIMIT = 410,
    OPT_SSET_GETLIST_VALUESCORE_BYRANK   = 411,
    OPT_SSET_GETLIST_VALUESCORE_BYSCORE  = 412,
    OPT_SSET_COUNT                       = 413,
    OPT_SSET_COUNT_BYSCORE               = 414,
    OPT_SSET_GETRANK                     = 415,
    OPT_SSET_GETSCORE                    = 416,

    OPT_HASH_ADD                         = 501, //HASH 哈希表 501-599
    OPT_HASH_ADDBATCH                    = 502,
    OPT_HASH_GET_VALUE_BYFIELD           = 503,
    OPT_HASH_INCRVALUE_BYFIELD           = 504,
    OPT_HASH_GETALL_FIELD                = 505,
    OPT_HASH_GETALL_VALUE                = 506,
    OPT_HASH_GETBATCH_VALUE_BYFIELD      = 507,
    OPT_HASH_GETALL_FIELDVALUE           = 508,
    OPT_HASH_COUNT                       = 509,
    OPT_HASH_DELBATCH                    = 510,
    OPT_HASH_ISEXIST_FIELD               = 511,

    OPT_KEY_ISEXIST                      = 601, //KEY 键 601-699
    OPT_KEY_SETEXP                       = 602,
    OPT_KEY_SETEXP_MS                    = 603,
    OPT_KEY_DEL                          = 604,
    OPT_KEY_DELBATCH                     = 605,

};

//回包数据类型
enum EM_REDIS_RSPDATA_TYPE
{
    REDIS_DTYPE_UNDEF                = 0, //未定义类型
    REDIS_DTYPE_EMPTY                = 1, //数据为空
    REDIS_DTYPE_INT                  = 2, //整型
    REDIS_DTYPE_STR                  = 3, //字符串
    REDIS_DTYPE_VEC_STR              = 4, //vector<string>
    REDIS_DTYPE_VEC_PAIR_VALUE_SCORE = 5, //vector<pair<string, string> > vecPairValueScore
    REDIS_DTYPE_MAP_FIELD_VALUE      = 6, //map<string,string> mapFieldValue
};

//redis统一回包数据
struct StRedisRspData
{
    EM_REDIS_RSPDATA_TYPE eRedisDataType; //根据这个类型来解析数据

    long long                     llResult;
    string                        strResult;
    vector<string>                vecStrResult;
    vector<pair<string, string> > vecPairValueScore; //vector(pair(value--score))
    map<string,string>            mapFieldValue;     //map(field--value)

    StRedisRspData()
    {
        eRedisDataType = REDIS_DTYPE_UNDEF;
        llResult       = -1;
    };
    ~StRedisRspData()
    {

    };
};

class COperateRedis : public CTaskBase
{
  public:
    //eRecvBufLen接受数据默认初始buf，不够时spp框架会自动扩展的；为了避免多次重新分配，默认初始buf应该和平均真实包大小相当
    COperateRedis(ENUM_TASK_TYPE eTaskType, ENUM_TASK_BUFLEN eRecvBufLen = BUF_LEN_4K);
    ~COperateRedis();

    //redis包完整性校验函数
    static int CheckRedisPkt(const char *buf, int len);

    //redis请求打包函数--分命令定义打包函数，使调用者不必关心如何拼接各种redis命令，直接调用对应函数即可。
    int SetReq_Str_Add(const string &strKey, const string &strValue);
    int SetReq_Str_AddNX(const string &strKey, const string &strValue);
    int SetReq_Str_AddEX(const string &strKey, const string &strValue, long long llExpTime_s);
    int SetReq_Str_AddNXEX(const string &strKey, const string &strValue, long long llExpTime_s); //Redis2.6.12及之后的版本才支持(视频redis不支持，使用SetReq_Str_AddNXEX_TVideo)
    int SetReq_Str_AddNXEX_TVideo(const string &strKey, const string &strValue, long long llExpTime_s); //腾讯视频自建redis不支持标准redis的扩展SET操作，它支持使用setnxex命令来做set NX EX操作
    int SetReq_Str_AddPEX(const string &strKey, const string &strValue, long long llExpTime_ms);
    int SetReq_Str_Get(const string& strKey);
    int SetReq_Str_GetBatch(const vector<string> &vecRedisKey);
    int SetReq_Str_Incr(const string &strKey);
    int SetReq_Str_IncrBy(const string &strKey, long long llNum);
    int SetReq_Str_Decr(const string &strKey);
    int SetReq_Str_DecrBy(const string &strKey, long long llNum);

    int SetReq_List_RPush(const string &strKey, const string &strValue);
    int SetReq_List_LPush(const string &strKey, const string &strValue);
    int SetReq_List_RPop(const string &strKey);
    int SetReq_List_LPop(const string &strKey);
    int SetReq_List_GetRangeByRank(const string &strKey, int iStart, int iEnd);
    int SetReq_List_GetValueByIndex(const string &strKey, int index);
    int SetReq_List_Count(const string &strKey);
    int SetReq_List_ResizeTrim(const string &strKey, int iStart, int iEnd);

    int SetReq_Set_Add(const string &strKey, const string &strValue);
    int SetReq_Set_AddBatch(const string &strKey, const vector<string> &vecMember);
    int SetReq_Set_Count(const string &strKey);
    int SetReq_Set_GetRandom(const string &strKey);
    int SetReq_Set_GetPop(const string &strKey);
    int SetReq_Set_GetAll(const string &strKey);
    int SetReq_Set_DelMem(const string &strKey, const string &strMember);
    int SetReq_Set_DelMemBatch(const string &strKey, const vector<string> &vecMember);
    int SetReq_Set_IsMem(const string &strKey, const string &strMember);

    int SetReq_SortSet_Add(const string &strKey, const string &strScore, const string &strMember);
    int SetReq_SortSet_DelMem(const string &strKey,const string &strMember);
    int SetReq_SortSet_DelMemBatch(const string &strKey, const vector<string> &vecMember);
    int SetReq_SortSet_DelRange_ByRank(const string &strKey, int iStart, int iEnd);
    int SetReq_SortSet_DelRange_ByScore(const string &strKey, const string &strStart, const string &strEnd);
    int SetReq_SortSet_AddBatch_ScoreValue(const string &strKey, map<string,string> &mapScoreValue);
    int SetReq_SortSet_IncrBy_MemScore(const string &strKey, long long llNum, const string &strMember);
    int SetReq_SortSet_GetRange_Value_ByRank(const string &strKey, int iStart, int iEnd, bool bDesc = true);
    int SetReq_SortSet_GetRange_Value_ByScore(const string &strKey, const string &strStart, const string &strEnd, bool bDesc = true);
    int SetReq_SortSet_GetRange_Value_ByScore_limit(const string &strKey, const string &strStart, const string &strEnd, int iOffset, int iCount, bool bDesc = true);
    int SetReq_SortSet_GetRange_ValueScore_ByRank(const string &strKey, int iStart, int iEnd, bool bDesc = true);
    int SetReq_SortSet_GetRange_ValueScore_ByScore(const string &strKey, const string &strStart, const string &strEnd, bool bDesc = true);
    int SetReq_SortSet_Count(const string &strKey);
    int SetReq_SortSet_Count_ByScore(const string &strKey, const string &strStart, const string &strEnd);
    int SetReq_SortSet_GetRank(const string &strKey, const string &strMember, bool bDesc = true);
    int SetReq_SortSet_GetScore(const string &strKey, const string &strMember);

    int SetReq_Hash_Add(const string &strKey, const string &strField, const string &strValue);
    int SetReq_Hash_AddBatch(const string &strKey, map<string, string> &mapFieldValue);
    int SetReq_Hash_GetValue_ByField(const string &strKey, const string &strField);
    int SetReq_Hash_IncrValue_ByField(const string &strKey, const string &strField,long long llNum);
    int SetReq_Hash_GetAllField(const string &strKey);
    int SetReq_Hash_GetAllValue(const string &strKey);
    int SetReq_Hash_GetBatchValue_ByField(const string &strKey, const vector<string> &vecField);
    int SetReq_Hash_GetAllFieldValue(const string &strKey);
    int SetReq_Hash_Count(const string &strKey);
    int SetReq_Hash_DelBatch(const string &strKey, const vector<string> &vecField);
    int SetReq_Hash_IsExist_Field(const string &strKey, const string &strField);

    int SetReq_Key_IsExist(const string &strKey);
    int SetReq_Key_SetExp(const string &strKey, long long llExpTime_s);
    int SetReq_Key_SetExp_MS(const string &strKey, long long llExpTime_ms);
    int SetReq_Key_Del(const string &strKey);
    int SetReq_Key_DelBatch(const vector<string> &vecKey);

    //redis回报解析函数
    const StRedisRspData& GetRspData()
    {
        return m_stRedisRspData;
    }

    // ====spp微线程框架的执行函数============================================
    virtual int Process();

  private:
    bool CheckStrParamIsInt(const string &strData);
    bool CheckStrParamIsDouble(const string &strData); //只判断XX.XX这种形式的浮点型，科学计数类型的不处理（3.4E+38）
    bool CheckStrParamIsMaxMin(const string &strData);
    bool CheckValidScore(const string &strData);

    // 请求打包函数
    int EncodeRedisCmd(const char *format, ...);
    int EncodeRedisCmdArray(const string &strCmd,const map<string, string> &mapKV);
    int EncodeRedisCmdArray(const string &strCmd,const vector<string> &vecV);
    int EncodeRedisCmdArray(const string &strCmd,const string &strReidsKey,const map<string, string> &mapKV);
    int EncodeRedisCmdArray(const string &strCmd,const string &strReidsKey,const vector<string> &vecV);

    // 回包解析
    int DecodeRedisResult(const char *buf, int len);
    int RetErrAndFreeRedis(int iError, const string &strMsg, void *reply, redisReader *reader);

  private:
    EM_REDIS_OPT_TYPE m_eOptType;
    string            m_strReqData;
    StRedisRspData    m_stRedisRspData;

};

#endif /*__TASK_OPERATE_REDIS_H__*/
