#pragma once

#pragma warning(disable : 4996)

#include "Singleton.h"


#define LOG_INFO(msg) \
	CLogger::GetInstance()->Write(CLogger::LV_INFO, msg)

#define LOG_WARN(msg) \
	CLogger::GetInstance()->Write(CLogger::LV_WARNING, msg)

#define LOG_ERR(msg) \
	CLogger::GetInstance()->Write(CLogger::LV_ERROR, msg)

#define LOGF_INFO(format, ...) \
	CLogger::GetInstance()->Writef(CLogger::LV_INFO, format, __VA_ARGS__)

#define LOGF_WARN(format, ...) \
	CLogger::GetInstance()->Writef(CLogger::LV_WARNING, format, __VA_ARGS__)

#define LOGF_ERR(format, ...) \
	CLogger::GetInstance()->Writef(CLogger::LV_ERROR, format, __VA_ARGS__)

#define SHOW_DEBUG_CONSOLE() \
	CLogger::GetInstance()->ShowConsole()


class CLogger : public CSingleton<CLogger>
{
public:
	enum LOG_LEVEL{
		LV_INFO,
		LV_WARNING,
		LV_ERROR
	};
	
	CLogger(void);
	~CLogger(void);
	void Write(LOG_LEVEL level ,const char * message);
	void Writef(LOG_LEVEL level, const char * format, ...);
	void ShowConsole();
private:
	bool isConsoleActive;
};
