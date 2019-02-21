/*
 *  Filename: task_operate_redis.cpp
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 通用的请求redis数据的task
 *
 */

#include "task_operate_redis.h"

static int CheckRedisPktSPP301(void *buf, int len, bool closed, void *msg_ctx, bool &msg_len_detected)
{
    return COperateRedis::CheckRedisPkt((char*)buf, len);
}

COperateRedis::COperateRedis(ENUM_TASK_TYPE eTaskType, ENUM_TASK_BUFLEN eRecvBufLen)
:m_eOptType(OPT_UNDEF_TTYPE)
{
    SetTaskType((int)eTaskType);
    SetRecvInitBufSize(eRecvBufLen);
    SetCheckFunc(CheckRedisPktSPP301);

    SetTaskInfo(g_oMgSvrConn.GetSvrConnInfo(GetTaskType()),g_oMgMcall.GetMcallInfo(GetTaskType()),g_oMgMoni.GetMoniInfo(GetTaskType()));
}

COperateRedis::~COperateRedis()
{
}

int COperateRedis::CheckRedisPkt(const char *buf, int len)
{
    if (buf == NULL || len < 3)
    {
        return -1;
    }

    if (buf[0] != '-' && buf[0] != '+' && buf[0] != ':' && buf[0] != '$' && buf[0] != '*')
    {
        return -2;
    }

    redisReader *reader = NULL;
    reader = redisReaderCreate();
    if (reader == NULL)
    {
        return -3;
    }

    if (redisReaderFeed(reader, buf, len) != REDIS_OK)
    {
        redisReaderFree(reader);
        reader = NULL;
        return -4;
    }

    void *aux = NULL;
    if (redisReaderGetReply(reader, &aux) != REDIS_OK)
    {
        redisReaderFree(reader);
        reader = NULL;
        return -5;
    }

    //do not receive finish
    if (aux == NULL)
    {
        redisReaderFree(reader);
        reader = NULL;
        return 0;
    }

    redisReply *reply = (redisReply*)aux;

    int real_len = -1;
    if (reader->pos == 0 && len != (int)reader->len)
        real_len = len - reader->len;
    else
        real_len = reader->pos;

    freeReplyObject(reply);
    redisReaderFree(reader);
    reader = NULL;

    return real_len;
}

int COperateRedis::SetReq_Str_Add(const string &strKey, const string &strValue)
{
    m_eOptType = OPT_STR_SET;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("SET %s %b", strKey.c_str(), strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_Str_AddNX(const string &strKey, const string &strValue)
{
    m_eOptType = OPT_STR_SETNX;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("SETNX %s %b", strKey.c_str(), strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_Str_AddEX(const string &strKey, const string &strValue, long long llExpTime_s)
{
    m_eOptType = OPT_STR_SETEX;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("SETEX %s %lld %b", strKey.c_str(), llExpTime_s, strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_Str_AddNXEX(const string &strKey, const string &strValue, long long llExpTime_s)
{
    m_eOptType = OPT_STR_SETEXNX;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("SET %s %b EX %lld NX", strKey.c_str(), strValue.c_str(), strValue.length(), llExpTime_s);
}

int COperateRedis::SetReq_Str_AddNXEX_TVideo(const string &strKey, const string &strValue, long long llExpTime_s)
{
    m_eOptType = OPT_STR_SETEXNX;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("SETNXEX %s %lld %b", strKey.c_str(), llExpTime_s, strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_Str_AddPEX(const string &strKey, const string &strValue, long long llExpTime_ms)
{
    m_eOptType = OPT_STR_PSETEX;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("PSETEX %s %lld %b", strKey.c_str(), llExpTime_ms, strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_Str_Get(const string& strKey)
{
    m_eOptType = OPT_STR_GET;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("GET %s", strKey.c_str());
}

int COperateRedis::SetReq_Str_GetBatch(const vector<string> &vecRedisKey)
{
    m_eOptType = OPT_STR_GETBATCH;

    CHECK_PARAM_RETWHENERR(vecRedisKey.empty());

    return EncodeRedisCmdArray("MGET", vecRedisKey);
}

int COperateRedis::SetReq_Str_Incr(const string &strKey)
{
    m_eOptType = OPT_STR_INCR;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("INCR %s", strKey.c_str());
}

int COperateRedis::SetReq_Str_IncrBy(const string &strKey, long long llNum)
{
    m_eOptType = OPT_STR_INCRBY;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("INCRBY %s %lld", strKey.c_str(), llNum);
}

int COperateRedis::SetReq_Str_Decr(const string &strKey)
{
    m_eOptType = OPT_STR_DECR;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("DECR %s", strKey.c_str());
}

int COperateRedis::SetReq_Str_DecrBy(const string &strKey, long long llNum)
{
    m_eOptType = OPT_STR_DECRBY;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("DECRBY %s %lld", strKey.c_str(), llNum);
}

int COperateRedis::SetReq_List_RPush(const string &strKey, const string &strValue)
{
    m_eOptType = OPT_LIST_RPUSH;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("RPUSH %s %b", strKey.c_str(), strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_List_LPush(const string &strKey, const string &strValue)
{
    m_eOptType = OPT_LIST_LPUSH;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("LPUSH %s %b", strKey.c_str(), strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_List_RPop(const string &strKey)
{
    m_eOptType = OPT_LIST_RPOP;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("RPOP %s", strKey.c_str());
}

int COperateRedis::SetReq_List_LPop(const string &strKey)
{
    m_eOptType = OPT_LIST_LPOP;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("LPOP %s", strKey.c_str());
}

int COperateRedis::SetReq_List_GetRangeByRank(const string &strKey, int iStart, int iEnd)
{
    m_eOptType = OPT_LIST_GETLIST_BYRANK;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("LRANGE %s %d %d", strKey.c_str(), iStart, iEnd);
}

int COperateRedis::SetReq_List_GetValueByIndex(const string &strKey, int index)
{
    m_eOptType = OPT_LIST_GET_BYINDEX;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("LINDEX %s %lld", strKey.c_str(), index);
}

int COperateRedis::SetReq_List_Count(const string &strKey)
{
    m_eOptType = OPT_LIST_COUNT;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("LLEN %s", strKey.c_str());
}

int COperateRedis::SetReq_List_ResizeTrim(const string &strKey, int iStart, int iEnd)
{
    m_eOptType = OPT_LIST_RESIZE_TRIM;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("LTRIM %s %d %d", strKey.c_str(), iStart, iEnd);
}

int COperateRedis::SetReq_Set_Add(const string &strKey, const string &strValue)
{
    m_eOptType = OPT_SET_ADD;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("SADD %s %b", strKey.c_str(), strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_Set_AddBatch(const string &strKey, const vector<string> &vecMember)
{
    m_eOptType = OPT_SET_ADDBATCH;

    CHECK_PARAM_RETWHENERR(vecMember.empty());

    return EncodeRedisCmdArray("SADD", strKey, vecMember);
}

int COperateRedis::SetReq_Set_Count(const string &strKey)
{
    m_eOptType = OPT_SET_COUNT;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("SCARD %s", strKey.c_str());
}

int COperateRedis::SetReq_Set_GetRandom(const string &strKey)
{
    m_eOptType = OPT_SET_GETMEM_RANDOM;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("SRANDMEMBER %s", strKey.c_str());
}

int COperateRedis::SetReq_Set_GetPop(const string &strKey)
{
    m_eOptType = OPT_SET_GETPOP;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("SPOP %s", strKey.c_str());
}

int COperateRedis::SetReq_Set_GetAll(const string &strKey)
{
    m_eOptType = OPT_SET_GETALL;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("SMEMBERS %s", strKey.c_str());
}

int COperateRedis::SetReq_Set_DelMem(const string &strKey, const string &strMember)
{
    m_eOptType = OPT_SET_DELMEM;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strMember.empty());

    return EncodeRedisCmd("SREM %s %b", strKey.c_str(), strMember.c_str(), strMember.length());
}

int COperateRedis::SetReq_Set_DelMemBatch(const string &strKey, const vector<string> &vecMember)
{
    m_eOptType = OPT_SET_DELMEM_BATCH;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(vecMember.empty());

    return EncodeRedisCmdArray("SREM", strKey, vecMember);
}

int COperateRedis::SetReq_Set_IsMem(const string &strKey, const string &strMember)
{
    m_eOptType = OPT_SET_ISMEM;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strMember.empty());

    return EncodeRedisCmd("SISMEMBER %s %b", strKey.c_str(), strMember.c_str(), strMember.length());
}

int COperateRedis::SetReq_SortSet_Add(const string &strKey, const string &strScore, const string &strMember)
{
    m_eOptType = OPT_SSET_ADD;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strScore));
    CHECK_PARAM_RETWHENERR(strMember.empty());

    return EncodeRedisCmd("ZADD %s %s %b", strKey.c_str(), strScore.c_str(), strMember.c_str(), strMember.length());
}

int COperateRedis::SetReq_SortSet_DelMem(const string &strKey,const string &strMember)
{
    m_eOptType = OPT_SSET_DELMEM;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strMember.empty());

    return EncodeRedisCmd("ZREM %s %b", strKey.c_str(), strMember.c_str(), strMember.length());
}

int COperateRedis::SetReq_SortSet_DelMemBatch(const string &strKey, const vector<string> &vecMember)
{
    m_eOptType = OPT_SSET_DELMEM_BATCH;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(vecMember.empty());

    return EncodeRedisCmdArray("ZREM", strKey, vecMember);
}

int COperateRedis::SetReq_SortSet_DelRange_ByRank(const string &strKey, int iStart, int iEnd)
{
    m_eOptType = OPT_SSET_DELLIST_BYRANK;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("ZREMRANGEBYRANK %s %d %d", strKey.c_str(), iStart, iEnd);
}

int COperateRedis::SetReq_SortSet_DelRange_ByScore(const string &strKey, const string &strStart, const string &strEnd)
{
    m_eOptType = OPT_SSET_DELLIST_BYSCORE;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strStart)); //判断是否是有效分数（整型、浮点数、-inf、+inf）
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strEnd));

    return EncodeRedisCmd("ZREMRANGEBYSCORE %s %s %s", strKey.c_str(), strStart.c_str(), strEnd.c_str());
}

int COperateRedis::SetReq_SortSet_AddBatch_ScoreValue(const string &strKey, map<string,string> &mapScoreValue)
{
    m_eOptType = OPT_SSET_ADDBATCH_SCOREVALUE;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(mapScoreValue.empty());

    return EncodeRedisCmdArray("ZADD", strKey, mapScoreValue);
}

int COperateRedis::SetReq_SortSet_IncrBy_MemScore(const string &strKey, long long llNum, const string &strMember)
{
    m_eOptType = OPT_SSET_INCR_MEMSCORE;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strMember.empty());

    return EncodeRedisCmd("ZINCRBY %s %lld %b", strKey.c_str(), llNum, strMember.c_str(), strMember.length());
}

int COperateRedis::SetReq_SortSet_GetRange_Value_ByRank(const string &strKey, int iStart, int iEnd, bool bDesc)
{
    m_eOptType = OPT_SSET_GETLIST_VALUE_BYRANK;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    if(bDesc)
        return EncodeRedisCmd("ZREVRANGE %s %d %d", strKey.c_str(), iStart, iEnd);
    else
        return EncodeRedisCmd("ZRANGE %s %d %d", strKey.c_str(), iStart, iEnd);
}

int COperateRedis::SetReq_SortSet_GetRange_Value_ByScore(const string &strKey, const string &strStart, const string &strEnd, bool bDesc)
{
    m_eOptType = OPT_SSET_GETLIST_VALUE_BYSCORE;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strStart)); //判断是否是有效分数（整型、浮点数、-inf、+inf）
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strEnd));

    if(bDesc)
        return EncodeRedisCmd("ZREVRANGEBYSCORE %s %s %s", strKey.c_str(), strStart.c_str(), strEnd.c_str());
    else
        return EncodeRedisCmd("ZRANGEBYSCORE %s %s %s", strKey.c_str(), strStart.c_str(), strEnd.c_str());
}

int COperateRedis::SetReq_SortSet_GetRange_Value_ByScore_limit(const string &strKey, const string &strStart, const string &strEnd, int iOffset, int iCount, bool bDesc)
{
    m_eOptType = OPT_SSET_GETLIST_VALUE_BYSCORE_LIMIT;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strStart)); //判断是否是有效分数（整型、浮点数、-inf、+inf）
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strEnd));

    if(bDesc)
        return EncodeRedisCmd("ZREVRANGEBYSCORE %s %s %s LIMIT %d %d", strKey.c_str(),strStart.c_str(),strEnd.c_str(),iOffset,iCount);
    else
        return EncodeRedisCmd("ZRANGEBYSCORE %s %s %s LIMIT %d %d", strKey.c_str(),strStart.c_str(),strEnd.c_str(),iOffset,iCount);
}

int COperateRedis::SetReq_SortSet_GetRange_ValueScore_ByRank(const string &strKey, int iStart, int iEnd, bool bDesc)
{
    m_eOptType = OPT_SSET_GETLIST_VALUESCORE_BYRANK;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    if(bDesc)
        return EncodeRedisCmd("ZREVRANGE %s %d %d WITHSCORES", strKey.c_str(), iStart, iEnd);
    else
        return EncodeRedisCmd("ZRANGE %s %d %d WITHSCORES", strKey.c_str(), iStart, iEnd);
}

int COperateRedis::SetReq_SortSet_GetRange_ValueScore_ByScore(const string &strKey, const string &strStart, const string &strEnd, bool bDesc)
{
    m_eOptType = OPT_SSET_GETLIST_VALUESCORE_BYSCORE;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strStart));//判断是否是有效分数（整型、浮点数、-inf、+inf）
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strEnd));

    if(bDesc)
        return EncodeRedisCmd("ZREVRANGEBYSCORE %s %s %s WITHSCORES", strKey.c_str(), strStart.c_str(), strEnd.c_str());
    else
        return EncodeRedisCmd("ZRANGEBYSCORE %s %s %s WITHSCORES", strKey.c_str(), strStart.c_str(), strEnd.c_str());
}

int COperateRedis::SetReq_SortSet_Count(const string &strKey)
{
    m_eOptType = OPT_SSET_COUNT;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("ZCARD %s", strKey.c_str());
}

int COperateRedis::SetReq_SortSet_Count_ByScore(const string &strKey, const string &strStart, const string &strEnd)
{
    m_eOptType = OPT_SSET_COUNT_BYSCORE;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strStart));//判断是否是有效分数（整型、浮点数、-inf、+inf）
    CHECK_PARAM_RETWHENERR(!CheckValidScore(strEnd));

    return EncodeRedisCmd("ZCOUNT %s %s %s", strKey.c_str(), strStart.c_str(), strEnd.c_str());
}

int COperateRedis::SetReq_SortSet_GetRank(const string &strKey, const string &strMember, bool bDesc)
{
    m_eOptType = OPT_SSET_GETRANK;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strMember.empty());

    if(bDesc)
        return EncodeRedisCmd("ZREVRANK %s %b", strKey.c_str(), strMember.c_str(), strMember.length());
    else
        return EncodeRedisCmd("ZRANK %s %b", strKey.c_str(), strMember.c_str(), strMember.length());
}

int COperateRedis::SetReq_SortSet_GetScore(const string &strKey, const string &strMember)
{
    m_eOptType = OPT_SSET_GETSCORE;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strMember.empty());

    return EncodeRedisCmd("ZSCORE %s %b", strKey.c_str(), strMember.c_str(), strMember.length());
}

int COperateRedis::SetReq_Hash_Add(const string &strKey, const string &strField, const string &strValue)
{
    m_eOptType = OPT_HASH_ADD;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strField.empty());
    CHECK_PARAM_RETWHENERR(strValue.empty());

    return EncodeRedisCmd("HSET %s %s %b", strKey.c_str(), strField.c_str(), strValue.c_str(), strValue.length());
}

int COperateRedis::SetReq_Hash_AddBatch(const string &strKey, map<string, string> &mapFieldValue)
{
    m_eOptType = OPT_HASH_ADDBATCH;

    CHECK_PARAM_RETWHENERR(mapFieldValue.empty());

    return EncodeRedisCmdArray("HMSET", strKey, mapFieldValue);
}

int COperateRedis::SetReq_Hash_GetValue_ByField(const string &strKey, const string &strField)
{
    m_eOptType = OPT_HASH_GET_VALUE_BYFIELD;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strField.empty());

    return EncodeRedisCmd("HGET %s %s", strKey.c_str(), strField.c_str());
}

int COperateRedis::SetReq_Hash_IncrValue_ByField(const string &strKey, const string &strField,long long llNum)
{
    m_eOptType = OPT_HASH_INCRVALUE_BYFIELD;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strField.empty());

    return EncodeRedisCmd("HINCRBY %s %s %lld", strKey.c_str(), strField.c_str(), llNum);
}

int COperateRedis::SetReq_Hash_GetAllField(const string &strKey)
{
    m_eOptType = OPT_HASH_GETALL_FIELD;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("HKEYS %s", strKey.c_str());
}

int COperateRedis::SetReq_Hash_GetAllValue(const string &strKey)
{
    m_eOptType = OPT_HASH_GETALL_VALUE;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("HVALS %s", strKey.c_str());
}

int COperateRedis::SetReq_Hash_GetBatchValue_ByField(const string &strKey, const vector<string> &vecField)
{
    m_eOptType = OPT_HASH_GETBATCH_VALUE_BYFIELD;

    CHECK_PARAM_RETWHENERR(vecField.empty());

    return EncodeRedisCmdArray("HMGET", strKey, vecField);
}

int COperateRedis::SetReq_Hash_GetAllFieldValue(const string &strKey)
{
    m_eOptType = OPT_HASH_GETALL_FIELDVALUE;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("HGETALL %s", strKey.c_str());
}

int COperateRedis::SetReq_Hash_Count(const string &strKey)
{
    m_eOptType = OPT_HASH_COUNT;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("HLEN %s", strKey.c_str());
}

int COperateRedis::SetReq_Hash_DelBatch(const string &strKey, const vector<string> &vecField)
{
    m_eOptType = OPT_HASH_DELBATCH;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(vecField.empty());

    return EncodeRedisCmdArray("HDEL", strKey, vecField);
}

int COperateRedis::SetReq_Hash_IsExist_Field(const string &strKey, const string &strField)
{
    m_eOptType = OPT_HASH_ISEXIST_FIELD;

    CHECK_PARAM_RETWHENERR(strKey.empty());
    CHECK_PARAM_RETWHENERR(strField.empty());

    return EncodeRedisCmd("HEXISTS %s %s", strKey.c_str(), strField.c_str());
}

int COperateRedis::SetReq_Key_IsExist(const string &strKey)
{
    m_eOptType = OPT_KEY_ISEXIST;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("EXISTS %s", strKey.c_str());
}

int COperateRedis::SetReq_Key_SetExp(const string &strKey, long long llExpTime_s)
{
    m_eOptType = OPT_KEY_SETEXP;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("EXPIRE %s %lld", strKey.c_str(), llExpTime_s);
}

int COperateRedis::SetReq_Key_SetExp_MS(const string &strKey, long long llExpTime_ms)
{
    m_eOptType = OPT_KEY_SETEXP_MS;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("PEXPIRE %s %lld", strKey.c_str(), llExpTime_ms);
}

int COperateRedis::SetReq_Key_Del(const string &strKey)
{
    m_eOptType = OPT_KEY_DEL;

    CHECK_PARAM_RETWHENERR(strKey.empty());

    return EncodeRedisCmd("DEL %s", strKey.c_str());
}

int COperateRedis::SetReq_Key_DelBatch(const vector<string> &vecKey)
{
    m_eOptType = OPT_KEY_DELBATCH;

    CHECK_PARAM_RETWHENERR(vecKey.empty());

    return EncodeRedisCmdArray("DEL", vecKey);
}

int COperateRedis::Process()
{
    int iRet = 0;

    ReportMoniAll();

    iRet = SetSendBufByExtSendData(m_strReqData);
    if (0 != iRet)
    {
        SPP_ERROR_LOG("optRedisTask tName[%s]tKey[%s]: SetSendBuf_error, ret[%d] "
            ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(), iRet);

        ReportMoniEncodeErr();
        ReportMcall(iRet);
        return iRet;
    }
    SPP_TRACE_LOG("optRedisTask tName[%s]tKey[%s]: sendBufSize[%u]"
        ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(), GetSendLen());

    iRet = CTaskBase::Process();
    if (0 != iRet)
    {
        SPP_ERROR_LOG("optRedisTask tName[%s]tKey[%s]: baseTaskProcess_error, ret[%d]"
            ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(), iRet);

        ReportMoniFail();
        ReportMcall(iRet);
        return iRet;
    }

    //解析redis数据
    iRet = DecodeRedisResult(GetRecvBuf(),GetRecvLen());
    if(0 != iRet)
    {
        //错误日志在DecodeRedisResult函数中已打印
        ReportMoniDecodeErr();
        ReportMcall(iRet);
        return iRet;
    }

    SPP_TRACE_LOG("optRedisTask tName[%s]tKey[%s]: baseTaskProcess, optSucc"
        ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str());

    ReportMoniSucc();
    ReportMcall(iRet);
    return iRet;
}

inline bool COperateRedis::CheckStrParamIsInt(const string &strData)
{
    if(strData.empty())//字符串为空返回false
        return false;

    size_t uiIdx = 0;
    if('-' == strData[0] || '+' == strData[0])
        uiIdx++;

    for(; uiIdx < strData.size(); ++uiIdx)
    {
        if(!(strData[uiIdx] >= '0' && strData[uiIdx] <= '9'))
            return false;
    }
    return true;
}

//只判断XX.XX这种形式的浮点型，科学计数类型的不处理（3.4E+38）
inline bool COperateRedis::CheckStrParamIsDouble(const string &strData)
{
    if(strData.empty())//字符串为空返回false
        return false;

    size_t uiIdx = 0;
    if('-' == strData[0] || '+' == strData[0])
        uiIdx++;

    size_t uiCount = 0;
    for(; uiIdx < strData.size(); ++uiIdx)
    {
        if('.' == strData[uiIdx])
        {
            uiCount++;
            if(0 == uiIdx || strData.size() - 1 == uiIdx)//小数点不能是第一个或者最后一个
                return false;

            break;
        }
        if(!(strData[uiIdx] >= '0' && strData[uiIdx] <= '9'))
            return false;
    }

    if(uiCount > 1) //小数点不能大于1
        return false;

    return true;
}

inline bool COperateRedis::CheckStrParamIsMaxMin(const string &strData)
{
    if("-inf" == strData)
        return true;
    else if("+inf" == strData)
        return true;

    return false;
}

inline bool COperateRedis::CheckValidScore(const string &strData)
{
    if(CheckStrParamIsMaxMin(strData))
        return true;
    else if(CheckStrParamIsDouble(strData))
        return true;

    return false;
}

// 请求打包函数
int COperateRedis::EncodeRedisCmd(const char *format, ...)
{
    char *pCmdBuf = NULL;
    va_list ap;
    va_start(ap,format);
    int iBufLen = redisvFormatCommand(&pCmdBuf,format,ap);
    va_end(ap);
    if (iBufLen < 0)
        return ERR_ENCODE_FAILED;

    m_strReqData.assign(pCmdBuf,iBufLen);
    free(pCmdBuf);
    pCmdBuf = NULL;

    return 0;
}

int COperateRedis::EncodeRedisCmdArray(const string &strCmd,const map<string, string> &mapKV)
{
    size_t uiSize = mapKV.size() * 2 + 1;
    vector<const char*> vecValue(uiSize);
    vector<size_t>      vecLen(uiSize);
    int index = 0;

    vecValue[0] = strCmd.c_str();
    vecLen[0]   = strCmd.size();
    index++;

    for (map<string, string>::const_iterator it = mapKV.begin(); it != mapKV.end(); it++)
    {
        vecValue[index] = it->first.c_str();
        vecLen[index]   = it->first.size();
        index++;
        vecValue[index] = it->second.c_str();
        vecLen[index]   = it->second.size();
        index++;
    }

    char *pCmdBuf = NULL;
    int iBufLen = redisFormatCommandArgv(&pCmdBuf, index, &(vecValue[0]), &(vecLen[0]));
    if (iBufLen < 0)
        return ERR_ENCODE_FAILED;

    m_strReqData.assign(pCmdBuf,iBufLen);
    free(pCmdBuf);
    pCmdBuf = NULL;

    return 0;
}

int COperateRedis::EncodeRedisCmdArray(const string &strCmd,const vector<string> &vecV)
{
    size_t uiSize = vecV.size() + 1;
    vector<const char*> vecValue(uiSize);
    vector<size_t>      vecLen(uiSize);
    int index = 0;

    vecValue[0] = strCmd.c_str();
    vecLen[0]   = strCmd.size();
    index++;

    for (size_t uiIdx = 0; uiIdx < vecV.size(); ++uiIdx)
    {
        vecValue[index] = vecV[uiIdx].c_str();
        vecLen[index]   = vecV[uiIdx].size();
        index++;
    }

    char *pCmdBuf = NULL;
    int iBufLen = redisFormatCommandArgv(&pCmdBuf, index, &(vecValue[0]), &(vecLen[0]));
    if (iBufLen < 0)
        return ERR_ENCODE_FAILED;

    m_strReqData.assign(pCmdBuf,iBufLen);
    free(pCmdBuf);
    pCmdBuf = NULL;

    return 0;
}

int COperateRedis::EncodeRedisCmdArray(const string &strCmd,const string &strReidsKey,const map<string, string> &mapKV)
{
    size_t uiSize = mapKV.size() * 2 + 2;
    vector<const char*> vecValue(uiSize);
    vector<size_t>      vecLen(uiSize);
    int index = 0;

    vecValue[0] = strCmd.c_str();
    vecLen[0]   = strCmd.size();
    index++;

    vecValue[1] = strReidsKey.c_str();
    vecLen[1]   = strReidsKey.size();
    index++;

    for (map<string, string>::const_iterator it = mapKV.begin(); it != mapKV.end(); it++)
    {
        vecValue[index] = it->first.c_str();
        vecLen[index]   = it->first.size();
        index++;
        vecValue[index] = it->second.c_str();
        vecLen[index]   = it->second.size();
        index++;
    }

    char *pCmdBuf = NULL;
    int iBufLen = redisFormatCommandArgv(&pCmdBuf, index, &(vecValue[0]), &(vecLen[0]));
    if (iBufLen < 0)
        return ERR_ENCODE_FAILED;

    m_strReqData.assign(pCmdBuf,iBufLen);
    free(pCmdBuf);
    pCmdBuf = NULL;

    return 0;
}

int COperateRedis::EncodeRedisCmdArray(const string &strCmd,const string &strReidsKey,const vector<string> &vecV)
{
    size_t uiSize = vecV.size() + 2;
    vector<const char*> vecValue(uiSize);
    vector<size_t>      vecLen(uiSize);
    int index = 0;

    vecValue[0] = strCmd.c_str();
    vecLen[0]   = strCmd.size();
    index++;

    vecValue[1] = strReidsKey.c_str();
    vecLen[1]   = strReidsKey.size();
    index++;

    for (size_t uiIdx = 0; uiIdx < vecV.size(); ++uiIdx)
    {
        vecValue[index] = vecV[uiIdx].c_str();
        vecLen[index]   = vecV[uiIdx].size();
        index++;
    }

    char *pCmdBuf = NULL;
    int iBufLen = redisFormatCommandArgv(&pCmdBuf, index, &(vecValue[0]), &(vecLen[0]));
    if (iBufLen < 0)
        return ERR_ENCODE_FAILED;

    m_strReqData.assign(pCmdBuf,iBufLen);
    free(pCmdBuf);
    pCmdBuf = NULL;

    return 0;
}

int COperateRedis::DecodeRedisResult(const char *buf, int len)
{
    int          iRet   = 0;
    void        *reply  = NULL;
    redisReader *reader = NULL;

    //读取buf数据
    reader = redisReaderCreate();
    if (NULL == reader)
    {
        return RetErrAndFreeRedis(ERR_DECODE_FAILED,"redisReaderCreateError",reply,reader);
    }
    iRet = redisReaderFeed(reader, buf, len);
    if (REDIS_OK != iRet)
    {
        return RetErrAndFreeRedis(ERR_DECODE_FAILED,"redisReaderFeedError",reply,reader);
    }

    //获取reply数据
    iRet = redisReaderGetReply(reader, &reply);
    if (REDIS_OK != iRet)
    {
        return RetErrAndFreeRedis(ERR_DECODE_FAILED,"redisGetReplyError",reply,reader);
    }

    //解析reply数据，写入到m_stRedisRspData中
    redisReply *pRedisReply = (redisReply*)reply;
    if(NULL == pRedisReply)// 连接出错
    {
        return RetErrAndFreeRedis(ERR_DECODE_FAILED,"redisReplyIsNULL",reply,reader);
    }
    if(pRedisReply->type == REDIS_REPLY_ERROR) // 数据格式错误
    {
        return RetErrAndFreeRedis(ERR_DATA_ERROR,"redisDataError",reply,reader);
    }

    //读取相应的数据
    if(REDIS_REPLY_NIL == pRedisReply->type) //数据为空为正常的状态，有些key确实就没有对应的数据
    {
        m_stRedisRspData.eRedisDataType = REDIS_DTYPE_EMPTY;
    }
    else if(REDIS_REPLY_INTEGER == pRedisReply->type)
    {
        m_stRedisRspData.eRedisDataType = REDIS_DTYPE_INT;
        m_stRedisRspData.llResult = pRedisReply->integer;
    }
    else if(REDIS_REPLY_STRING == pRedisReply->type)
    {
        m_stRedisRspData.eRedisDataType = REDIS_DTYPE_STR;
        m_stRedisRspData.strResult.assign(pRedisReply->str,pRedisReply->len);
    }
    else if(REDIS_REPLY_STATUS == pRedisReply->type)
    {
        m_stRedisRspData.eRedisDataType = REDIS_DTYPE_STR;
        m_stRedisRspData.strResult.assign(pRedisReply->str,pRedisReply->len);
    }
    else if(REDIS_REPLY_ARRAY == pRedisReply->type)
    {
        //对于特定的命令，解析数据到对应的vector结构中

        if(OPT_SSET_GETLIST_VALUESCORE_BYRANK == m_eOptType || OPT_SSET_GETLIST_VALUESCORE_BYSCORE == m_eOptType)
        {
            //有序集合中查询带分数数据时，解析存储到vector<pair<string, string> > vecPairValueScore
            m_stRedisRspData.eRedisDataType = REDIS_DTYPE_VEC_PAIR_VALUE_SCORE;
            for(size_t i = 0; i < pRedisReply->elements && i+1 < pRedisReply->elements; i += 2)
            {
                string strValue;
                string strScore;
                if(NULL != pRedisReply->element[i]->str)
                    strValue.assign(pRedisReply->element[i]->str, pRedisReply->element[i]->len);
                if(NULL != pRedisReply->element[i+1]->str)
                    strScore.assign(pRedisReply->element[i+1]->str, pRedisReply->element[i+1]->len);

                m_stRedisRspData.vecPairValueScore.push_back(make_pair(strValue,strScore));
            }
        }
        else if(OPT_HASH_GETALL_FIELDVALUE == m_eOptType)
        {
            //hash中查询field和value时，解析存储到map<string,string> mapFieldValue
            m_stRedisRspData.eRedisDataType = REDIS_DTYPE_MAP_FIELD_VALUE;
            for(size_t i = 0; i < pRedisReply->elements && i+1 < pRedisReply->elements; i += 2)
            {
                string strField;
                string strValue;
                if(NULL != pRedisReply->element[i]->str)
                    strField.assign(pRedisReply->element[i]->str, pRedisReply->element[i]->len);
                if(NULL != pRedisReply->element[i+1]->str)
                    strValue.assign(pRedisReply->element[i+1]->str, pRedisReply->element[i+1]->len);

                m_stRedisRspData.mapFieldValue[strField] = strValue;
            }
        }
        else
        {
            //其他情况存储到vector<string>中
            m_stRedisRspData.eRedisDataType = REDIS_DTYPE_VEC_STR;
            for(size_t i = 0; i < pRedisReply->elements; i++)
            {
                string sResult = "";
                if(pRedisReply->element[i]->str)
                    sResult.append(pRedisReply->element[i]->str, pRedisReply->element[i]->len);

                m_stRedisRspData.vecStrResult.push_back(sResult);
            }
        }
    }

    //打印下redis的reply结构数据、redis回包存储数据StRedisRspData
    SPP_TRACE_LOG("optRedisTask tName[%s]tKey[%s]: redisReply, type[%d] integer[%lld] strPtr[%p]strLen[%d] elementsNum[%u]"
        ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str()
        ,pRedisReply->type,pRedisReply->integer,pRedisReply->str,pRedisReply->len,pRedisReply->elements);

    SPP_TRACE_LOG("optRedisTask tName[%s]tKey[%s]: StRspRedisData: type[%d]llResult[%lld]strResultSize[%u]"
        "vecStrSize[%u]vecPairValueScoreSize[%u]mapFieldValueSize[%u]"
        ,GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str()
        , m_stRedisRspData.eRedisDataType, m_stRedisRspData.llResult, m_stRedisRspData.strResult.size()
        , m_stRedisRspData.vecStrResult.size(), m_stRedisRspData.vecPairValueScore.size(), m_stRedisRspData.mapFieldValue.size());

    return RetErrAndFreeRedis(0,"DecodeSucc",reply,reader);
}

inline int COperateRedis::RetErrAndFreeRedis(int iError, const string &strMsg, void *reply, redisReader *reader)
{
    if(NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    if(NULL != reader)
    {
        redisReaderFree(reader);
        reader = NULL;
    }

    //日志打印
    if( 0 != iError)
        SPP_ERROR_LOG("optRedisTask tName[%s]tKey[%s]: Error, %s",GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(),strMsg.c_str());
    else
        SPP_TRACE_LOG("optRedisTask tName[%s]tKey[%s]: Succ, %s",GetTaskName(GetTaskType()).c_str(), GetTaskKey().c_str(),strMsg.c_str());

    return iError;
}
