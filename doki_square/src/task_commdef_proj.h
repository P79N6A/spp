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

#include "syncincl.h"   //spp微线程需要包含的头文件,里面会依赖sppincl.h
#include "log_sppsvr.h" //spp微线程服务的日志宏定义
#include <sstream>

//====常量定义===============================================================
//SPP框架异常的上报，monitor属性值固定下来，各个项目同用
#define MONI_SPP_TASKEXEC_ERR  487412 //task运行失败

//union的appid和appkey定义;不同的服务申请不同的appid
#define TASK_UNION_APPID    "20001484"
#define TASK_UNION_APPKEY   "7a618e61a5ecc1d4"

// ====task类型定义==========================================================
enum ENUM_TASK_TYPE
{
    TASK_UNDEF              = 0,

    LIVE_COMMENT = 1,
    LIVE_COMPERE = 2,
    LIVE_POLL = 3,

    UNION_GETVID_1006       = 13,
    UNION_GETVID_2001       = 14,
    UNION_GETCID            = 15,
    UNION_GETPID            = 16,
    UNION_GETSTAR           = 17,
    UNION_GETDOKI           = 18,   //union数据

    REDIS_EXPIRE            = 19,
    REDIS_SET_COUNT         = 20,
    REDIS_STR_SET           = 21,
    REDIS_SET_ADD           = 22,
    REDIS_SET_GET           = 23,
    REDIS_SSET_ADD          = 24,
    REDIS_SSET_DEL          = 25,
    REDIS_SSET_GET          = 26,
    REDIS_HASH_DEL          = 27,
    REDIS_HASH_ADD          = 28,
    REDIS_HASH_GET          = 29,

    RANK_REDIS_STR_GETBATCH = 40,
    GET_CMS_SETTING         = 41,
    GET_HOTTOPIC            = 42,
    GET_RECOMMEND           = 43,
    GET_FEED_DEAIL          = 44,    // 帖子详情信息
    GET_PHOTO_QUALITY       = 45,    // 图片质量分

    GET_CONFIG_ADMIN         = 46,     // 粉丝管理台 明星配置数据
    FANS_ADAPT              = 47,    // 勋章信息
    GET_DOKI_INFO           = 48,    // 明星基本信息 -- 譬如昵称
    GET_RIGHT_ADMIN         = 49,    // 粉丝管理台 权限数据
    GET_STAR_ADMIN          = 50,     // 粉丝管理台 
    GET_STAR_RANK           = 51,    // 人气榜
    GET_USR_INFO            = 52,    // 用户基本信息
    GET_FT_REDIS            = 53,    // 饭团粉丝数  -- redis hgetall 
    GET_FANS_ADMIN_CTT      = 54,    // 粉丝管理台 -- CONTENT模块
    GET_WELFARE_WALLPAPER   = 55,    // 壁纸
    GET_WELFARE_EMOTICON    = 56,    // 表情包
    GET_PROMOTION           = 57,    // PR管理台配置信息
};

static string GetTaskName(int iTaskType)
{
    switch(iTaskType)
    {
        case TASK_UNDEF             : return "TASK_UNDEF";
        //case UNION_GETPID           : return "UNION_GETPID";
        case UNION_GETSTAR          : return "UNION_GETSTAR";

        case REDIS_STR_SET          : return "REDIS_STR_SET";
        case REDIS_SET_ADD          : return "REDIS_SET_ADD";
        case REDIS_SET_GET          : return "REDIS_SET_GET";
        case REDIS_SSET_ADD         : return "REDIS_SSET_ADD";
        case REDIS_SSET_DEL         : return "REDIS_SSET_DEL";
        case REDIS_SSET_GET         : return "REDIS_SSET_GET";
        case REDIS_HASH_DEL         : return "REDIS_HASH_DEL";
        case REDIS_HASH_ADD         : return "REDIS_HASH_ADD";
        case REDIS_HASH_GET         : return "REDIS_HASH_GET";

        case RANK_REDIS_STR_GETBATCH: return "RANK_REDIS_STR_GETBATCH";
        case GET_CMS_SETTING        : return "GET_CMS_SETTING";
        case GET_HOTTOPIC           : return "GET_HOTTOPIC";
        case GET_RECOMMEND          : return "GET_RECOMMEND";
        case GET_FEED_DEAIL         : return "GET_FEED_DEAIL";
        case GET_PHOTO_QUALITY      : return "GET_PHOTO_QUALITY";
        case GET_CONFIG_ADMIN       : return "GET_CONFIG_ADMIN";
        default:
            break;
    }

    stringstream ssTemp;
    ssTemp<<iTaskType;
    return ssTemp.str();
};


#endif // __TASK_COMMDEF_PROJ_H__
