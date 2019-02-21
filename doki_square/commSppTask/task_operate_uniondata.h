/*
 *  Filename: task_operate_uniondata.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 通用的获取union数据的task
 *
 */

#ifndef __TASK_OPERATE_UNIONDATA_H__
#define __TASK_OPERATE_UNIONDATA_H__

#include "task_base.h"
#include "unionapi.h"  //union Api

#include <vector>
#include <map>
#include <set>
#include <sstream>
using namespace std;

struct StUnionReqData //union请求数据封装
{
  public:
    int iUnionId;               //要查询的union字典id
    vector<string> vecVideoIds; //要查询的视频id列表
    vector<string> vecFields;   //要查询的union字段列表

    void AppendBatchField(const string &strFields) //请求数据设置字段值(字段使用逗号分隔)
    {
        SplitToVecAppend(strFields, ",", vecFields);
        return;
    }

    void AppendBatchField(vector<string> &vecReqField)
    {
        vecFields.insert(vecFields.end(), vecReqField.begin(), vecReqField.end());
        return;
    }

    void AppendBatchField(set<string> &setReqField)
    {
        vecFields.insert(vecFields.end(), setReqField.begin(), setReqField.end());
        return;
    }

    void AppendBatchKeyId(vector<string> &vecKeyId) //请求数据添加视频id
    {
        vecVideoIds.insert(vecVideoIds.end(), vecKeyId.begin(), vecKeyId.end());
        return;
    }

    void AppendBatchKeyId(set<string> &setKeyId) //请求数据添加视频id
    {
        vecVideoIds.insert(vecVideoIds.end(), setKeyId.begin(), setKeyId.end());
        return;
    }

  private:
    void SplitToVecAppend(const string &strSource, const string &strDelimiter, vector<string> &vec, bool bCleanEmpty = true)
    {
        if (strSource.empty())
            return;

        string str = strSource;
        str.append(strDelimiter);
        string substr;
        size_t pos = 0;
        size_t n;

        while ((n = str.find(strDelimiter, pos)) != string::npos)
        {
            substr = str.substr(pos, n - pos);
            if (!bCleanEmpty || !substr.empty())
                vec.push_back(str.substr(pos, n - pos));
            pos = n + strDelimiter.size();
        }

        return;
    }
};

class COperateUnionData : public CTaskBase
{
  public:
    //eRecvBufLen接受数据默认初始buf，不够时spp框架会自动扩展的；为了避免多次重新分配，默认初始buf应该和平均真实包大小相当
    COperateUnionData(ENUM_TASK_TYPE eTaskType, ENUM_TASK_BUFLEN eRecvBufLen = BUF_LEN_8K,const string &strAppid = TASK_UNION_APPID, const string &strAppkey = TASK_UNION_APPKEY);
    ~COperateUnionData();

    // ====设置请求的平台和版本信息，union中有些字段如角标，其有基于平台版本的逻辑在内,需要传递平台和版本信息
    void SetReqAppInfo(int iPlatform, const string &strAppver)
    {
        m_iReqPlatform = iPlatform;
        m_strReqAppVer = strAppver;
    }

    // ====设置union请求结构==================================================
    // 使用StUnionReqData设置请求结构
    void SetReqStUnion(StUnionReqData &stUnionReq) //请求数据整体赋值，这里会有一次结构体赋值操作
    {
        m_stUnionReq = stUnionReq;
    }

    // ====设置请求字典表========
    void SetReqUnionId(int iUnionId) //请求数据设置union字典id
    {
        m_stUnionReq.iUnionId = iUnionId;
    }

    // ====添加请求字段的函数========
    void SetReqFields(const string &strFields, bool bClear = true)//请求数据设置字段值(字段使用逗号分隔)，bClear是否清除之前的设置
    {
        if(bClear)
            m_stUnionReq.vecFields.clear();

        m_stUnionReq.AppendBatchField(strFields);
    }
    void SetReqFields(vector<string> &vecFields, bool bClear = true)//请求数据设置字段值，bClear是否清除之前的设置
    {
        if(bClear)
            m_stUnionReq.vecFields.clear();

        m_stUnionReq.AppendBatchField(vecFields);
    }
    void SetReqFields(set<string> &setFields, bool bClear = true)//请求数据设置字段值，bClear是否清除之前的设置
    {
        if(bClear)
            m_stUnionReq.vecFields.clear();

        m_stUnionReq.AppendBatchField(setFields);
    }
    void SetReqAddFieldId(const string &strField)//请求数据添加字段值
    {
        m_stUnionReq.vecFields.push_back(strField);
    }

    // ====设置请求视频id的函数
    void SetReqVideoIds(vector<string> &vecKeyId, bool bClear = true) //请求数据添加视频id，bClear是否清除之前的设置
    {
        if(bClear)
            m_stUnionReq.vecVideoIds.clear();

        m_stUnionReq.AppendBatchKeyId(vecKeyId);
    }
    void SetReqVideoIds(set<string> &setKeyId, bool bClear = true) //请求数据添加视频id，bClear是否清除之前的设置
    {
        if(bClear)
            m_stUnionReq.vecVideoIds.clear();

        m_stUnionReq.AppendBatchKeyId(setKeyId);
    }
    void SetReqAddVideoId(const string &strVideoId) //请求数据添加视频id
    {
        m_stUnionReq.vecVideoIds.push_back(strVideoId);
    }

    // ====获取union回包结构==================================================
    int GetUnionId() //获取union字典id
    {
        return m_stUnionReq.iUnionId;
    }
    tv::DSGetRsp &GetUnionRspData() //获取union回包数据
    {
        return m_stUnionRsp;
    }

    int GetMapRspData(map< string, map<string, string> > &mapStrUnionRsp); //获取字符类型数据:外部map的key为视频id,内部的map中key为字段名，value为字段值

    // ====spp微线程框架的执行函数============================================
    virtual int Process();

  public:
    static string  UnionValueToString(const tv::Value &v);
    static int64_t UnionValueToInt64(const tv::Value &v);
    static int UnionValueToVec(const tv::Value &v, vector<string> &vecStrData);
    static int UnionValueToSet(const tv::Value &v, set<string> &setStrData);

  private:
    template <typename T>
    static string UnionNum2Str(T n)
    {
        stringstream os;
        os << n;
        return os.str();
    }

  private:
    string         m_strAppid;
    string         m_strAppkey;
    string         m_strReqAppVer;    //请求的app版本，主要用于取角标
    int            m_iReqPlatform;    //请求的平台号，主要用于取角标

    StUnionReqData m_stUnionReq;      //union的请求结构
    tv::DSGetRsp   m_stUnionRsp;      //union的回包结构
};

#endif /*__TASK_OPERATE_UNIONDATA_H__*/
