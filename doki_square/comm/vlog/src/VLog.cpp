/*
* Filename: VLog.cpp
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

#include "VLog.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

namespace VLOG
{

using namespace std;

VLog 	 g_vlog;
VSafeLog g_vsafelog;

VLog::VLog()
{
	fileName = "log";
	level = VLOG_DEBUG;
	maxSize = 1024*1024;
	maxNum = 10;
	isByDate = 0;
	isStdOut = 0;
	isByLevel = 0;
	isLogTime = 1;
	logTimes = 0;
	checkFileTime = 1000;
}
VLog::~VLog()
{

}

VSafeLog::~VSafeLog()
{

}

void VLog::setFileName(const char *fileName)
{
	char tmp[200];
	snprintf(tmp,sizeof(tmp)-1,"%s",fileName);
	this->fileName = tmp;
}

void VLog::setFileName(string &fileName)
{
	char tmp[200];
	snprintf(tmp,sizeof(tmp)-1,"%s",fileName.c_str());
	this->fileName = tmp;
}

void VLog::setLevel(int level)
{
	if(level<VLOG_FATAL || level>VLOG_ALL)
	{
		return;
	}
	this->level = level;
}

void VLog::setMaxSize(unsigned int maxSize)
{
	if(maxSize<1024)
	{
		this->maxSize = 1024;
	}
	else
	{
		this->maxSize = maxSize;
	}
}

void VLog::setMaxNum(int maxNum)
{
	if(maxNum<2 && maxNum>=0)
	{
		this->maxNum = 2;
	}
	else
	{
		this->maxNum = maxNum;
	}
}

void VLog::setByDate(int isByDate)
{
	this->isByDate = isByDate;
}

void VLog::setStdOut(int isStdOut)
{
	this->isStdOut = isStdOut;
}

void VLog::setByLevel(int isByLevel)
{
	this->isByLevel = isByLevel;
}

void VLog::setLogTime(int isLogTime)
{
	this->isLogTime = isLogTime;
}

void VLog::setCheckFileTime(int checkFileTime)
{
	if(checkFileTime<=0)
	{
		return;
	}
	this->checkFileTime = checkFileTime;
}

void VLog::vlog(const char *srcfile_name,int line,int level,const char * csformat,...)
{
	va_list ap;
	va_start(ap, csformat);
	log(srcfile_name,line,level,csformat,ap);
	va_end(ap);
}

void VLog::log(const char *srcfile_name,int line,int level,const char * csformat,va_list &ap)
{
	if(level>this->level)
	{
		return;
	}

	if(isStdOut)
	{
		vprintf(csformat, ap);
		printf("\n");
	}

	char logFile[256];
	if(isByLevel)
	{
		char tmp[7];

		if(level==VLOG_FATAL)
		{
			memcpy(tmp,"fatal",5);
			tmp[5] = '\0';
		}
		else if(level==VLOG_ERROR)
		{
			memcpy(tmp,"error",5);
			tmp[5] = '\0';
		}
		else if(level==VLOG_WARN)
		{
			memcpy(tmp,"warn",4);
			tmp[4] = '\0';
		}
		else if(level==VLOG_INFO)
		{
			memcpy(tmp,"info",4);
			tmp[4] = '\0';
		}
		else if(level==VLOG_DEBUG)
		{
			memcpy(tmp,"debug",5);
			tmp[5] = '\0';
		}
		else if(level==VLOG_ALL)
		{
			memcpy(tmp,"all",3);
			tmp[3] = '\0';
		}
		if(isByDate)
		{
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			snprintf(logFile,sizeof(logFile)-1,"%s_%s_%d_%02d_%02d.log",
				fileName.c_str(),tmp,1900+p->tm_year,1+p->tm_mon,p->tm_mday);
		}
		else
		{
			snprintf(logFile,sizeof(logFile)-1,"%s_%s.log",fileName.c_str(),tmp);
		}
	}
	else
	{
		if(isByDate)
		{
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			snprintf(logFile,sizeof(logFile)-1,"%s_%d_%02d_%02d.log",
				fileName.c_str(),1900+p->tm_year,1+p->tm_mon,p->tm_mday);
		}
		else
		{
			snprintf(logFile,sizeof(logFile)-1,"%s.log",fileName.c_str());
		}
	}

	filelog(srcfile_name,line,level,logFile,csformat,ap);
}

void VLog::filelog(const char *srcfile_name,int line,int level,const char * filename,const char * csformat,va_list &ap)
{
	char logBuf[1024*4];
	int fd = -1;
	fd = open(filename,O_RDWR|O_CREAT|O_APPEND,0644);
	if(fd<0)
	{
		fprintf(stderr,"\n file open error: file=[%s] line=[%d]  filename=[%s]\n",__FILE__,__LINE__,filename);
		return;
	}
	if(isLogTime)
	{
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep);
		//fprintf(fd,"[%02d-%02d %02d:%02d:%02d]",(1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
		int len = snprintf(logBuf,sizeof(logBuf)-1,"[%s:%d][%s][%04d-%02d-%02d %02d:%02d:%02d]"
						,srcfile_name,line,getLevelStr(level).c_str()
						,(1900+p->tm_year),(1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
		if(len>=0)
		{
			int iRet = write(fd,logBuf,len);
			if(iRet<0)
			{
				fprintf(stderr,"\n file write error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
			}
		}
	}
	//只判断>0有时会返回错误,len返回值会大于bufsize,是该函数一个bug,与函数说明不符合
	int len = vsnprintf(logBuf,sizeof(logBuf)-2,csformat,ap);
	if(len>=0 && len<(int)(sizeof(logBuf)-1))
	{
		//printf("%d size=%d\n",len,sizeof(logBuf)-2);

		logBuf[len] = '\n';
		len++;
		int iRet = write(fd,logBuf,len);
		if(iRet<0)
		{
			fprintf(stderr,"\n file write error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
		}
	}
	else
	{
		//如果len>0,则返回需要的空间大小
		//如果len=-1,则未知空间大小,与具体环境有关
		char *tmpBuf;
		int bufSize = 1024*10;

		while(true)
		{
			tmpBuf = new char[bufSize];
			if(tmpBuf==NULL)
			{
				fprintf(stderr,"\n new char[] error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
				return;
			}
			len = vsnprintf(tmpBuf,bufSize-1,csformat,ap);
			if(len>=0 && len<bufSize-1)
			{
				break;
			}
			else
			{
				bufSize *= 2;
				delete[] tmpBuf;
				continue;
			}
			break;
		}
		tmpBuf[len] = '\n';
		int iRet = write(fd,tmpBuf,len);
		if(iRet<0)
		{
			fprintf(stderr,"\n file write error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
		}

		delete [] tmpBuf;
	}

	close(fd);

	shiftfiles(filename);
}

void VLog::shiftfiles(const char *filename)
{
	logTimes++;
	//记录1000统计一次
	if(logTimes%checkFileTime==0)
	{
		struct stat stStat;
		if(stat(filename, &stStat) < 0)
		{
			fprintf(stderr,"\n file sate error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
			return;
		}

		if ((unsigned int)stStat.st_size < maxSize)
		{
			return;
		}

		char sLogFileName[256];
		if(maxNum<0)
		{
			int i=0;
			while(true)
			{
				snprintf(sLogFileName,sizeof(sLogFileName)-1,"%s_%d", filename, i);
				if (access(sLogFileName, F_OK) == 0)
				{
					i++;
					continue;
				}
				if (rename(filename,sLogFileName) < 0 )
				{
					fprintf(stderr,"\n file rename error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
					return;
				}

				break;
			}
		}
		else
		{
			snprintf(sLogFileName,sizeof(sLogFileName)-1,"%s_%d", filename, maxNum-1);
			if (access(sLogFileName, F_OK) == 0)
			{
				if (remove(sLogFileName) < 0 )
				{
					fprintf(stderr,"\n file remove error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
					return;
				}
			}
			for(int i = maxNum-2; i >= 0; i--)
			{
				char sNewLogFileName[256];
				if (i == 0)
					snprintf(sLogFileName,sizeof(sLogFileName)-1,"%s", filename);
				else
					snprintf(sLogFileName,sizeof(sLogFileName)-1,"%s_%d", filename, i);

				if (access(sLogFileName, F_OK) == 0)
				{
					snprintf(sNewLogFileName,sizeof(sNewLogFileName)-1,"%s_%d", filename, i+1);
					if (rename(sLogFileName,sNewLogFileName) < 0 )
					{
						fprintf(stderr,"\n file rename error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
						return;
					}
				}
			}
		}
	}
}

void VSafeLog::filelog(int level,const char * filename,const char * csformat,va_list &ap)
{
	char logBuf[1024*10];
	int fd = -1;
	fd = open(filename,O_RDWR|O_CREAT|O_APPEND,0644);
	if(fd<0)
	{
		fprintf(stderr,"\n file open error: file=[%s] line=[%d] filename=[%s]\n",__FILE__,__LINE__,filename);
		return;
	}
	int iRet = -1;
	iRet = flock(fd,LOCK_EX);
	if(iRet<0)
	{
		close(fd);
		fprintf(stderr,"\n file lock error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
		return;
	}

	if(isLogTime)
	{
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep);
		//fprintf(fd,"[%02d-%02d %02d:%02d:%02d]",(1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
		int len = snprintf(logBuf,sizeof(logBuf)-1,"[%d][%d][%02d-%02d %02d:%02d:%02d]",level,getpid(),(1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
		if(len>=0)
		{
			int iRet = write(fd,logBuf,len);
			if(iRet<0)
			{
				fprintf(stderr,"\n file write error: file=[%s] line=[%d]\n",__FILE__,__LINE__);
			}
		}
	}
	int len = vsnprintf(logBuf,sizeof(logBuf)-2,csformat,ap);
	if(len>=0 && len<(int)(sizeof(logBuf)-1))
	{
		logBuf[len] = '\n';
		len++;
		int iRet = write(fd,logBuf,len);
		if(iRet<0)
		{
			fprintf(stderr,"\n file write error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
		}
	}
	else
	{
		//如果len>0,则返回需要的空间大小
		//如果len=-1,则未知空间大小,与具体环境有关
		char *tmpBuf;
		int bufSize = 1024*20;

		while(true)
		{
			tmpBuf = new char[bufSize];
			if(tmpBuf==NULL)
			{
				fprintf(stderr,"\n new char[] error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
				return;
			}
			len = vsnprintf(tmpBuf,bufSize-1,csformat,ap);
			if(len>=0 && len<bufSize-1)
			{
				break;
			}
			else
			{
				bufSize *= 2;
				delete[] tmpBuf;
				continue;
			}
			break;
		}
		tmpBuf[len] = '\n';
		int iRet = write(fd,tmpBuf,len);
		if(iRet<0)
		{
			fprintf(stderr,"\n file write error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
		}

		delete [] tmpBuf;
	}

	iRet = flock(fd,LOCK_UN);
	if(iRet<0)
	{
		close(fd);
		fprintf(stderr,"\n file unlock error: file=[%s] line=[%d] \n",__FILE__,__LINE__);
		return;
	}
	close(fd);

	shiftfiles(filename);
}

string VLog::getLevelStr(int level)
{
	switch(level)
	{
		case VLOG_FATAL:
			return "FATAL";
		case VLOG_ERROR:
			return "ERROR";
		case VLOG_WARN:
			return "WARN";
		case VLOG_INFO:
			return "INFO";
		case VLOG_DEBUG:
			return "DEBUG";
		case VLOG_ALL:
			return "all";
		default:
			return "DEFAULT";
	}
}

}
