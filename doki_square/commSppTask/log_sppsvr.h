/*
 *  Filename: log_sppsvr.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: spp微线程服务使用的日志宏定义,spp服务的时候使用这个
 *
 */

#ifndef __LOG_SPPSVR_H__
#define __LOG_SPPSVR_H__

// ====spp日志 =============================================================

/**
 * spp中日志级别
 * enum LOG_LEVEL
 * {
 *     LOG_TRACE = 0,
 *     LOG_DEBUG,
 *     LOG_NORMAL,
 *     LOG_ERROR,
 *     LOG_FATAL,
 *     LOG_NONE    //当要禁止写任何日志的时候,可以设置tlog的日志级别为LOG_NONE
 * };
 */

extern CServerBase *g_pCServerBase; //仅仅只是为了task打印日志的

//====for-spp框架通用日志======================
#define SPP_FATAL_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_FATAL) \
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_FATAL,fmt"\n",##__VA_ARGS__);\
    }\
}while(0)

#define SPP_ERROR_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_ERROR)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_ERROR,fmt"\n",##__VA_ARGS__);\
    }\
}while(0)

#define SPP_NORMAL_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_NORMAL)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_NORMAL,fmt"\n",##__VA_ARGS__);\
    }\
}while(0)

#define SPP_DEBUG_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_DEBUG)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_DEBUG,fmt"\n",##__VA_ARGS__);\
    }\
}while(0)

#define SPP_TRACE_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_TRACE)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_TRACE,fmt"\n",##__VA_ARGS__);\
    }\
}while(0)

//====for-msg打印日志=====================================
#define SPP_MSG_FATAL_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_FATAL)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_FATAL,"cmd[0x%x]reqKey[%s]"fmt"\n" \
            ,m_uiCmd,m_strReqKey.c_str(),##__VA_ARGS__);\
    }\
}while(0)

#define SPP_MSG_ERROR_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_ERROR)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_ERROR,"cmd[0x%x]reqKey[%s]"fmt"\n" \
            ,m_uiCmd,m_strReqKey.c_str(),##__VA_ARGS__);\
    }\
}while(0)

#define SPP_MSG_NORMAL_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_NORMAL)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_NORMAL,"cmd[0x%x]reqKey[%s]"fmt"\n" \
            ,m_uiCmd,m_strReqKey.c_str(),##__VA_ARGS__);\
    }\
}while(0)

#define SPP_MSG_DEBUG_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_DEBUG)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_DEBUG,"cmd[0x%x]reqKey[%s]"fmt"\n" \
            ,m_uiCmd,m_strReqKey.c_str(),##__VA_ARGS__);\
    }\
}while(0)

#define SPP_MSG_TRACE_LOG(fmt,...) \
do{\
    if(NULL != g_pCServerBase && g_pCServerBase->log_.log_level(-1) <= (int)tbase::tlog::LOG_TRACE)\
    {\
        g_pCServerBase->log_.LOG_P_ALL(tbase::tlog::LOG_TRACE,"cmd[0x%x]reqKey[%s]"fmt"\n" \
            ,m_uiCmd,m_strReqKey.c_str(),##__VA_ARGS__);\
    }\
}while(0)

//=======================================================================================

#endif	// __LOG_SPPSVR_H__
