/*
 *  Filename: http_tool.cpp
 *
 *  Created on: 2017-09-06
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 将http协议相关的操作函数整理到这个文件空，方便非spp服务的程序调用
 *
 */

#include "http_tool.h"

void COptHttpData::ConstructHttpReqPkt(string &strHttpReqPkg,const string &strCgi,const string &strBody,EmHttpType eHttpType,MMapHttpHead &mmapHttpHead)
{
    strHttpReqPkg.clear();

    if(HTTP_POST == eHttpType)
    {
        strHttpReqPkg.append("POST " + strCgi + " HTTP/1.1\r\n");

        for(MMapHttpHead::const_iterator it = mmapHttpHead.begin(); it != mmapHttpHead.end(); ++it)
        {
            //没有对应值的字段过滤掉
            if(it->first.empty() || it->second.empty())
            {
                continue;
            }

            strHttpReqPkg.append(it->first);
            strHttpReqPkg.append(": ");
            strHttpReqPkg.append(it->second);
            strHttpReqPkg.append("\r\n");
        }

        strHttpReqPkg.append("Content-Length: " + COptHttpData::Num2Str(strBody.size()) + "\r\n");
        strHttpReqPkg.append("\r\n");
        strHttpReqPkg.append(strBody);
    }
    else if(HTTP_GET == eHttpType)
    {
        strHttpReqPkg.append("GET " + strCgi + "?" + strBody + " HTTP/1.1\r\n");

        for(MMapHttpHead::const_iterator it = mmapHttpHead.begin(); it != mmapHttpHead.end(); ++it)
        {
            //没有对应值的字段过滤掉
            if(it->first.empty() || it->second.empty())
            {
                continue;
            }

            strHttpReqPkg.append(it->first);
            strHttpReqPkg.append(": ");
            strHttpReqPkg.append(it->second);
            strHttpReqPkg.append("\r\n");
        }

        strHttpReqPkg.append("\r\n");
    }
}

void COptHttpData::ConstructHttpReqPkt(string &strHttpReqPkg,const string &strHost,const string &strCgi,const string &strBody
                                ,EmHttpType eHttpType,const string &strCookie,const string &strRefer)
{
    strHttpReqPkg.clear();

    if(eHttpType == HTTP_POST)
    {
        strHttpReqPkg.append("POST " + strCgi + " HTTP/1.1\r\n");
        strHttpReqPkg.append("Host: " + strHost + "\r\n");
        strHttpReqPkg.append("Accpet: */*\r\n");
        strHttpReqPkg.append("Content-Type: application/x-www-form-urlencoded\r\n");

        if(!strRefer.empty()){strHttpReqPkg.append("Referer: " + strRefer + "\r\n");}
        if(!strCookie.empty()){strHttpReqPkg.append("Cookie: " + strCookie + "\r\n");}

        strHttpReqPkg.append("Content-Length: " + COptHttpData::Num2Str(strBody.size()) + "\r\n");
        strHttpReqPkg.append("\r\n");
        strHttpReqPkg.append(strBody);
    }
    else if(eHttpType == HTTP_GET)
    {
        strHttpReqPkg.append("GET " + strCgi + "?" + strBody + " HTTP/1.1\r\n");
        strHttpReqPkg.append("Host: " + strHost + "\r\n");
        strHttpReqPkg.append("Accpet: */*\r\n");
        strHttpReqPkg.append("User-Agent: Wget/1.12 (linux-gnu)\r\n");
        strHttpReqPkg.append("Connection: Keep-Alive\r\n");

        if(!strRefer.empty()){strHttpReqPkg.append("Referer: " + strRefer + "\r\n");}
        if(!strCookie.empty()){strHttpReqPkg.append("Cookie: " + strCookie + "\r\n");}
        strHttpReqPkg.append("\r\n");
    }
}

void COptHttpData::ConstructHttpRspPkt(string &strHttpRspPkg,const string &strBody,MMapHttpHead &mmapHttpHead,bool bUseGZip)
{
    strHttpRspPkg.clear();
    strHttpRspPkg.append("HTTP/1.1 200 OK\r\n");
    strHttpRspPkg.append("Content-Type: text/plain; charset=utf-8\r\n");
    strHttpRspPkg.append("Connection: close\r\n");

    //带上其他head信息
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

    if(bUseGZip)
    {
        unsigned long ldBufLen = strBody.length() * 2;
        char *pGzipBuf = new char[ldBufLen];
        int iRet =COptHttpData::EncodeHttpGZip((Bytef*)(strBody.c_str()),strBody.length(), (Bytef*)pGzipBuf, &ldBufLen);
        if(iRet != 0)
        {
            strHttpRspPkg.append("Content-Length: " + COptHttpData::Num2Str(strBody.length()) + "\r\n");
            strHttpRspPkg.append("\r\n");
            strHttpRspPkg.append(strBody);
        }
        else
        {
            string strEncoded = "";
            strEncoded.assign((char*)pGzipBuf, ldBufLen);

            strHttpRspPkg.append("Content-Encoding: gzip\r\n");
            strHttpRspPkg.append("Content-Length: " + COptHttpData::Num2Str(strEncoded.length()) + "\r\n");
            strHttpRspPkg.append("\r\n");
            strHttpRspPkg.append(strEncoded);
        }

        if(pGzipBuf != NULL)
        {
            delete pGzipBuf;
            pGzipBuf = NULL;
        }
    }
    else
    {
        strHttpRspPkg.append("Content-Length: " + COptHttpData::Num2Str(strBody.length()) + "\r\n");
        strHttpRspPkg.append("\r\n");
        strHttpRspPkg.append(strBody);
    }

    return;
}

void COptHttpData::ConstructHttpRspPkt(string &strHttpRspPkg,const string &strBody)
{
    strHttpRspPkg.clear();
    strHttpRspPkg.append("HTTP/1.1 200 OK\r\n");
    strHttpRspPkg.append("Content-Type: text/plain; charset=utf-8\r\n");
    strHttpRspPkg.append("Connection: close\r\n");
    strHttpRspPkg.append("Content-Length: " + COptHttpData::Num2Str(strBody.length()) + "\r\n");
    strHttpRspPkg.append("\r\n");
    strHttpRspPkg.append(strBody);

    return;
}

int COptHttpData::CheckHttpPkt(void *pBuf,int iBufLen)
{
    char *pBufSrcCopy = (char*)pBuf;

    //获取Transfer-Encoding头，判断是否是chunked模式
    if( "chunked" == COptHttpData::GetHeadFromBuf(pBufSrcCopy,"Transfer-Encoding") ) //是chunked包，则校验chunked包的完整性
    {
        if(iBufLen < 5)
            return -600;

        const char *pEnd = pBufSrcCopy + iBufLen - 5;

        if(0 == strncmp(pEnd,"0\r\n\r\n",5))
            return iBufLen;

        return 0; //包不全，继续收数据
    }

    //校验非chunked包的完整性
    const char *pBody = strstr(pBufSrcCopy, "\r\n\r\n");//查找http头结尾
    if(pBody == NULL)
    {
        return 0; //未找到，则认为包不全，继续收数据
    }
    int iHeadLen = pBody + 4 - pBufSrcCopy; //找到了，计算http包的head长度

    //获取Content-Length头，
    string strHttpContLen = COptHttpData::GetHeadFromBuf(pBufSrcCopy,"Content-Length");
    if(strHttpContLen.empty())//没有Content-Length时(get请求不会带Content-Length)
    {
        return iHeadLen; //没有Content-Length时,包长即为http的head长度
    }
    int iContentLen = atoi(strHttpContLen.c_str());

    if(iHeadLen + iContentLen > iBufLen)
    {
        return 0; //包不全，继续收数据
    }

    return iHeadLen + iContentLen;
}

int COptHttpData::DecodeHttpPkt(const char *pBuf,int iLength,StHttpData &stHttpData)
{
    if(NULL == pBuf || iLength <= 0)
    {
        return -610;
    }

    //判断消息包头结束符
    if(strstr(pBuf, "\r\n\r\n") == NULL)
    {
        return -610; //http数据格式异常
    }

    //解析起始行，可以是http请求包请求首行，也可以是http响应包的首行
    //http请求包，目前只支持GET和POST
    //http响应包，以http版本号开头的
    if(0 == strncasecmp(pBuf, "GET " ,4))
    {
        stHttpData.eHttpType = HTTP_GET;
    }
    else if(0 == strncasecmp(pBuf, "POST " ,5))
    {
        stHttpData.eHttpType = HTTP_POST;
    }
    else if(0 == strncasecmp(pBuf, "HTTP/", 5))
    {
        stHttpData.eHttpType = HTTP_RSP;
    }
    else
    {
        return -611; //不支持其他类型的解析
    }

    //开始解包
    const char *pSrcCopy = pBuf;   //记录下原始的起始地址
    const char **ppBuf   = &pBuf;  //后面会移动ppBuf指向的地址，pBuf的值也随之改变

    if(HTTP_RSP == stHttpData.eHttpType)//解析响应包起始行
    {
        //响应包首行： HTTP/1.1 200 OK
        string strLine = COptHttpData::GetLineCRLF(ppBuf);
        string::size_type uiPos0 = strLine.find(' ');
        if(string::npos != uiPos0)
        {
            stHttpData.strVersion = strLine.substr(0,uiPos0);

            string strLeft = COptHttpData::Trim(strLine.substr(uiPos0));
            string::size_type uiPos1 = strLeft.find(' ');
            if(string::npos != uiPos1)
            {
                stHttpData.strRspStatus = strLeft.substr(0, uiPos0);
                stHttpData.strRspDesc   = COptHttpData::Trim(strLeft.substr(uiPos1 + 1));
            }
            else
            {
                stHttpData.strRspStatus = strLeft;
                stHttpData.strRspDesc   = "";
            }
        }
        else
        {
            stHttpData.strVersion   = strLine;
            stHttpData.strRspStatus = "0";
            stHttpData.strRspDesc   = "";
        }
    }
    else //解析请求包起始行
    {
        //请求包首行：GET /individual_data?openid=&newsid=&cur_pos=3&callback=_jsonp HTTP/1.1
        string strLine = COptHttpData::GetLineCRLF(ppBuf);
        string::size_type uiPos0 = strLine.find(" ");
        if(string::npos == uiPos0)
        {
            return -610;
        }

        string::size_type uiPos1 = strLine.rfind(" ");
        if(uiPos1 == string::npos || uiPos1 <= uiPos0)
        {
            return -610; //http数据格式异常
        }

        string strMethod      = COptHttpData::Trim(strLine.substr(0, uiPos0));
        stHttpData.strReqUrl  = COptHttpData::Trim(strLine.substr(uiPos0+1, uiPos1-uiPos0));
        stHttpData.strVersion = COptHttpData::Trim(strLine.substr(uiPos1+1));

        size_t uiPos = stHttpData.strReqUrl.find('?');
        if(string::npos == uiPos) //没有"?"的，
        {
            stHttpData.strReqCgi = stHttpData.strReqUrl;
        }
        else
        {
            stHttpData.strReqCgi = stHttpData.strReqUrl.substr(0,uiPos);
            if(HTTP_GET == stHttpData.eHttpType) //get请求包参数写入strbody中
                stHttpData.strBody = stHttpData.strReqUrl.substr(uiPos+1);
        }
    }

    //获取http头数据
    while(true)
    {
        string strLine = COptHttpData::GetLineCRLF(ppBuf);
        if(strLine.empty())
        {
            break;
        }

        string::size_type uiIdx = strLine.find(":");
        if(uiIdx != string::npos)
        {
            stHttpData.mmapHttpHead.insert(MMapHttpHead::value_type(COptHttpData::Trim(strLine.substr(0,  uiIdx), " ")
                                                 ,COptHttpData::Trim(strLine.substr(uiIdx + 1), " ")));
        }

        if( (int64_t)(*ppBuf - pSrcCopy) >= (int64_t)iLength )
        {
            break;
        }
    }

    int iHeadLen = *ppBuf - pSrcCopy;

    //获取http数据
    if("chunked" == stHttpData.GetHeader("Transfer-Encoding"))
    {
        //获取chunked模式下的数据
        string strTmp(*ppBuf, iLength-iHeadLen);
        while(true)
        {
            string::size_type uiPos = strTmp.find("\r\n");
            if(uiPos == string::npos)
                return -612; //chunked数据异常

            //查找当前chunk的大小
            string strChunkSize = strTmp.substr(0, uiPos);
            int iChunkSize      = strtol(strChunkSize.c_str(), NULL, 16);

            if(iChunkSize <= 0)  //所有chunk都接收完毕
                break;

            if(strTmp.length() >= uiPos + 2 + (size_t)iChunkSize + 2)   //接收到一个完整的chunk了
            {
                stHttpData.strBody += strTmp.substr(uiPos + 2, iChunkSize); //获取一个chunk的内容

                strTmp = strTmp.substr(uiPos + 2 + iChunkSize + 2); //删除一个chunk
            }
            else
            {
                //没有接收完整的chunk
                return -612; //chunked数据异常
            }
        }
    }
    else
    {
        if(iLength-iHeadLen > 0) //有数据才写入
            stHttpData.strBody.assign(*ppBuf, iLength-iHeadLen);
    }

    // printf("HeadLen[%d] pSrc[%x] pHead[%x] bodyLen[%d] strbodyLen[%u]\n", iHeadLen,pBuf,*ppBuf,iLength-iHeadLen,stHttpData.strBody.size());

    //gzip压缩的，做解压操作
    if("gzip" == stHttpData.GetHeader("Content-Encoding"))
    {
        uLong ulBodyLen = 2000000;
        char* LocalZipBuf = new char[ulBodyLen];//申请2M内存 用于存储gzip解压后的数据
        int iRet = DecodeHttpGZip(reinterpret_cast<unsigned char*>(const_cast<char*>(stHttpData.strBody.c_str()))
                        ,static_cast<uLong>(stHttpData.strBody.length()),reinterpret_cast<unsigned char*>(LocalZipBuf),&ulBodyLen);
        if( 0 == iRet)
        {
            stHttpData.strBody.assign(LocalZipBuf,ulBodyLen);
            delete[] LocalZipBuf;//释放内存
        }
        else
        {
            delete[] LocalZipBuf;//释放内存
            return iRet; //gzip解压失败
        }
    }

    // stHttpData.SetHeader("Content-Length", COptHttpData::Num2Str(stHttpData.strBody.length()));

    return 0;
}

void COptHttpData::GetHttpHeadFromStrPkt(const string &strHttpPkt,MMapHttpHead &mmapHttpHead)
{
    size_t uiHttpPktIdx = 0; //从0位置开始遍历
    while(true)
    {
        string strLine = COptHttpData::GetLineCRLF(strHttpPkt,uiHttpPktIdx);
        if(strLine.empty())
        {
            break;
        }

        string::size_type uiIdx = strLine.find(":");
        if(uiIdx != string::npos)
        {
            mmapHttpHead.insert(MMapHttpHead::value_type(COptHttpData::Trim(strLine.substr(0,  uiIdx))
                                                 ,COptHttpData::Trim(strLine.substr(uiIdx + 1))));
        }

        if(uiHttpPktIdx >= strHttpPkt.size())
        {
            break;
        }
    }

    return ;
}

void COptHttpData::GetMapCookieFromStr(map<string, string> &mapCookie, const string &strCookie)
{
    if (strCookie.empty())
        return;

    vector<string> vecKeyValue;
    COptHttpData::SplitToVec(strCookie, ";", vecKeyValue);

    for (size_t uiIdx = 0; uiIdx < vecKeyValue.size(); ++uiIdx)
    {
        string &strKeyValue = vecKeyValue[uiIdx];
        size_t uiPos = strKeyValue.find("=");
        if (string::npos != uiPos)
        {
            mapCookie[COptHttpData::Trim(strKeyValue.substr(0, uiPos))] = COptHttpData::Trim(strKeyValue.substr(uiPos + 1));
        }
    }

    return;
}

int COptHttpData::DecodeHttpGZip(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0};
    static char dummy_head[2] =
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    if(inflateInit2(&d_stream, 47) != Z_OK)
        return -613;//gzip解压失败
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata)
     {
        d_stream.avail_in = d_stream.avail_out = 1;
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END)
            break;
        if(err != Z_OK )
        {
            if(err == Z_DATA_ERROR)
            {
                d_stream.next_in = (Byte*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
                    return -614;//gzip解压失败
            }
            else
                return -615;//gzip解压失败
        }
    }
    if(inflateEnd(&d_stream) != Z_OK)
        return -616;//gzip解压失败

    *ndata = d_stream.total_out;

    return 0;
}

int COptHttpData::EncodeHttpGZip(Bytef *data, uLong ndata,Bytef *zdata, uLong *nzdata)
{
    z_stream c_stream;
    int err = 0;

    if(data && ndata > 0) {
        c_stream.zalloc = NULL;
        c_stream.zfree = NULL;
        c_stream.opaque = NULL;
        //只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
        if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                        MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
        c_stream.next_in  = data;
        c_stream.avail_in  = ndata;
        c_stream.next_out = zdata;
        c_stream.avail_out  = *nzdata;
        while(c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
            if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
        }
        if(c_stream.avail_in != 0) return c_stream.avail_in;
        for(;;) {
            if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
            if(err != Z_OK) return -1;
        }
        if(deflateEnd(&c_stream) != Z_OK) return -1;
        *nzdata = c_stream.total_out;
        return 0;
    }
    return -1;
}

//此函数用于从http包头中查询具体的某个包头(而不是把整个http包头都解析到MMapHttpHead中再来查询)
string COptHttpData::GetHeadFromBuf(const char *pBuf,string strHeadName)
{
    if(NULL == pBuf)
        return "";

    strHeadName += ":"; //冒号后的空格是可选的

    const char *pBegin = NULL;
    if((pBegin = strstr(pBuf, strHeadName.c_str())) == NULL)
    {
        return "";
    }

    pBegin += strHeadName.size();
    const char *pEnd = strstr(pBegin, "\r\n");
    if(pEnd != NULL)
    {
        string strTmep(pBegin, pEnd-pBegin);
        return COptHttpData::Trim(strTmep," ");
    }

    return "";
}

//新增函数，还没有验证，验证后删除这里的注释 TODO
string COptHttpData::GetCgiFromBuf(const char *pBuf)
{
    if(NULL == pBuf)
        return "";

    string strLine;
    strLine.reserve(512);
    while((*pBuf) != '\r' && (*pBuf) != '\n' && (*pBuf) != '\0')
    {
        strLine.append(1, (*pBuf));
        pBuf++;
    }
    if(strLine.empty())
        return "";

    string::size_type uiPos0 = strLine.find(" ");
    if(string::npos == uiPos0)
    {
        return ""; //http数据格式异常
    }
    string::size_type uiPos1 = strLine.rfind(" ");
    if(uiPos1 == string::npos || uiPos1 <= uiPos0)
    {
        return ""; //http数据格式异常
    }

    string strReqCgi;
    string strReqUrl = COptHttpData::Trim(strLine.substr(uiPos0 + 1, uiPos1 - uiPos0));
    size_t uiPos = strReqUrl.find('?');
    if(string::npos == uiPos) //没有"?"的，
        strReqCgi = strReqUrl;
    else
        strReqCgi = strReqUrl.substr(0,uiPos);

    return strReqCgi;
}

string COptHttpData::GetLineCRLF(const char** ppBuf)
{
    string strTmp;
    strTmp.reserve(512);

    while((**ppBuf) != '\r' && (**ppBuf) != '\n' && (**ppBuf) != '\0')
    {
        strTmp.append(1, (**ppBuf));
        (*ppBuf)++;
    }

    if((**ppBuf) == '\r')
    {
        (*ppBuf)++;   /* pass the char '\n' */
    }

    (*ppBuf)++;

    return strTmp;
}

string COptHttpData::GetLineCRLF(const string &strHttpPkt,size_t &uiIdx)
{
    if(strHttpPkt.empty() || uiIdx >= strHttpPkt.size())
        return "";

    string strTmp;
    strTmp.reserve(512);

    while(strHttpPkt[uiIdx] != '\r' && strHttpPkt[uiIdx] != '\n' && uiIdx < strHttpPkt.size())
    {
        strTmp.append(1, strHttpPkt[uiIdx]);
        uiIdx++;
    }

    if(strHttpPkt[uiIdx] == '\r')
    {
        uiIdx++;   /* pass the char '\n' */
    }

    uiIdx++;

    return strTmp;
}

string COptHttpData::Trim(const string &sStr, const string &s, bool bChar)
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
        return TrimRight(TrimLeft(sStr, s, false), s, false);
    }

    return TrimRight(TrimLeft(sStr, s, true), s, true);
}

string COptHttpData::TrimLeft(const string &sStr, const string &s, bool bChar)
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

string COptHttpData::TrimRight(const string &sStr, const string &s, bool bChar)
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
