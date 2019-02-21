/*
* Filename: VLog.h
*
* Created on   : 2015-02-14 20:57
*     Author: carinwang   carinwang@tencent.com
* Copyright(c) 2015 Tencent. All rights reserved.
*
* Last modified: 2015-02-26 21:00
*
* Description:
*
*/

#ifndef VLOG_H_
#define VLOG_H_

#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <time.h>

namespace VLOG
{

#define VLOG_FATAL	0
#define VLOG_ERROR	1
#define VLOG_WARN	2
#define VLOG_INFO	3
#define VLOG_DEBUG	4
#define VLOG_ALL	5

using std::string;

//д�ļ�������
class VLog
{
public:
	VLog();
	virtual ~VLog();
	//�ļ��������Ϊ200
	void setFileName(string &fileName);
	void setFileName(const char *fileName);
	void setLevel(int level);
	void setMaxSize(unsigned int maxSize);
	void setMaxNum(int maxNum);
	void setByDate(int isByDate);
	void setStdOut(int isStdOut);
	void setByLevel(int isByLevel);
	void setLogTime(int isLogTime);
	void setCheckFileTime(int checkFileTime);

	void vlog(const char *srcfile_name,int line,int level,const char * csformat,...);

protected:
	void log(const char *srcfile_name,int line,int level,const char * csformat,va_list &ap);
	virtual void filelog(const char *srcfile_name,int line,int level,const char * filename,const char * csformat,va_list &ap);
	void shiftfiles(const char *filename);
	string getLevelStr(int level);

	string fileName;
	int level;	//��־��¼�ȼ�
	unsigned int maxSize;	//ÿ����־�ļ�����С,-1Ϊ������,��СΪ1024
	int maxNum;	//�����־�ļ�����,-1Ϊ������,��СΪ2
	int isByDate;	//�Ƿ���д��־
	int isStdOut;	//�Ƿ��׼���
	int isByLevel;	//�Ƿ񰴵ȼ����ļ�
	int isLogTime;	//�Ƿ��¼��־ʱ��
	int checkFileTime;	//ѭ���鿴�ļ���С����,Ĭ��1000,�û��鿴�ļ��Ƿ񳬹�������
	int logTimes;
private:
};

//д�ļ�����
class VSafeLog:public VLog
{
public:
	virtual ~VSafeLog();
protected:
	virtual void filelog(int level,const char * filename,const char * csformat,va_list &ap);
};

extern VLog 	 g_vlog;
extern VSafeLog  g_vsafelog;
#define V_LOG(level,...) 	  g_vlog.vlog(__FILE__,__LINE__,level,__VA_ARGS__)
#define V_SAFE_LOG(level,...) g_vsafelog.vlog(__FILE__,__LINE__,level,__VA_ARGS__)

}

#endif /*VLOG_H_*/
