/*
 *  Filename: task_operate_uniondata.cpp
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 通用的获取union数据的task
 *
 */

#include "task_operate_uniondata.h"

/*
    @biref:对包的合法性进行检测
    @return:
    <0, 出错，包不合法
    =0, 包体没有接受完整,返回0让spp_proxy继续接受数据
    >0, 返回包体的长度
    @process:
    1、先检测包是否接受完整
    2、然后检查包是否合法
*/
static int CheckUnionPktRecv(void *buf, int len, bool closed, void* msg_ctx, bool &msg_len_detected)
{
    return UnionApi::UnionCoder::CheckRspPktComplete((const char *)buf, len);
}

COperateUnionData::COperateUnionData(ENUM_TASK_TYPE eTaskType, ENUM_TASK_BUFLEN eRecvBufLen,const string &strAppid, const string &strAppkey)
:m_iReqPlatform(0)
{
    m_strAppid  = strAppid;
    m_strAppkey = strAppkey;

    SetTaskType((int)eTaskType);
    SetRecvInitBufSize(eRecvBufLen);
    SetCheckFunc(CheckUnionPktRecv);

    SetTaskInfo(g_oMgSvrConn.GetSvrConnInfo(GetTaskType()),g_oMgMcall.GetMcallInfo(GetTaskType()),g_oMgMoni.GetMoniInfo(GetTaskType()));
}

COperateUnionData::~COperateUnionData()
{
}

int COperateUnionData::Process()
{
    int iRet = 0;

    ReportMoniAll();

    string strReqData;
    UnionApi::UnionCoder coder;
    coder.SetAppInfo(m_strAppid,m_strAppkey);
    iRet = coder.GetFieldsEncode(strReqData, 0, m_stUnionReq.iUnionId, m_stUnionReq.vecVideoIds, m_stUnionReq.vecFields,false,m_iReqPlatform,m_strReqAppVer);
    if(iRet != 0)
    {
        SPP_ERROR_LOG("optUnionTask tName[%s]tKey[%s]: ReqEncodeError, ret[%d] ",GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iRet);

        ReportMoniEncodeErr();
        ReportMcall(ERR_TASK_ENCODE);
        return ERR_TASK_ENCODE;
    }
    iRet = SetSendBufByExtSendData(strReqData);
    if(0 != iRet)
    {
        SPP_ERROR_LOG("optUnionTask tName[%s]tKey[%s]: SetSendBuf_error, ret[%d] ",GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iRet);

        ReportMoniEncodeErr();
        ReportMcall(iRet);
        return iRet;
    }
    SPP_TRACE_LOG("optUnionTask tName[%s]tKey[%s]: sendBufSize[%u]"
        ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),GetSendLen());


    iRet = CTaskBase::Process();
    if(0 != iRet)
    {
        SPP_ERROR_LOG("optUnionTask tName[%s]tKey[%s]: baseTaskProcess_error, ret[%d] ",GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),iRet);

        ReportMoniFail();
        ReportMcall(iRet);
        return iRet;
    }

    int iSeq = 0;
    iRet = coder.GetFieldsDecode((const char*)GetRecvBuf(), (size_t)GetRecvLen(), m_stUnionRsp, iSeq);
    if (iRet != 0)
    {
        SPP_ERROR_LOG("optUnionTask tName[%s]tKey[%s]: RspDecodeError, unionid[%d] funcRetCode[%d] error[%d:%s]  "
            ,GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str(),GetUnionId(),iRet,m_stUnionRsp.errorno(),m_stUnionRsp.errormsg().c_str());

        ReportMoniDecodeErr();
        ReportMcall(iRet);
        return iRet;
    }

    SPP_TRACE_LOG("optUnionTask tName[%s]tKey[%s]: baseTaskProcess, optSucc ",GetTaskName(GetTaskType()).c_str(),GetTaskKey().c_str());

    ReportMoniSucc();
    ReportMcall(iRet);
    return iRet;
}

int COperateUnionData::GetMapRspData(map<string,map<string,string> > &mapStrUnionRsp) //获取字符类型数据
{
    if(0 != m_stUnionRsp.errorno())
    {
        SPP_ERROR_LOG("UnionGetMapRspData: reterr[%d] unionId[%d]",m_stUnionRsp.errorno(),m_stUnionReq.iUnionId);
        return -1;
    }

    for (int k = 0; k < m_stUnionRsp.results_size(); k++)
    {
        const tv::DataSet &set = m_stUnionRsp.results(k);
        string strKeyId = set.id();

        map<string,string> mapFieldValue;
        for (int i = 0; i < set.fields_size(); i++)
        {
            const tv::KeyValue &kv = set.fields(i);
            mapFieldValue[kv.key()] = COperateUnionData::UnionValueToString(kv.value());
        }

        mapStrUnionRsp[strKeyId] = mapFieldValue;
    }

    return 0;
}

string COperateUnionData::UnionValueToString(const tv::Value &v)
{
    std::string strVal;
    switch (v.type())
    {
        case tv::Value::VT_STRING:
            strVal = v.strval();
            break;
        case tv::Value::VT_INT:
            strVal = COperateUnionData::UnionNum2Str(v.intval());
            break;
        case tv::Value::VT_FLOAT:
            strVal = COperateUnionData::UnionNum2Str(v.floatval());
            break;
        case tv::Value::VT_NULL:
            strVal = "";
            break;
        case tv::Value::VT_TRUE:
            strVal = "true";
            break;
        case tv::Value::VT_FALSE:
            strVal = "false";
            break;
        case tv::Value::VT_ARRAY:
            strVal.append("+");
            for (int j = 0; j < v.arrval_size(); j++)
            {
                strVal += COperateUnionData::UnionValueToString(v.arrval(j));
                if (j + 1 != v.arrval_size())
                    strVal.append("+");
            }
            strVal.append("+");
            break;
        case tv::Value::VT_OBJECT:
            strVal.append("{");
            for (int j = 0; j < v.objval_size(); j++)
            {
                const tv::KeyValue &kv = v.objval(j);
                strVal += COperateUnionData::UnionValueToString(kv.value());
                if (j + 1 != v.objval_size())
                    strVal.append(",");
            }
            strVal.append("}");
            break;
        default:
            break;
    }

    return strVal;
}

int64_t COperateUnionData::UnionValueToInt64(const tv::Value &v)
{
    int64_t llResult = 0;
    switch (v.type())
    {
        case tv::Value::VT_STRING:
            llResult = atoll(v.strval().c_str());
            break;
        case tv::Value::VT_INT:
            llResult = v.intval();
            break;
        case tv::Value::VT_FLOAT:
            llResult = v.floatval();
            break;
        case tv::Value::VT_TRUE:
            llResult = 1;
            break;
        case tv::Value::VT_FALSE:
            llResult = 0;
            break;
        default:
            break;
    }
    return llResult;
}

int COperateUnionData::UnionValueToVec(const tv::Value &v, vector<string> &vecStrData)
{
    if (tv::Value::VT_ARRAY != v.type())
        return -1;

    for (int i = 0; i < v.arrval_size(); i++)
    {
        vecStrData.push_back(COperateUnionData::UnionValueToString(v.arrval(i)));
    }

    return 0;
}

int COperateUnionData::UnionValueToSet(const tv::Value &v, set<string> &setStrData)
{
    if (tv::Value::VT_ARRAY != v.type())
        return -1;

    for (int i = 0; i < v.arrval_size(); i++)
    {
        setStrData.insert(COperateUnionData::UnionValueToString(v.arrval(i)));
    }

    return 0;
}
