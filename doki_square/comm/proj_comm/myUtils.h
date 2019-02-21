/*
 *  Filename: myUtils.h
 *
 *  Created on: 2015-01-16
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2015 Tencent. All rights reserved.
 *
 *  Description: 工具函数
 *
 *  修改记录：
 *  1. 版本5: 2016-04-27 整理主要的接口，细分为不同的类。
 *  2. 版本6: 2016-05-12 整理接口，补充接口
 *  3. 版本7: 2016-05-14 添加接口
 *  4. 版本8: 2016-07-18 添加字符串子串替换函数，添加HTML编码函数，添加去字符串首尾空格的函数
 *  5. 版本9: 2016-08-22 将需要包含第三方头文件的工具函数迁移到myUtilsTVideo.h中，myUtils.h不再
 *            包含需要引入第三方头文件的工具函数。
 *  6. 版本10:
 *      新增A=x&B=y形式的函数这种特殊类型的字符串解析函数     on 2016-10-16
 *      添加时间格式转换函数                                  on 2016-10-25
 *      新增信号操作类                                        on 2016-11-17
 *      新增函数将英文引号转成中文引号                        on 2016-11-25
 *      新增xss漏洞参数校验函数                               on 2016-12-08
 *      新增ip操作类                                          on 2016-12-28
 *      新增解析get方式url请求中的参数                        on 2017-01-06
 *      新增html编码解码函数                                  on 2017-01-11
 *      html函数整理成类，增加参数url编码，                   on 2017-02-28
 *      增加版本比较函数                                      on 2017-03-01
 *      补充获取到当前时间精度到微秒的函数                    on 2017-03-02
 *      增加hash函数                                          on 2017-05-10
 *      业务相关的函数迁移到myUtilsTVideo中                   on 2017-05-11
 *      增加字符串字母大小写转换函数                          on 2017-05-18
 *      修改Ip地址转换函数，支持网络和主机字节数              on 2017-06-05
 *      增加将map<string,string>转换成成A=x&B=y形式的函数     on 2017-06-27
 *      增加整数分片函数                                      on 2017-06-27
 *      新增新的url编解码函数                                 on 2017-07-07
 *      新增BinStr2HexStr、HexStr2BinStr的函数                on 2017-07-12
 *      添加取消字符串中特定子串的函数                        on 2017-07-16
 *      添加防雪崩类                                          on 2017-07-16
 *      添加从字符串中解析L5的函数                            on 2017-07-16
 *      从cookie的字符串列表中，获取map存储的kv数据           on 2017-08-07
 *      添加hash函数                                          on 2017-08-13
 *
 */

#ifndef _MY_UTILS_H_
#define _MY_UTILS_H_
#include <sys/time.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <cassert>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iconv.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include "json.h"

using namespace std;
namespace MyUtils
{

//解析json的宏定义封装,json对象默认使用jsonValue===============
#define DEF_GET_JSON_TYPE(obj,fieldName,Type) do \
{\
    if(jsonValue.isMember(fieldName) && jsonValue[fieldName].is##Type()) \
    {\
        obj = jsonValue[fieldName].as##Type();\
    }\
} while(0) \

//老版本的json转jce的宏定义，字段不用引号括起来，为了减少老版本的修改量，也放到这里来
#define DEF_GET_JSON_TYPE_OLD(obj,fieldName,Type) do \
{\
    if(jsonValue.isMember(#fieldName) && jsonValue[#fieldName].is##Type()) \
    {\
        obj = jsonValue[#fieldName].as##Type();\
    }\
} while(0) \

#define DEF_GET_JSON_STR(obj,fieldName) do \
{\
    stringstream ssTemp; \
    if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isString()) \
    {\
        obj = jsonValue[fieldName].asString();\
    }\
    else if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isInt()) \
    {\
        ssTemp << jsonValue[fieldName].asInt();\
        ssTemp >> obj; \
    }\
    else if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isDouble()) \
    {\
        ssTemp << jsonValue[fieldName].asDouble();\
        ssTemp >> obj; \
    }\
} while(0) \

#define DEF_GET_JSON_INT(obj,fieldName) do \
{\
    stringstream ssTemp; \
    if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isString()) \
    {\
        ssTemp << jsonValue[fieldName].asString();\
        ssTemp >> obj; \
    }\
    else if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isDouble()) \
    {\
        ssTemp << jsonValue[fieldName].asDouble();\
        ssTemp >> obj; \
    }\
    else if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isInt()) \
    {\
        obj = jsonValue[fieldName].asInt();\
    }\
} while(0) \

#define DEF_GET_JSON_DOUBLE(obj,fieldName) do \
{\
    stringstream ssTemp; \
    if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isString()) \
    {\
        ssTemp << jsonValue[fieldName].asString();\
        ssTemp >> obj; \
    }\
    else if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isInt()) \
    {\
        ssTemp << jsonValue[fieldName].asInt();\
        ssTemp >> obj; \
    }\
    else if(jsonValue.isMember(fieldName) && jsonValue[fieldName].isDouble()) \
    {\
        obj = jsonValue[fieldName].asDouble();\
    }\
} while(0) \

//=============================================================================

#ifndef foreach
#define foreach(container,it) \
    for(typeof((container).begin()) it = (container).begin();it!=(container).end();++it)
#endif

//参照phorix的优化
static char c_b2s[256][4] = {"00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f"
                            ,"10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f"
                            ,"20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f"
                            ,"30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f"
                            ,"40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f"
                            ,"50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f"
                            ,"60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f"
                            ,"70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f"
                            ,"80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f"
                            ,"90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f"
                            ,"a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af"
                            ,"b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf"
                            ,"c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf"
                            ,"d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df"
                            ,"e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef"
                            ,"f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff"};

// ================ class CNum2Str===================================================
class CNum2Str
{
  public:
    template<typename T>
    static string Num2Str(T n)
    {
        stringstream os;
        os << n;
        return os.str();
    }
};

// ================ class CStrUnits===================================================
class CStrUnits //字符串相关操作
{
  public:
    CStrUnits(){};
    ~CStrUnits(){};

    //时间戳转换成时间格式的字符串 "%Y%m%d_%H%M%S"
    static string FormatTime(time_t llTime,const string &strFormatStr)
    {
        time_t t;
        t=llTime;
        struct tm *p;
        p=localtime(&t);
        char s[40];
        strftime(s, sizeof(s), strFormatStr.c_str(), p);
        return string(s);
    }

    //时间戳转换成时间格式的字符串 "%Y%m%d_%H%M%S" - 格林威治时间
    static string FormatTime_g(time_t llTime, const string &strFormatStr)
    {
        time_t t;
        t = llTime;
        struct tm *p;
        p = gmtime(&t);
        char s[40];
        strftime(s, sizeof(s), strFormatStr.c_str(), p);
        return string(s);
    }

    //将一种时间格式转换成另一种时间格式 %Y-%m-%d %H:%M:%S
    //能转换成功需要strSrcTime带有完整的年月日时分秒数据
    static string FormatTimeFStr(const string &strSrcTime,const string &strSrcFormat,const string &strDstFormat)
    {
        string strDstTime = strSrcTime;

        tm tmData;
        char *pChar = strptime(strSrcTime.c_str(), strSrcFormat.c_str(), &tmData);
        if(NULL != pChar)
        {
            time_t llStartTimeStamp = mktime(&tmData);
            strDstTime  = CStrUnits::FormatTime(llStartTimeStamp,strDstFormat.c_str());
        }

        return strDstTime;
    }

    //能转换成功需要strSrcTime带有完整的年月日时分秒数据 %Y%m%d%H%M%S
    static int64_t GetTimeStampFromStrTime(const string &strSrcTime,const string &strSrcFormat)
    {
        tm tmData;
        char *pChar = strptime(strSrcTime.c_str(), strSrcFormat.c_str(), &tmData);
        if(NULL != pChar)
            return mktime(&tmData);

        return 0;
    }

    //字符串转换成16进制，
    //调用太复杂了，使用BinStr2HexStr函数代替
    static int MemToHex(void *pMem, size_t uDumpOffset, size_t uDumpLen, char *pOut, size_t* piOutLen)
    {
        if (!pMem || !pOut || !piOutLen || *piOutLen < uDumpLen*2+1)
            return -1;

        int inc;
        size_t i;
        char *pSrc = (char*)pMem + uDumpOffset;

        char *pDst = pOut;
        pOut[0] = '\0';

        for (i = uDumpOffset; i < uDumpOffset + uDumpLen; i++, pSrc++) {
            inc = snprintf(pDst, pOut + *piOutLen - pDst, "%02x", (unsigned char) *pSrc);
            if (inc < 0) { return -2; }
            pDst += inc;
        }
        *pDst = '\0';
        *piOutLen = pDst - pOut + 1;
        return 0;
    }

    //16进制转换成字符串
    //调用太复杂了，使用HexStr2BinStr函数代替
    static int HexToMem(char *pMem, size_t uDumpOffset, size_t uDumpLen, char aMemOut[], unsigned short *pwMemLen)
    {
        size_t i;
        char sTmp[10] = {0};
        char *pSrc = pMem + uDumpOffset;

        int j = 0;
        sTmp[2] = '\0';
        size_t uiTmpLength = uDumpLen;

        if (pwMemLen == NULL)
        {
            return -1;
        }

        if (uDumpLen%2 != 0)
        {
            uDumpLen = uDumpLen-1;
            //return -2;
        }


        for (i = uDumpOffset; i < uDumpOffset + uDumpLen; )
        {
            sTmp[0] = *pSrc;
            pSrc++;
            sTmp[1] = *pSrc;
            pSrc++;
            i += 2;
            aMemOut[j++] = (char)strtoul(sTmp, NULL, 16);
            //sscanf(sTmp,"%x",&aMemOut[j++]);

        }
        if(uiTmpLength > uDumpLen)
        {
            sTmp[1] = '\0';
            sTmp[0] = *pSrc;
            int iTmp = 0;
            iTmp = (char)strtoul(sTmp, NULL, 16);
            aMemOut[j++] = iTmp<<4;
        }

        *pwMemLen = j;
        return 0;
    }

    /**
     * @brief  二进制数据转换成16进制字符串.
     *
     * @param sBinData 二进制数据
     * @param sSep     分隔符
     * @param lines    多少个字节换一行, 默认0表示不换行
     * @return         转换后的字符串
     */
    static string BinStr2HexStr(const string &sBinData, const string &sSep = "", size_t lines = 0)
    {
        if(sBinData.empty())
        {
            return "";
        }

        string sOut;
        const unsigned char *p = (const unsigned char *) sBinData.data();

        for (size_t i = 0; i < sBinData.length(); ++i, ++p)
        {
            sOut += c_b2s[*p][0];
            sOut += c_b2s[*p][1];
            sOut += sSep;

            //换行
            if ((lines != 0) && ((i+1) % lines == 0))
            {
                sOut += "\n";
            }
        }

        return sOut;
    }

    /**
     * @brief  16进制字符串转换成二进制数据.
     *
     * @param sString   要转换的字符串
     * @param sSep      分隔符
     * @param lines     多少个字节换一行, 默认0表示不换行
     * @return          转换后的二进制数据
     */
    static string HexStr2BinStr(const string &sString, const string &sSep = "", size_t lines = 0)
    {
        const char *psAsciiData = sString.c_str();

        int iAsciiLength = sString.length();
        string sBinData;
        for (int i = 0 ; i < iAsciiLength ; i++)
        {
            sBinData += CStrUnits::HexStrToC(psAsciiData + i);
            i++;
            i += sSep.length(); //过滤掉分隔符

            if (lines != 0 && sBinData.length()%lines == 0)
            {
                i++;    //过滤掉回车
            }
        }

        return sBinData;
    }

    //gbk转utf8
    static int CodeConvert_Gbk_To_Utf8(const string& in, string& out)
    {
        if(in.empty())
        {
            out.resize(0);
            return 0;
        }
        unsigned int uInLen = in.length();
        unsigned int uOutLen = uInLen*6;
        char * pOut = new char[uOutLen];
        memset(pOut,0,uOutLen);
        int  iRet = CStrUnits::CodeConvert("gbk", "utf-8", (const char*)in.c_str(), uInLen, pOut, (size_t*)&uOutLen);
        int ret =0 ;
        if (iRet <0)
        {
            out.resize(0);
            ret = iRet;
        }
        else
        {
            out = pOut;
            ret = 0;
        }
        delete []pOut;
        return ret;
    }

    //字符串特殊字符拆分
    static void SplitToVec(const string &strSource, const string &strDelimiter, vector<string> &vec, bool bCleanEmpty=true)
    {
        vec.clear();
        if(strSource.empty())
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

    //字符串特殊字符拆分
    static void SplitToVecAppend(const string &strSource, const string &strDelimiter, vector<string> &vec, bool bCleanEmpty=true)
    {
        if(strSource.empty())
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

    //字符串特殊字符拆分
    static void SplitToSet(const string &strSource, const string &strDelimiter, set<string> &setData, bool bCleanEmpty=true)
    {
        setData.clear();
        if(strSource.empty())
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
                setData.insert(str.substr(pos, n - pos));
            pos = n + strDelimiter.size();
        }

        return;
    }

    //字符串特殊字符拆分
    static void SplitToSetAppend(const string &strSource, const string &strDelimiter, set<string> &setData, bool bCleanEmpty=true)
    {
        if(strSource.empty())
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
                setData.insert(str.substr(pos, n - pos));
            pos = n + strDelimiter.size();
        }

        return;
    }

    static void SplitString(vector<string> &v, const string &src, const string &delimit, const string &null_subst)
    {
        v.clear();
        if (src.empty() || delimit.empty())
            return;

        string::size_type deli_len = delimit.size();
        string::size_type index = 0;
        string::size_type last_search_position = 0;
        while ((index = src.find(delimit, last_search_position)) != string::npos)
        {
            if (index == last_search_position)
                v.push_back(null_subst);
            else
                v.push_back(src.substr(last_search_position, index - last_search_position));

            last_search_position = index + deli_len;
        }

        string last_one = src.substr(last_search_position);
        v.push_back(last_one.empty() ? null_subst : last_one);
    }

    static void SplitToJson(const string& strParam, Json::Value& jsParams)
    {
        jsParams.clear();
        if (strParam.empty())
        {
            return;
        }

        vector<string> vecParams;
        SplitString(vecParams, strParam, "&", "");

        for (size_t i = 0; i < vecParams.size(); i++)
        {
            const std::string &kv_string = vecParams[i];
            vector<string> vecKV;
            SplitString(vecKV, kv_string, "=", "");

            if (vecKV.size() != 2) // must have key=value
                continue;

            // set key = value
            std::string key = vecKV[0];
            std::string value = vecKV[1];
            jsParams[key] = value;
        }
    }

    //字符串内容判断（字符或者数字）
    static bool StrIsNumOrAZaz(const string &strData,int iLen = 0)//ilen=0表示不判断长度
    {
        if(strData.empty())//字符串为空返回false
            return false;

        if(iLen !=0 && (int)strData.length() != iLen)//ilen参数不为0时,判断字符串的长度
            return false;

        for(size_t uiIdx = 0; uiIdx < strData.size(); ++uiIdx)
        {
            bool bTemp = (strData[uiIdx] >= '0' && strData[uiIdx] <= '9') || (strData[uiIdx] >= 'A' && strData[uiIdx] <= 'Z')
                        || (strData[uiIdx] >= 'a' && strData[uiIdx] <= 'z');
            if(!bTemp)
                return false;
        }
        return true;
    }

    //字符串内容判断（数字）不能用于判断负数
    static bool StrIsNum(const string &strData,int iLen = 0)//ilen=0表示不判断长度
    {
        if(strData.empty())//字符串为空返回false
            return false;

        if(iLen !=0 && (int)strData.length() != iLen)//ilen参数不为0时,判断字符串的长度
            return false;

        for(size_t uiIdx = 0; uiIdx < strData.size(); ++uiIdx)
        {
            bool bTemp = (strData[uiIdx] >= '0' && strData[uiIdx] <= '9');
            if(!bTemp)
                return false;
        }
        return true;
    }

    //字符串内容判断是否是数字，支持带正负符号的
    static bool StrIsNumWithSign(const string &strData)
    {
        if(strData.empty())//字符串为空返回false
            return false;

        char cSign = strData[0];
        if('-' == cSign || '+' == cSign)
            return StrIsNum(strData.substr(1));

        return StrIsNum(strData);
    }

    //字符串子串替换函数
    static string Replace(const string &sString, const string &sSrc, const string &sDest)
    {
        if(sSrc.empty() || sString.empty())
            return sString;

        //先做一次查找，判断是否存在需要替换的字符，再做后续操作，避免不必要的字符串拷贝
        string::size_type uiPos = sString.find(sSrc, 0);
        if(string::npos == uiPos) //未找到字符，则直接返回原始字符串即可
            return sString;

        string sBuf = sString;
        while(string::npos != uiPos)
        {
            sBuf.replace(uiPos, sSrc.length(), sDest);

            uiPos += sDest.length();
            uiPos = sBuf.find(sSrc, uiPos);
        }

        return sBuf;
    }

    static string BossTransStr(const string& str)
    {
        string strTemp = "";

        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (str[i] == 44 || str[i] == 92 || str[i] == 39 || str[i] == 34 || str[i] == 10 || str[i] == 13)
            {
                strTemp += "\\";
            }

            strTemp += str[i];
        }
        return strTemp;
    }

    //将英文引号转成中文引号
    static void TransQuotationMarkToChinese(string &strTitle)
    {
        string::size_type pos = 0;

        string strSrc = "\"";
        string strDst;
        int iNum = 0;
        while( (pos = strTitle.find(strSrc, pos)) != string::npos)
        {
            ++iNum;
            if(0 == iNum%2)
                strDst = "”";
            else
                strDst = "“";

            strTitle.replace(pos, strSrc.length(), strDst);
            pos += strDst.length();
        }

        return;
    }

    //去掉字符串首尾空格
    static string TrimSpace(const string &strSrc)
    {
        string strTemp = strSrc;

        if (strTemp.empty())
            return strTemp;

        strTemp.erase(0,strTemp.find_first_not_of(" "));
        strTemp.erase(strTemp.find_last_not_of(" ") + 1);

        return strTemp;
    }

    /**
    * @brief  去掉头部以及尾部的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string Trim(const string &sStr, const string &s = " \r\n\t", bool bChar = true)
    {
        if(sStr.empty())
        {
            return sStr;
        }

        /**
        * 将完全与s相同的字符串去掉
        */
        if(!bChar)
        {
            return CStrUnits::TrimRight(CStrUnits::TrimLeft(sStr, s, false), s, false);
        }

        return CStrUnits::TrimRight(CStrUnits::TrimLeft(sStr, s, true), s, true);
    }

    /**
    * @brief  去掉左边的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string TrimLeft(const string &sStr, const string &s = " \r\n\t", bool bChar = true)
    {
        if(sStr.empty())
        {
            return sStr;
        }

        /**
        * 去掉sStr左边的字符串s
        */
        if(!bChar)
        {
            if(sStr.length() < s.length())
            {
                return sStr;
            }

            if(sStr.compare(0, s.length(), s) == 0)
            {
                return sStr.substr(s.length());
            }

            return sStr;
        }

        /**
        * 去掉sStr左边的 字符串s中的字符
        */
        string::size_type pos = 0;
        while(pos < sStr.length())
        {
            if(s.find_first_of(sStr[pos]) == string::npos)
            {
                break;
            }

            pos++;
        }

        if(pos == 0) return sStr;

        return sStr.substr(pos);
    }

    /**
    * @brief  去掉右边的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string TrimRight(const string &sStr, const string &s = " \r\n\t", bool bChar = true)
    {
        if(sStr.empty())
        {
            return sStr;
        }

        /**
        * 去掉sStr右边的字符串s
        */
        if(!bChar)
        {
            if(sStr.length() < s.length())
            {
                return sStr;
            }

            if(sStr.compare(sStr.length() - s.length(), s.length(), s) == 0)
            {
                return sStr.substr(0, sStr.length() - s.length());
            }

            return sStr;
        }

        /**
        * 去掉sStr右边的 字符串s中的字符
        */
        string::size_type pos = sStr.length();
        while(pos != 0)
        {
            if(s.find_first_of(sStr[pos-1]) == string::npos)
            {
                break;
            }

            pos--;
        }

        if(pos == sStr.length()) return sStr;

        return sStr.substr(0, pos);
    }

    //A=x&B=y这种特殊类型的字符串解析为map结构
    static void TransStrKVToMap(const string &strKV, map<string,string> &mapKeyValue)
    {
        mapKeyValue.clear();

        vector<string> vecFields;
        CStrUnits::SplitToVec(strKV,"&",vecFields);
        for(size_t uiIdx = 0; uiIdx < vecFields.size(); ++uiIdx)
        {
            vector<string> vecKeyValue;
            CStrUnits::SplitToVec(vecFields[uiIdx],"=",vecKeyValue);
            if(2 == vecKeyValue.size())
            {
                mapKeyValue[vecKeyValue[0]] = vecKeyValue[1];
            }
        }

        return;
    }

    static string TransMapToStrKV(map<string,string> &mapKeyValue)
    {
        string strKV;
        for(map<string,string>::iterator it = mapKeyValue.begin(); it != mapKeyValue.end(); ++it)
        {
            if(!strKV.empty())
                strKV += "&";

            strKV += it->first + "=" + it->second;
        }

        return strKV;
    }

    //从A=x&B=y这种特殊类型的字符串中获取A或者B的值
    static int GetValueFromStrKV(const string &strKV, const string &strKey, string &strValue)
    {
        if(0 == strKV.size())
            return -1;

        string strRealKey = strKey + "=";
        string::size_type uiBegin = strKV.find(strRealKey);
        if(string::npos == uiBegin)
            return -1;

        uiBegin = uiBegin + strRealKey.size();
        string::size_type uiEnd = strKV.find('&', uiBegin);
        if(string::npos == uiEnd)
            uiEnd = strKV.size(); //没有找到&符号，说明是最后一个KV结构，直接到字符串末尾

        strValue.assign(strKV, uiBegin, uiEnd - uiBegin );

        if(0 == strValue.size())
            return -1;

        return 0;
    }

    static string GetValueFromStrKV(const string &strKV, const string &strKey)
    {
        if(0 == strKV.size())
            return "";

        string strRealKey = strKey + "=";
        string::size_type uiBegin = strKV.find(strRealKey);
        if(string::npos == uiBegin)
            return "";

        uiBegin = uiBegin + strRealKey.size();
        string::size_type uiEnd = strKV.find('&', uiBegin);
        if(string::npos == uiEnd)
            uiEnd = strKV.size(); //没有找到&符号，说明是最后一个KV结构，直接到字符串末尾

        string strValue;
        strValue.assign(strKV, uiBegin, uiEnd - uiBegin );

        return strValue;
    }

    //参数xss漏洞判断
    static bool CheckParamXss(const string &strParam)
    {
        if(strParam.empty())
            return false;

        if( (strParam.find("<") != string::npos) || (strParam.find(">") != string::npos)
            || (strParam.find("\"") != string::npos) || (strParam.find("\'") != string::npos)
            || (strParam.find("%") != string::npos) || (strParam.find("||") != string::npos)
            )
        {
            return true;
        }

        return false;
    }

    static string GetUrlParamWithCheckXss(const string &strParam)
    {
        if(CStrUnits::CheckParamXss(strParam))
            return "";

        return strParam;
    }

    static int ParseUrlParam(const string &strUrl, map<string, string> &mapKeys)
    {
        size_t iStart = strUrl.find('?');
        if(iStart == string::npos)
        {
            return -1; //url中没有找到get参数的起始位置，返回错误
        }
        iStart++;

        size_t iLeft = iStart;
        size_t iMid = 0;
        size_t iRight = 0;
        while(true)
        {
            iMid = strUrl.find('=', iLeft);
            if(iMid == string::npos)
            {
                break;
            }
            iRight = strUrl.find('&', iMid);
            if(iRight == string::npos)
            {
                iRight = strUrl.length();
            }

            mapKeys.insert(pair<string, string>(strUrl.substr(iLeft, iMid - iLeft), strUrl.substr(iMid + 1, iRight - iMid - 1)));
            iLeft = iRight + 1;
        }

        return 0;
    }

    static string GetCgiFromUrl(const string &strUrl)
    {
        string strCgi = strUrl;

        //先去掉参数
        size_t uiPos = strCgi.find('?');
        if(string::npos != uiPos)
            strCgi = strCgi.substr(0,uiPos);

        //如果有http头的话，去掉http
        string strTemp = "http://";
        uiPos = strCgi.find(strTemp);
        if(0 == uiPos)
            strCgi = strCgi.substr(uiPos+strTemp.size());

        strTemp = "https://";
        uiPos = strCgi.find(strTemp);
        if(0 == uiPos)
            strCgi = strCgi.substr(uiPos+strTemp.size());

        //找到第一个“/”,取包含“/”之后的字符串
        uiPos = strCgi.find("/");
        if(string::npos != uiPos)
            strCgi = strCgi.substr(uiPos);

        return strCgi;
    }

    static string GetHostFromUrl(const string &strUrl)
    {
        string strHost = strUrl;

        //先去掉参数
        size_t uiPos = strHost.find('?');
        if(string::npos != uiPos)
            strHost = strHost.substr(0,uiPos);

        //如果有http头的话，去掉http
        string strTemp = "http://";
        uiPos = strHost.find(strTemp);
        if(0 == uiPos)
            strHost = strHost.substr(uiPos+strTemp.size());

        strTemp = "https://";
        uiPos = strHost.find(strTemp);
        if(0 == uiPos)
            strHost = strHost.substr(uiPos+strTemp.size());

        //找到第一个“/”,取包含“/”之后的字符串
        uiPos = strHost.find("/");
        if(string::npos != uiPos)
            strHost = strHost.substr(0,uiPos);

        return strHost;
    }

    static string StrToUpper(const string &strInput)
    {
        string strOut = strInput;
        for(size_t uiIdx = 0; uiIdx < strOut.length(); uiIdx++)
        {
            if(strOut[uiIdx]>='a'&&strOut[uiIdx]<='z')
                strOut[uiIdx] -= 32;
        }

        return strOut;
    }

    static string StrToLower(const string &strInput)
    {
        string strOut = strInput;
        for(size_t uiIdx = 0; uiIdx < strOut.length(); uiIdx++)
        {
            if(strOut[uiIdx]>='A'&&strOut[uiIdx]<='Z')
                strOut[uiIdx] += 32;
        }

        return strOut;
    }

    static int64_t GetTimeSlice(int64_t llTimestamp, int iTimeInterval)
    {
        if(0 == iTimeInterval)
            return 0;

        return llTimestamp / iTimeInterval * iTimeInterval;
    }

    static string GetStrTimeSlice(int64_t llTimestamp, int iTimeInterval)
    {
        if(0 == iTimeInterval)
            return "";

        return CNum2Str::Num2Str(llTimestamp / iTimeInterval * iTimeInterval);
    }

    /**
    * @brief  "%xx" 转换为字符 例如"3D" 转换成'='
    * @param sWhat: %xx字符串后面的两个字符
    * @return char 返回字符
    */
    static char HexStrToC(const string &sWhat)
    {
        register char digit;

        if(sWhat.length() < 2)
        {
            return '\0';
        }

        digit = (sWhat[0] >= 'A' ? ((sWhat[0] & 0xdf) - 'A')+10 : (sWhat[0] - '0'));
        digit *= 16;
        digit += (sWhat[1] >= 'A' ? ((sWhat[1] & 0xdf) - 'A')+10 : (sWhat[1] - '0'));

        return(digit);
    }

    static int GetL5FromStr(int &iModid,int &iCmdid,const string &strL5)
    {
        if(strL5.empty())
            return -1;

        vector<string> vecFields;
        CStrUnits::SplitToVec(strL5, ":", vecFields);
        if(2 == vecFields.size())
        {
            iModid = atoi(vecFields[0].c_str());
            iCmdid = atoi(vecFields[1].c_str());

            if(iModid >= 0 && iCmdid >= 0)
            {
                return 0;
            }
        }

        return -2;
    }


    static void GetMapCookieFromStr(map<string,string> &mapCookie, const string &strCookie)
    {
        if(strCookie.empty())
            return ;

        vector<string> vecKeyValue;
        CStrUnits::SplitToVec(strCookie, ";", vecKeyValue);

        for(size_t uiIdx = 0; uiIdx < vecKeyValue.size(); ++uiIdx)
        {
           string &strKeyValue = vecKeyValue[uiIdx];
           size_t uiPos = strKeyValue.find("=");
           if(string::npos != uiPos)
           {
                mapCookie[ CStrUnits::TrimSpace(strKeyValue.substr(0,uiPos)) ] = CStrUnits::TrimSpace(strKeyValue.substr(uiPos+1));
           }
        }

        return ;
    }

    //写入时保持为json中的整数类型
    static string ConsIntJsonByInt(const string &strField, const int64_t &llIntValue, const string strEnd = ",")
    {
        return "\"" + strField + "\":" + CNum2Str::Num2Str(llIntValue) + strEnd;
    }

    //写入时保持为json中的字符串类型
    static string ConsStrJsonByStr(const string &strField, const string &strValue, const string strEnd = ",")
    {
        string strFormat = strValue;

        //处理特殊字符
        string::size_type uiPos = 0;
        if ((uiPos = strFormat.find("\\")) != string::npos) //反斜杠要先处理才行
            strFormat = CStrUnits::Replace(strFormat, "\\", "\\\\");

        if ((uiPos = strFormat.find("\"")) != string::npos)
            strFormat = CStrUnits::Replace(strFormat,"\"","\\\"");

        if ((uiPos = strFormat.find("\n")) != string::npos)
            strFormat = CStrUnits::Replace(strFormat, "\n", ""); //弹幕需要忽略换行符,这里将换行符替换为空

        return "\"" + strField + "\":\"" + strFormat + "\"" + strEnd;
    }

    //值为存储json对象的字符串，写入时保持为obj类型
    static string ConsObjJsonByStrObj(const string &strField, const string &strObjValue, const string strEnd = ",")
    {
        if(strObjValue.empty())
            return "\"" + strField + "\":{}" + strEnd;

        return "\"" + strField + "\":" + strObjValue + strEnd;
    }

    static string GetLineCRLF(const string &strData, size_t &uiIdx)
    {
        if (strData.empty() || uiIdx >= strData.size())
            return "";

        string strTmp;
        strTmp.reserve(512);

        while (strData[uiIdx] != '\r' && strData[uiIdx] != '\n' && uiIdx < strData.size())
        {
            strTmp.append(1, strData[uiIdx]);
            uiIdx++;
        }

        if (strData[uiIdx] == '\r')
        {
            uiIdx++; /* pass the char '\n' */
        }

        uiIdx++;

        return strTmp;
    }

  private:
    static int CodeConvert(const char *from_charset, const char *to_charset,const char *inbuf, size_t inlen,char *outbuf, size_t *outlen)
    {
        iconv_t cd;
        const char **pin = &inbuf;
        char **pout = &outbuf;
        cd = iconv_open(to_charset,from_charset);
        if (cd == (iconv_t)(-1))
            return -1;

        if (iconv(cd, (char **)pin, (size_t*)&inlen, pout, outlen)==(size_t)(-1))
        {
            iconv_close(cd);
            return -1;
        }
        iconv_close(cd);
        return 0 ;
    }

};

// ================ class CHashFunction===================================================
class CHashFunction
{
  public:

    //哈希time33算法
    static unsigned int HashTime33(const string &sKey,unsigned int uiInitNum)
    {
        unsigned int uiHash = uiInitNum;
        for (size_t uiIdx = 0; uiIdx < sKey.length(); ++uiIdx)
        {
            uiHash += (uiHash << 5) + sKey[uiIdx];
        };

        return uiHash & 0x7fffffff;
    }

    // BKDR Hash Function
    static unsigned int BKDRHash(const string &sKey)
    {
        unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
        unsigned int hash = 0;

        for (size_t uiIdx = 0; uiIdx < sKey.length(); ++uiIdx)
        {
            hash = hash * seed + sKey[uiIdx];
        }

        return (hash & 0x7FFFFFFF);
    }

    static unsigned int DJBHash(const string &sKey)
    {
        unsigned int hash = 5381;

        for (size_t uiIdx = 0; uiIdx < sKey.length(); ++uiIdx)
        {
            hash += (hash << 5) + sKey[uiIdx];
        }

        return (hash & 0x7FFFFFFF);
    }
};

// ================ class CJudgeAppVerBy3Slice===================================================
//比较app版本号(分片必须大于等于3个，即必须为4.5.1或者4.5.1.0001这样的，4.5这种版本号是不支持的)，
//注意:这里比较大小不要用相反的操作，例如判断版本比较大，就用IsBigger，不要用IsLittle再取反
class CJudgeAppVerBy3Slice
{
  public:
    //注意:这里比较大小不要用相反的操作，例如判断版本比较大，就用IsBigger，不要用IsLittle再取反

    static bool IsBigger(const string &strAppVerSrc, const string &strAppVerDst)
    {
        int64_t llSrc = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerSrc);
        int64_t llDst = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerDst);

        if(llSrc > 0 && llDst > 0 && llSrc > llDst)
            return true;

        return false;
    }

    static bool IsBiggerAndEqual(const string &strAppVerSrc, const string &strAppVerDst)
    {
        int64_t llSrc = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerSrc);
        int64_t llDst = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerDst);

        if(llSrc > 0 && llDst > 0 && llSrc >= llDst)
            return true;

        return false;
    }

    static bool IsEqual(const string &strAppVerSrc, const string &strAppVerDst)
    {
        int64_t llSrc = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerSrc);
        int64_t llDst = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerDst);

        if(llSrc > 0 && llDst > 0 && llSrc == llDst)
            return true;

        return false;
    }

    static bool IsLittle(const string &strAppVerSrc, const string &strAppVerDst)
    {
        int64_t llSrc = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerSrc);
        int64_t llDst = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerDst);

        if(llSrc > 0 && llDst > 0 && llSrc < llDst)
            return true;

        return false;
    }

    static bool IsLittleAndEqual(const string &strAppVerSrc, const string &strAppVerDst)
    {
        int64_t llSrc = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerSrc);
        int64_t llDst = CJudgeAppVerBy3Slice::TransStrAppverToNum(strAppVerDst);

        if(llSrc > 0 && llDst > 0 && llSrc <= llDst)
            return true;

        return false;
    }

  private:

    static int64_t TransStrAppverToNum(const string &strAppver)
    {
        vector<string> vecSlice;
        CStrUnits::SplitToVec(strAppver, ".", vecSlice);
        if(vecSlice.size() < 3) // 分片必须小于3个的直接返回-1
            return -1;

        int64_t llSlice_1 = atoll(vecSlice[0].c_str());
        int64_t llSlice_2 = atoll(vecSlice[1].c_str());
        int64_t llSlice_3 = atoll(vecSlice[2].c_str());

        if(llSlice_1 < 0 || llSlice_2 < 0 || llSlice_3 < 0)
            return -1;

        return llSlice_1*10000*10000 + llSlice_2*10000 + llSlice_3*1;
    }
};

// ================ class COptIP===================================================
class COptIP
{
  public:
    static string IPV4IntToStrByNetByteOrder(uint32_t iNetIp) //参数是网络字节序的ip地址
    {
        struct sockaddr_in addr;
        addr.sin_addr.s_addr = iNetIp;
        string strIp = inet_ntoa(addr.sin_addr);
        return strIp;
    }

    static string IPV4IntToStrByHostByteOrder(uint32_t iHostIp) //参数是主机字节序的ip地址
    {
        struct sockaddr_in addr;
        addr.sin_addr.s_addr = htonl(iHostIp);
        string strIp = inet_ntoa(addr.sin_addr);
        return strIp;
    }

    static uint32_t IPV4StrToInt(const string &strIp)
    {
        uint32_t uiIp = ntohl(inet_addr(strIp.c_str()));
        return uiIp;
    }

    static int GetLocalHostIp(string &strLocalIp)
    {
        struct ifaddrs *ifAddrSt = NULL;

        getifaddrs(&ifAddrSt);
        struct ifaddrs *ifAddrHead = ifAddrSt;

        while(ifAddrSt != NULL)
        {
            if(strncmp(ifAddrSt->ifa_name, "eth1", 4)==0 && ifAddrSt->ifa_addr->sa_family==AF_INET)
            {
                char szIp[INET_ADDRSTRLEN];
                void* sinAddrPtr = &((struct sockaddr_in*)ifAddrSt->ifa_addr)->sin_addr;
                inet_ntop(AF_INET, sinAddrPtr, szIp, INET_ADDRSTRLEN);
                strLocalIp = szIp;
                break;
            }
            ifAddrSt=ifAddrSt->ifa_next;
        }
        freeifaddrs(ifAddrHead);
        return 0;
    }

    static string GetLocalHostIp()
    {
        string strLocalIp;

        struct ifaddrs *ifAddrSt = NULL;
        getifaddrs(&ifAddrSt);
        struct ifaddrs *ifAddrHead = ifAddrSt;

        while(ifAddrSt != NULL)
        {
            if(strncmp(ifAddrSt->ifa_name, "eth1", 4)==0 && ifAddrSt->ifa_addr->sa_family==AF_INET)
            {
                char szIp[INET_ADDRSTRLEN];
                void* sinAddrPtr = &((struct sockaddr_in*)ifAddrSt->ifa_addr)->sin_addr;
                inet_ntop(AF_INET, sinAddrPtr, szIp, INET_ADDRSTRLEN);
                strLocalIp = szIp;
                break;
            }
            ifAddrSt = ifAddrSt->ifa_next;
        }
        freeifaddrs(ifAddrHead);

        return strLocalIp;
    }
};

// ================ class CHTMLCode===================================================
class CHTMLCode
{
  public:
    //HTML编码函数
    static string HTMLEncode(const string &strSrc, bool blankEncode)
    {
        if (strSrc == "")
            return "";


        string result = strSrc;

        if(string::npos != result.find("&", 0)) //找到字符，才处理，避免不必要的拷贝
            result = CStrUnits::Replace(result, "&", "&amp;");

        if(string::npos != result.find("<", 0))
            result = CStrUnits::Replace(result, "<", "&lt;");

        if(string::npos != result.find(">", 0))
            result = CStrUnits::Replace(result, ">", "&gt;");

        if(string::npos != result.find("\"", 0))
            result = CStrUnits::Replace(result, "\"", "&quot;");

        if(string::npos != result.find("\'", 0))
            result = CStrUnits::Replace(result, "\'", "&#39;");

        if (blankEncode)
        {
            if(string::npos != result.find("\t", 0))
                result = CStrUnits::Replace(result, "\t", "    ");

            if(string::npos != result.find("  ", 0))
                result = CStrUnits::Replace(result, "  ", "&nbsp; ");

            if(string::npos != result.find("\r\n", 0))
                result = CStrUnits::Replace(result, "\r\n", "<br>");

            if(string::npos != result.find("\n", 0))
                result = CStrUnits::Replace(result, "\n", "<br>");

        }
        return result;
    }

    //HTML解码函数
    static string HTMLDecode(const string &strSrc)
    {
        if (strSrc == "")
            return "";

        string result = strSrc;

        if(string::npos != result.find("&amp;", 0))
            result = CStrUnits::Replace(result,"&amp;", "&");

        if(string::npos != result.find("&lt;", 0))
            result = CStrUnits::Replace(result,"&lt;", "<");

        if(string::npos != result.find("&gt;", 0))
            result = CStrUnits::Replace(result,"&gt;", ">");

        if(string::npos != result.find("&quot;", 0))
            result = CStrUnits::Replace(result,"&quot;", "\"");

        if(string::npos != result.find("&nbsp; ", 0))
            result = CStrUnits::Replace(result,"&nbsp; ", "  ");

        if(string::npos != result.find("&#39;", 0))
            result = CStrUnits::Replace(result,"&#39;", "'");

        if(string::npos != result.find("&#039;", 0))
            result = CStrUnits::Replace(result,"&#039;", "'");

        if(string::npos != result.find("<br>", 0))
            result = CStrUnits::Replace(result, "<br>", "\r\n");


        return result;
    }
};

// ================ class CUrlCode===================================================

//URL中的空格有时候被编码成%20，有时候被编码成加号+
//一个URL的基本组成部分包括协议(scheme),域名，端口号，路径和查询字符串（路径参数和锚点标记就暂不考虑了）。
//路径和查询字符串之间用问号?分离。例如http://www.example.com/index?param=1，路径为index，查询字符串(Query String)为param=1。
//URL中关于空格的编码正是与空格所在位置相关：空格被编码成加号+的情况只会在查询字符串部分出现，而被编码成%20则可以出现在路径和查询字符串中。
//造成这种混乱局面的原因在于：
//W3C标准规定，当Content-Type为application/x-www-form-urlencoded时，URL中查询参数名和参数值中空格要用加号+替代，
//所以几乎所有使用该规范的浏览器在表单提交后，URL查询参数中空格都会被编成加号+。
//而在另一份规范(RFC 2396，定义URI)里, URI里的保留字符都需转义成%HH格式(Section 3.4 Query Component)，因此空格会被编码成%20，
//加号+本身也作为保留字而被编成%2B，对于某些遵循RFC 2396标准的应用来说，它可能不接受查询字符串中出现加号+，认为它是非法字符。
//所以一个安全的举措是URL中统一使用%20来编码空格字符。

class CUrlCode //url编解码，废弃，统一改用CNewUrlCode
{
  public:
    //url编码
    static string UrlEncode(const string& str)
    {
        string strTemp = "";

        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (isalnum((unsigned char)str[i]) ||(str[i] == '-') ||(str[i] == '_') ||(str[i] == '.') ||(str[i] == '~'))
                strTemp += str[i];
            else if (str[i] == ' ')
                strTemp += "+";
            else
            {
                strTemp += '%';
                strTemp += ToHex((unsigned char)str[i] >> 4);
                strTemp += ToHex((unsigned char)str[i] % 16);
            }
        }
        return strTemp;
    }

    //url解码
    static string UrlDecode(const string& str)
    {
        string strTemp = "";

        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (str[i] == '+') strTemp += ' ';
            else if (str[i] == '%')
            {
                assert(i + 2 < length);
                unsigned char high = FromHex((unsigned char)str[++i]);
                unsigned char low = FromHex((unsigned char)str[++i]);
                strTemp += high*16 + low;
            }
            else strTemp += str[i];
        }
        return strTemp;
    }

    //url中参数值的url编码，只处理“=”和“&”
    static string UrlParamDecode(const string &strParamValue)
    {
        string strParamValueTmp = strParamValue;

        string strSrc = "=";
        string strDst = "%3D";
        string::size_type pos=0;
        string::size_type srclen=strSrc.size();
        string::size_type dstlen=strDst.size();
        while( (pos=strParamValueTmp.find(strSrc, pos)) != string::npos)
        {
            strParamValueTmp.replace(pos, srclen, strDst);
            pos += dstlen;
        }

        strSrc = "&";
        strDst = "%26";
        pos=0;
        srclen=strSrc.size();
        dstlen=strDst.size();
        while( (pos=strParamValueTmp.find(strSrc, pos)) != string::npos)
        {
            strParamValueTmp.replace(pos, srclen, strDst);
            pos += dstlen;
        }

        return strParamValueTmp;
    }

  private:
    static unsigned char ToHex(unsigned char x)
    {
        return  x > 9 ? x + 55 : x + 48;
    }

    // 16进制
    static unsigned char FromHex(unsigned char x)
    {
        unsigned char y;
        if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
        else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
        else if (x >= '0' && x <= '9') y = x - '0';
        else assert(0);
        return y;
    }
};

// ================ class CNewUrlCode===================================================
class CNewUrlCode //url编解码
{
  public:
    //url编码
    static string UrlEncode(const string& strUrl)
    {
        static char HEX_TABLE[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

        string result;
        for (size_t i = 0; i < strUrl.length(); i++)
        {
            char c = strUrl[i];

            //RFC3986文档规定，Url中只允许包含英文字母（a-zA-Z）、数字（0-9）、-_.~4个特殊字符以及所有保留字符
            if ( isalnum((unsigned char)c) || (c == '-') || (c == '_') || (c == '.') || (c == '~') )
            {
                result.append(1, c);
            }
            else //这里就不将空格编码成+了，全部转成%XX的形式
            {
                result.append(1, '%');
                result.append(1, HEX_TABLE[(c >> 4) & 0x0f]);
                result.append(1, HEX_TABLE[c & 0x0f]);
            }
        }

        return result;
    }

    //url解码
    static string UrlDecode(const string& strUrl)
    {
        string sDecodeUrl;
        register string::size_type pos = 0;
        string::size_type len = strUrl.length();

        sDecodeUrl = "";

        while (pos < len)
        {
            if('%' == strUrl[pos])
            {
                sDecodeUrl += CStrUnits::HexStrToC(strUrl.substr(pos + 1));
                pos += 3;
            }
            else if('+' == strUrl[pos]) //兼容处理将空格编码成+的情况
            {
                sDecodeUrl += ' ';
                ++pos;
            }
            else
            {
                sDecodeUrl += strUrl[pos];
                ++pos;
            }
        }

        return sDecodeUrl;
    }

    //url中参数值的url编码，只处理“=”和“&”
    static string UrlParamDecode(const string &strParamValue)
    {
        string strParamValueTmp = strParamValue;

        string strSrc = "=";
        string strDst = "%3D";
        string::size_type pos=0;
        string::size_type srclen=strSrc.size();
        string::size_type dstlen=strDst.size();
        while( (pos=strParamValueTmp.find(strSrc, pos)) != string::npos)
        {
            strParamValueTmp.replace(pos, srclen, strDst);
            pos += dstlen;
        }

        strSrc = "&";
        strDst = "%26";
        pos=0;
        srclen=strSrc.size();
        dstlen=strDst.size();
        while( (pos=strParamValueTmp.find(strSrc, pos)) != string::npos)
        {
            strParamValueTmp.replace(pos, srclen, strDst);
            pos += dstlen;
        }

        return strParamValueTmp;
    }
};

// ================ class CTimer===================================================
class CTimer //计时器
{
  public:
    CTimer()
    {
        gettimeofday(&m_tv_begin, NULL);
    }
    virtual ~CTimer()
    {

    }
    void restart()
    {
        gettimeofday(&m_tv_begin, NULL);
    }
    int elapsed_us()
    {
        gettimeofday(&m_tv_end, NULL);
        return static_cast<int>(1000000 * (m_tv_end.tv_sec - m_tv_begin.tv_sec) + m_tv_end.tv_usec - m_tv_begin.tv_usec);
    }

    int elapsed_ms()
    {
        return elapsed_us()/1000;
    }

    static int64_t GetCurrentTime_us()
    {
        struct timeval tv_cur;
        gettimeofday(&tv_cur, NULL);

        return (tv_cur.tv_sec)*1000000 + tv_cur.tv_usec;
    }

    static int64_t GetCurrentTime_ms()
    {
        struct timeval tv_cur;
        gettimeofday(&tv_cur, NULL);

        return (tv_cur.tv_sec)*1000 + (tv_cur.tv_usec)/1000;
    }

  private:
    struct timeval m_tv_begin;
    struct timeval m_tv_end;
};

// ================ class CCstl===================================================
class CCstl
{
  public:
    //出生日期转星座标识,int返回星座序号，字符串表示描述信息
    static int TransDate2Cstl(int iInputDate,string *pStrOutCstlDesc = NULL)
    {
        int iCstl = 0;
        string strOutCstlDesc;
        if( iInputDate >= 120 && iInputDate <=218)
        {
            strOutCstlDesc = "水瓶座";
            iCstl = 1;
        }
        else if( iInputDate >= 219 && iInputDate <=320)
        {
            strOutCstlDesc = "双鱼座";
            iCstl = 2;
        }
        else if( iInputDate >= 321 && iInputDate <=419)
        {
            strOutCstlDesc = "白羊座";
            iCstl = 3;
        }
        else if( iInputDate >= 420 && iInputDate <=520)
        {
            strOutCstlDesc = "金牛座";
            iCstl = 4;
        }
        else if( iInputDate >= 521 && iInputDate <=621)
        {
            strOutCstlDesc = "双子座";
            iCstl = 5;
        }
        else if( iInputDate >= 622 && iInputDate <=722)
        {
            strOutCstlDesc = "巨蟹座";
            iCstl = 6;
        }
        else if( iInputDate >= 723 && iInputDate <=822)
        {
            strOutCstlDesc = "狮子座";
            iCstl = 7;
        }
        else if( iInputDate >= 823 && iInputDate <=922)
        {
            strOutCstlDesc = "处女座";
            iCstl = 8;
        }
        else if( iInputDate >= 923 && iInputDate <=1023)
        {
            strOutCstlDesc = "天秤座";
            iCstl = 9;
        }
        else if( iInputDate >= 1024 && iInputDate <=1121)
        {
            strOutCstlDesc = "天蝎座";
            iCstl = 10;
        }
        else if( iInputDate >= 1122 && iInputDate <=1221)
        {
            strOutCstlDesc = "射手座";
            iCstl = 11;
        }
        else if( iInputDate >= 1222  && iInputDate <=1231)
        {
            strOutCstlDesc = "摩羯座";
            iCstl = 12;
        }
        else if( iInputDate >= 101 && iInputDate <=119)
        {
            strOutCstlDesc = "摩羯座";
            iCstl = 12;
        }

        if(pStrOutCstlDesc != NULL)
            *pStrOutCstlDesc = strOutCstlDesc;

        return iCstl;
    }

    static string GetCstlDesc(int iCstl)
    {
        string strOutCstlDesc;
        if( 1 == iCstl )
            strOutCstlDesc = "水瓶座";
        else if( 2 == iCstl )
            strOutCstlDesc = "双鱼座";
        else if( 3 == iCstl )
            strOutCstlDesc = "白羊座";
        else if( 4 == iCstl )
            strOutCstlDesc = "金牛座";
        else if( 5 == iCstl )
            strOutCstlDesc = "双子座";
        else if( 6 == iCstl )
            strOutCstlDesc = "巨蟹座";
        else if( 7 == iCstl )
            strOutCstlDesc = "狮子座";
        else if( 8 == iCstl )
            strOutCstlDesc = "处女座";
        else if( 9 == iCstl )
            strOutCstlDesc = "天秤座";
        else if( 10 == iCstl )
            strOutCstlDesc = "天蝎座";
        else if( 11 == iCstl )
            strOutCstlDesc = "射手座";
        else if( 12 == iCstl )
            strOutCstlDesc = "摩羯座";

        return strOutCstlDesc;
    }
};

// ================ class IgnoreSignal===================================================
//信号处理类型
class IgnoreSignal
{
  public:
    static int IgnoreAll()
    {
        signal(SIGTSTP,SIG_IGN);
        signal(SIGHUP,  SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGSTOP, SIG_IGN);
        signal(SIGTERM, SIG_IGN);
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        return 0;
    }

    static int IgnoreDaemon()
    {
        signal(SIGHUP  ,SIG_IGN );   /* hangup, generated when terminal disconnects */
        signal(SIGINT  ,SIG_IGN );   /* interrupt, generated from terminal special char */
        signal(SIGQUIT ,SIG_IGN );   /* (*) quit, generated from terminal special char */
        signal(SIGILL  ,SIG_IGN );   /* (*) illegal instruction (not reset when caught)*/
        signal(SIGTRAP ,SIG_IGN );   /* (*) trace trap (not reset when caught) */
        signal(SIGABRT ,SIG_IGN );   /* (*) abort process */
        signal(SIGFPE  ,SIG_IGN );   /* (*) floating point exception */
        signal(SIGKILL ,SIG_IGN );   /* kill (cannot be caught or ignored) */
        signal(SIGBUS  ,SIG_IGN );   /* (*) bus error (specification exception) */
        signal(SIGSEGV ,SIG_IGN );   /* (*) segmentation violation */
        signal(SIGSYS  ,SIG_IGN );   /* (*) bad argument to system call */
        signal(SIGPIPE ,SIG_IGN );   /* write on a pipe with no one to read it */
        signal(SIGALRM ,SIG_IGN );   /* alarm clock timeout */
        //signal(SIGTERM ,stopproc );  /* software termination signal */
        signal(SIGURG  ,SIG_IGN );   /* (+) urgent contition on I/O channel */
        signal(SIGSTOP ,SIG_IGN );   /* (@) stop (cannot be caught or ignored) */
        signal(SIGTSTP ,SIG_IGN );   /* (@) interactive stop */
        signal(SIGCONT ,SIG_IGN );   /* (!) continue (cannot be caught or ignored) */
        signal(SIGCHLD ,SIG_IGN);    /* (+) sent to parent on child stop or exit */
        signal(SIGTTIN ,SIG_IGN);    /* (@) background read attempted from control terminal*/
        signal(SIGTTOU ,SIG_IGN);    /* (@) background write attempted to control terminal */
        signal(SIGIO   ,SIG_IGN);    /* (+) I/O possible, or completed */
        signal(SIGXCPU ,SIG_IGN);    /* cpu time limit exceeded (see setrlimit()) */
        signal(SIGXFSZ ,SIG_IGN);    /* file size limit exceeded (see setrlimit()) */
        signal(SIGWINCH,SIG_IGN);    /* (+) window size changed */
        signal(SIGPWR  ,SIG_IGN);    /* (+) power-fail restart */
        //signal(SIGUSR1 ,stopproc);   /* user defined signal 1 */
        //signal(SIGUSR2 ,stopproc);   /* user defined signal 2 */
        signal(SIGPROF ,SIG_IGN);    /* profiling time alarm (see setitimer) */
        signal(SIGVTALRM,SIG_IGN);   /* virtual time alarm (see setitimer) */
        return 0;
    }
};


// ================ class CDate===================================================
// 日期处理类
class CDate
{
public:
    static bool IsSameDay(const tm & a, const tm& b)
    {
        return ((a.tm_year == b.tm_year)
            && (a.tm_mon == b.tm_mon)
            && (a.tm_mday == b.tm_mday));
    }

    static bool IsSameYear(const tm & a, const tm& b)
    {
        return (a.tm_year == b.tm_year);
    }

    static bool IsSameDay(time_t a, time_t b)
    {
        struct tm tmA;
        struct tm tmB;
            
        localtime_r((time_t*)&a, &tmA);
        localtime_r((time_t*)&b, &tmB);

        return CDate::IsSameDay(tmA, tmB);
    }
};

// ================ class AntiSnow===================================================
// ====防雪球结构体设置========
class AntiSnow
{
    uint32_t dwLastTime;
    uint32_t dwCurReqNum;
    uint32_t dwReqLimit;

  public:
    AntiSnow(uint32_t dwLimit = 0) : dwReqLimit(dwLimit)
    {
        dwLastTime = 0;
        dwCurReqNum = 0;
    }

    inline void setReqLimit(uint32_t dwLimit) { dwReqLimit = dwLimit; }
    inline uint32_t getReqLimit() { return dwReqLimit; }
    inline uint32_t getCurReqNum() { return dwCurReqNum; }

    bool needsAntiSnow()
    {
        struct timeval tval;
        gettimeofday(&tval, NULL);

        dwCurReqNum++;

        //req/second
        if (dwLastTime == (uint32_t)tval.tv_sec)
        {
            if(dwCurReqNum >= dwReqLimit)
                return true;
            else
                return false;
        }
        else
        {
            dwLastTime = tval.tv_sec;
            dwCurReqNum = 1;
            return false;
        }
    }
};

} //namespace MyUtils

#endif	//_MY_UTILS_H_
