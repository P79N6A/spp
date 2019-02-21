/*
 *  Filename: log_mtalone.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 单独微线程的脚本程序使用的日志宏定义，一般是一些脚本或者定时任务的程序
 *
 */

#ifndef __LOG_MTALONE_H__
#define __LOG_MTALONE_H__

#include "VLog.h"
using namespace VLOG;

//====Vlog日志 ==============================================================
// 日志级别说明
// VLOG_FATAL   0
// VLOG_ERROR   1
// VLOG_WARN    2
// VLOG_INFO    3
// VLOG_DEBUG   4
// VLOG_ALL     5

//for msg打印日志
#define SPP_FATAL_LOG(fmt,...) \
do{\
    V_LOG(VLOG_FATAL,fmt,##__VA_ARGS__);\
}while(0)

#define SPP_ERROR_LOG(fmt,...) \
do{\
    V_LOG(VLOG_ERROR,fmt,##__VA_ARGS__);\
}while(0)

#define SPP_DEBUG_LOG(fmt,...) \
do{\
    V_LOG(VLOG_DEBUG,fmt,##__VA_ARGS__);\
}while(0)

#define SPP_TRACE_LOG(fmt,...) \
do{\
    V_LOG(VLOG_ALL,fmt,##__VA_ARGS__);\
}while(0)

//for msg打印日志
#define SPP_MSG_FATAL_LOG(fmt,...) \
do{\
    V_LOG(VLOG_FATAL,"reqkey[%s]"fmt,m_strReqKey.c_str(),##__VA_ARGS__);\
}while(0)

#define SPP_MSG_ERROR_LOG(fmt,...) \
do{\
    V_LOG(VLOG_ERROR,"reqkey[%s]"fmt,m_strReqKey.c_str(),##__VA_ARGS__);\
}while(0)

#define SPP_MSG_DEBUG_LOG(fmt,...) \
do{\
    V_LOG(VLOG_DEBUG,"reqkey[%s]"fmt,m_strReqKey.c_str(),##__VA_ARGS__);\
}while(0)

#define SPP_MSG_TRACE_LOG(fmt,...) \
do{\
    V_LOG(VLOG_ALL,"reqkey[%s]"fmt,m_strReqKey.c_str(),##__VA_ARGS__);\
}while(0)

//=======================================================================================

#endif	// __LOG_MTALONE_H__
