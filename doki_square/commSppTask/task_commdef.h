/*
 *  Filename: task_commdef.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: task相关枚举、结构、宏定义
 *
 */

#ifndef __TASK_COMMDEF_H__
#define __TASK_COMMDEF_H__

#include <string>
using namespace std;

// ====框架相关的枚举=========================================================================

//spp3.0.1版本之后tcp请求支持自动扩展接受buf了，接受buf默认最小为4K且会自动修正为4K的整数倍
//udp请求的接受buf还是需要自己来分配
enum ENUM_TASK_BUFLEN
{
    BUF_LEN_1K    = 1024*1,
    BUF_LEN_2K    = 1024*2,
    BUF_LEN_4K    = 1024*4,
    BUF_LEN_8K    = 1024*8,
    BUF_LEN_12K   = 1024*12,
    BUF_LEN_16K   = 1024*16,
    BUF_LEN_20K   = 1024*20,
    BUF_LEN_24K   = 1024*24,
    BUF_LEN_28K   = 1024*28,
    BUF_LEN_32K   = 1024*32,
    BUF_LEN_36K   = 1024*36,
    BUF_LEN_40K   = 1024*40,
    BUF_LEN_48K   = 1024*48,
    BUF_LEN_64K   = 1024*64,
    BUF_LEN_72K   = 1024*72,
    BUF_LEN_80K   = 1024*80,
    BUF_LEN_96K   = 1024*96,
    BUF_LEN_128K  = 1024*128,
    BUF_LEN_256K  = 1024*256,
    BUF_LEN_512K  = 1024*512,
    BUF_LEN_1024K = 1024*1024,
    BUF_LEN_2048K = 2*1024*1024,
};

//task使用的请求方式:
enum ENUM_CONN
{
    EM_CONN_UNSET = 0, //未设置
    EM_CONN_TCP   = 1, //TCP
    EM_CONN_UDP   = 2, //UDP
};

//task使用的获取地址的方式:
enum ENUM_GETADDR
{
    EM_GETADDR_UNSET  = 0, //未设置
    EM_GETADDR_IPPORT = 1, //指定ip:port
    EM_GETADDR_NAME   = 2, //名字服务
    EM_GETADDR_L5     = 3, //L5
};

//task常用错误码
enum ENUM_TASK_OPTERR
{
    ERR_TASK_PARAM      = -300, //缺少必要数据
    ERR_TASK_GETADDR    = -301, //获取地址失败
    ERR_TASK_MALLOCBUF  = -302, //分配内存失败
    ERR_TASK_ENCODE     = -303, //请求包打包失败
    ERR_TASK_DECODE     = -304, //回包解包失败
    ERR_TASK_SEND_EMPTY = -305, //发送数据为空
    ERR_TASK_OPT_LOGIC  = -306, //task逻辑操作异常(请求数据为空、错误的函数调用等)
};

//spp微线程错误码: mt_tcpsendrcv函数返回的错误码(以网络操作的先后次序排序)
enum ENUM_SPPMT_TCP_OPTERR
{
    ERR_SPPMT_TCP_PARAM_INVALID       = -10,  //参数无效(目标地址指针为空，sendbuf指针为空，sendbuf长度小于1)
    ERR_SPPMT_TCP_GET_KEEP_CONN       = -1,   //TCP连接池获取fd失败
    ERR_SPPMT_TCP_CONNECT             = -4,   //TCP连接失败
    ERR_SPPMT_TCP_SENDDATA            = -2,   //发送数据失败
    ERR_SPPMT_TCP_MALOC_RECVBUF_INIT  = -11,  //接受buf内存分配失败
    ERR_SPPMT_TCP_TIMEOUT_OR_ERCVFAIL = -3,   //tcp超时或接受失败
    ERR_SPPMT_TCP_REMOTE_CLOSE        = -7,   //对端关闭连接
    ERR_SPPMT_TCP_CHECK_PKG           = -5,   //tcp包校验失败
    ERR_SPPMT_TCP_RECVBUF_NOTENOUGH   = -6,   //接受数据buf不足(spp3.0.1之后的版本不在使用了)
};

//spp微线程错误码: mt_udpsendrcv函数返回的错误码(以网络操作的先后次序排序)
enum ENUM_SPPMT_UDP_OPTERR
{
    ERR_SPPMT_UDP_PARAM_INVALID = -10, //参数无效
    ERR_SPPMT_UDP_SOCKET        = -1,  //socket失败
    ERR_SPPMT_UDP_SENDTO        = -2,  //发送失败
    ERR_SPPMT_UDP_RECVFROM      = -3,  //接收应答失败
};

// ====框架相关的结构定义=========================================================================
struct StSvrConnInfo //服务连接信息
{
    //请求服务的连接信息
    ENUM_CONN     eConnType;    //连接方式：UDP、TCP、UNSET
    ENUM_GETADDR  eGetAddrType; //获取地址的方式：L5、Zkname、IpPort、Unset
    int           iTimeOut_ms;
    int           iModId;
    int           iCmdId;
    string        strName;
    string        strIP;        //直接使用ip
    int           iPort;        //直接使用port

    StSvrConnInfo()
        :eConnType(EM_CONN_UNSET)
        ,eGetAddrType(EM_GETADDR_UNSET)
        ,iTimeOut_ms(0)
        ,iModId(0)
        ,iCmdId(0)
        ,iPort(0)
    {
    };
};

struct StMcallInfo   //模调监控上报信息
{
    bool bMcall;     //是否需要进行模调上报

    int  iInitModId; //模调--主调模块
    int  iInitIface; //模调--主调接口
    int  iPassModId; //模调--被调模块
    int  iPassIface; //模调--被调接口

    StMcallInfo()
        :bMcall(false)
        ,iInitModId(0)
        ,iInitIface(0)
        ,iPassModId(0)
        ,iPassIface(0)
    {
    };
};

struct StMoniInfo    //monitor监控上报信息
{
    bool bMoni;

    int  iMoniAll;       //monitor上报属性--总数
    int  iMoniFail;      //monitor上报属性--失败数
    int  iMoniRetErr;    //monitor上报属性--返回错误数
    int  iMoniSucc;      //monitor上报属性--成功数
    int  iMoniEncodeErr; //monitor上报属性--打包失败
    int  iMoniDecodeErr; //monitor上报属性--解包失败

    StMoniInfo()
        :bMoni(false)
        ,iMoniAll(0)
        ,iMoniFail(0)
        ,iMoniRetErr(0)
        ,iMoniSucc(0)
        ,iMoniEncodeErr(0)
        ,iMoniDecodeErr(0)
    {
    };
};

//==============================================================================================

 #endif	// __TASK_COMMDEF_H__
