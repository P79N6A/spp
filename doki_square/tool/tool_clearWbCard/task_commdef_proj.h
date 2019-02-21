/*
 *  Filename: task_commdef_proj.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 * 	Description: 项目自定义的枚举或宏定义，用于项目和task框架交互定义
 *
 */

#ifndef __TASK_COMMDEF_PROJ_H__
#define __TASK_COMMDEF_PROJ_H__

#include "mt_incl.h"     //spp微线程需要包含的头文件
#include "log_mtalone.h" //spp微线程服务的日志宏定义
#include <sstream>

//====常量定义===============================================================
//SPP框架异常的上报，monitor属性值固定下来，各个项目同用
#define MONI_SPP_TASKEXEC_ERR  487412 //task运行失败

//union的appid和appkey定义;不同的服务申请不同的appid
#define TASK_UNION_APPID    ""
#define TASK_UNION_APPKEY   ""

// ====task类型定义====
enum ENUM_TASK_TYPE
{
    TASK_UNDEF           = 0,
    TASK_REDIS_STAR_CARD = 2,
    TASK_REDIS_STAR_IDX  = 3,
    TASK_REDIS_FEED_INFO = 4,

};

static string GetTaskName(int iTaskType)
{
    switch(iTaskType)
    {
        case TASK_UNDEF          : return "TASK_UNDEF";
        case TASK_REDIS_STAR_CARD: return "TASK_REDIS_STAR_CARD";
        case TASK_REDIS_STAR_IDX : return "TASK_REDIS_STAR_IDX";
        case TASK_REDIS_FEED_INFO: return "TASK_REDIS_FEED_INFO";
    }

    stringstream ssTemp;
    ssTemp<<iTaskType;
    return ssTemp.str();
};


#endif  /*__TASK_COMMDEF_PROJ_H__*/
