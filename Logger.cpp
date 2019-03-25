#include "Logger.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <string>
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

CLogger::CLogger(void) : isConsoleActive(false)
{
}

CLogger::~CLogger(void)
{
}

void CLogger::Write(LOG_LEVEL level ,const char * message){
	const char * level_name;
	switch(level){
	case LV_INFO:
		level_name = "INFO";
		break;
	case LV_WARNING:
		level_name = "WARNING";
		break;
	case LV_ERROR:
		level_name = "ERROR";
		break;
	default:
		level_name = "INFO";
	}
	time_t now;
	time(&now);
	std::string t(ctime(&now));

	printf("[%s] %s - %s\n",  t.substr(0, t.length() -1).c_str(), level_name, message);
}
void CLogger::Writef(LOG_LEVEL level, const char * format, ...){
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf_s(buffer, format, args);
	va_end(args);

	Write(level, buffer);
}

void CLogger::ShowConsole(){
	if(isConsoleActive) return;

	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);                // return an OS file handle
	int hCrt = _open_osfhandle((long)handle,0);     // return a runtime file handle
	FILE * hf = _fdopen( hCrt, "w" );                                       // stream
	char buf[2];
	setvbuf( hf, buf, _IONBF, 1 );
	*stdout = *hf;
	SetConsoleTitleA("DEBUG");
	isConsoleActive = true;
}