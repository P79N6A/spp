/*
 *  Filename: http_tool.h
 *
 *  Created on: 2017-09-06
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 将http协议相关的操作函数整理到这个文件空，方便非spp服务的程序调用
 *
 */

#ifndef __HTTP_TOOL__
#define __HTTP_TOOL__

#include <map>
#include <zlib.h>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <vector>

using namespace std;

//http请求方式的枚举定义
enum EmHttpType
{
    HTTP_GET  = 1,
    HTTP_POST = 2,

    HTTP_RSP  = 20,
};

//http包数据
struct CmpCase
{
    bool operator()(const string &s1, const string &s2) const
    {
        if(strcasecmp(s1.c_str(), s2.c_str()) < 0)
            return true;
        else
            return false;
    }
};

typedef multimap<string, string, CmpCase> MMapHttpHead; //比较时，忽略大小写

struct StHttpData
{
    EmHttpType   eHttpType;    //http类型
    string       strReqUrl;    //请求url(post请求只有cgi，get请求有cgi和请求参数)
    string       strReqCgi;    //请求cgi
    string       strRspStatus; //响应状态码
    string       strRspDesc;   //响应原因说明
    string       strVersion;   //http版本
    MMapHttpHead mmapHttpHead; //http的包头数据(Set-Cookie和Cookie可以有多个头,其他头部字段不能重复)
    string       strBody;      //http消息体(get请求是从requrl中解析出来的)

    /**
	 * @brief 获取http头部参数，(Set-Cookie和Cookie<可能存在多个>不要用这个获取,直接从mampHttpHead结构中取)
     * 例如
     * for(MMapHttpHead::iterator it = mmapHttpHead.equal_range("Cookie").first; it != mmapHttpHead.equal_range("Cookie").second; ++it)
     * {
     *     m_mmapHttpHead.insert(MMapHttpHead::value_type("Cookie",it->second));
     * }
     * for(MMapHttpHead::iterator it = mmapHttpHead.equal_range("Set-Cookie").first; it != mmapHttpHead.equal_range("Set-Cookie").second; ++it)
     * {
     *     m_mmapHttpHead.insert(MMapHttpHead::value_type("Set-Cookie",it->second));
     * }
     *
	 * @param sHeader header名字
     * @return        header的相关信息
     */
    string GetHeader(const string &strHeadName) const
    {
        MMapHttpHead::const_iterator it = mmapHttpHead.find(strHeadName);
        if(it == mmapHttpHead.end())
        {
            return "";
        }

        return it->second;
    }

    void SetHeader(const string &strHeadName, const string &strHeadValue)
    {
        if(strHeadValue.empty())
            return ;

        //Set-Cookie和Cookie可以有多个头
        const char *pStr1 = "SET-COOKIE";
        const char *pStr2 = "COOKIE";
        if((strcasecmp(strHeadName.c_str(), pStr1) != 0) && (strcasecmp(strHeadName.c_str(), pStr2) != 0))
            mmapHttpHead.erase(strHeadName);

        mmapHttpHead.insert(MMapHttpHead::value_type(strHeadName, strHeadValue));

        return ;
    }

    void DelHeader(const string &strHeadName)
    {
        mmapHttpHead.erase(strHeadName);

        return ;
    }

    string GetMethod()
    {
        if(HTTP_GET == eHttpType)
            return "GET";
        else if(HTTP_POST == eHttpType)
            return "POST";
        else
            return "";
    }

    void WriteStHttpDataToStr(string &strHttpRspPkg)
    {
        strHttpRspPkg.clear();

        //添加首行
        if(HTTP_GET == eHttpType || HTTP_POST == eHttpType)
        {
            strHttpRspPkg.append(GetMethod() + " " + strReqUrl + " " + strVersion + "\r\n");
        }
        else if(HTTP_RSP == eHttpType)
        {
            strHttpRspPkg.append(strVersion + " " + strRspStatus + " " + strRspDesc + "\r\n");
        }
        else
        {
            return;
        }

        //添加http的head信息
        for(MMapHttpHead::const_iterator it = mmapHttpHead.begin(); it != mmapHttpHead.end(); ++it)
        {
            //没有对应值的字段过滤掉
            if(it->first.empty() || it->second.empty())
            {
                continue;
            }

            strHttpRspPkg.append(it->first);
            strHttpRspPkg.append(": ");
            strHttpRspPkg.append(it->second);
            strHttpRspPkg.append("\r\n");
        }

        strHttpRspPkg.append("\r\n");
        strHttpRspPkg.append(strBody); //无body，这里就是空值

        return;
    }

    //打印全部数据
    string DisplayAllToStr()
    {
        stringstream ssTemp;
        ssTemp<<"httpType ==> "<<eHttpType<<" || 1==>GET 2==>POST 20==>RSP"<<endl
              <<"reqUrl ==> "<<strReqUrl<<endl
              <<"reqCgi ==> "<<strReqCgi<<endl
              <<"rspStatus ==> "<<strRspStatus<<endl
              <<"rspDesc ==> "<<strRspDesc<<endl
              <<"httpVer ==> "<<strVersion<<endl;

        ssTemp<<"HttpHead ==> "<<endl;
        MMapHttpHead::const_iterator it = mmapHttpHead.begin();
        for(; it != mmapHttpHead.end(); ++it)
        {
            ssTemp<<(it->first)<<": "<<(it->second)<<endl;
        }

        ssTemp<<"HttpBody ==> "<<endl
              <<strBody<<endl;

        return ssTemp.str();
    }
};

class COptHttpData
{
public:
	COptHttpData(){};
	~COptHttpData(){};

public:
    //对外静态函数，主要用于校验http和解析http

    //构造http请求包
    static void ConstructHttpReqPkt(string &strHttpReqPkg,const string &strCgi,const string &strBody,EmHttpType eHttpType,MMapHttpHead &mmapHttpHead);

    static void ConstructHttpReqPkt(string &strHttpReqPkg,const string &strHost,const string &strCgi,const string &strBody
                    ,EmHttpType eHttpType,const string &strCookie,const string &strRefer);

    //构造http回包
    static void ConstructHttpRspPkt(string &strHttpRspPkg,const string &strBody,MMapHttpHead &mmapHttpHead,bool bUseGZip = false);
    static void ConstructHttpRspPkt(string &strHttpRspPkg,const string &strBody);

    //校验http包的函数
    static int  CheckHttpPkt(void *pBuf,int iBufLen);

    //解析http包--获取全部http包数据
    //解析全部http包数据，包括http选项值、cookie值、body数据、host等等
    static int  DecodeHttpPkt(const char* pBuf,int iBufLen,StHttpData &stHttpData);

    //从文本中解析出http头信息(单独解析头时或文本只包含http头信息时使用,)
    static void GetHttpHeadFromStrPkt(const string &strHttpPkt,MMapHttpHead &mmapHttpHead);

    //从文本信息中解析cookie数据存放到map结构中
    static void GetMapCookieFromStr(map<string,string> &mapCookie, const string &strCookie);

    static int DecodeHttpGZip(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata);  //http gzip解压
    static int EncodeHttpGZip(Bytef *data, uLong ndata,Bytef *zdata, uLong *nzdata); //http gzip压缩

    static string GetHeadFromBuf(const char *pBuf,string strHeadName); //不解析整个http包，只从数据buf查找，减少不必要的操作
    static string GetCgiFromBuf(const char *pBuf);//不解析整个http包，只从数据buf查找，减少不必要的操作

    static string GetLineCRLF(const char **ppChar); //*ppChar的地址会被修改的
    static string GetLineCRLF(const string &strHttpPkt,size_t &uiIdx);


    /**
    * @brief  去掉头部以及尾部的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string Trim(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
    * @brief  去掉左边的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string TrimLeft(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
    * @brief  去掉右边的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string TrimRight(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    static void SplitToVec(const string &strSource, const string &strDelimiter, vector<string> &vec, bool bCleanEmpty = true)
    {
        vec.clear();
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

    template<typename T>
    static string Num2Str(T n)
    {
        stringstream os;
        os << n;
        return os.str();
    }
};


#endif  /*__HTTP_TOOL__*/
