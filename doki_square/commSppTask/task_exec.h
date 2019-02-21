/*
 *  Filename: task_exec.h
 *
 *  Created on: 2017-07-19
 *      Author: carinwang   carinwang@tencent.com
 *  Copyright(c) 2017 Tencent. All rights reserved.
 *
 *  Description: 运行task的基类
 *
 */

#ifndef __TASK_EXEC_H__
#define __TASK_EXEC_H__

#include "task_base.h"

//传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源
class CTaskExec
{
  public:
    CTaskExec() {};

    ~CTaskExec()
    {
       ClearAllTask();//回收资源
    };

    void ClearAllTask()
    {
        //回收资源
        for(size_t uiIdx = 0; uiIdx < m_taskList.size(); ++uiIdx)
        {
            IMtTask *pTask = m_taskList[uiIdx];
            if(NULL != pTask)
            {
                delete pTask;
                pTask = NULL;
            }
        }

        m_taskList.clear();
    }

    void PushBack(CTaskBase *pTask) //传递给CTaskExec的task指针必须为new出来的对象，CTaskExec的析构函数或ClearAllTask函数负责delete回收资源
    {
        m_taskList.push_back(pTask);
    }

    const vector<IMtTask*>& GetVecPtrTask()
    {
        return m_taskList;
    }

    int RunTaskList()
    {
        if(!m_taskList.empty())
        {
            int iRet = mt_exec_all_task(m_taskList);
            if (iRet < 0)
            {
                Attr_API(MONI_SPP_TASKEXEC_ERR,1);
                SPP_ERROR_LOG("execAllTask: failed, iRet[%d]", iRet);
                return -1;
            }
        }

        return 0;
    }

  private:
    vector<IMtTask*> m_taskList;
};

#endif	// __TASK_EXEC_H__
