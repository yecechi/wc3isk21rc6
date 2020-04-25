////////////////////////////////////////////////////////////////////////////
//	_trace.h
//
//	Cee'z
//	Jan-23rd-2010
//	
//	This is use for Debuging. Instead of break the program, we use an ouput
//	window that print the results.
//	Use this for debug key press processing.
//
//	First we need to include this header #include "_trace.h"
//	Any time we need a TRACE we use
//			TRACE(...) or _trace(...) or _tracenl(...)
//	Example:
//			LPTSTR buffer;
//			buffer = (LPTSTR)GetSomeName(SomeValue);  // GetSomeName return LPSTR
//			_tracenl(buffer);
//			strcpy(buffer, "Hello world"); 
//			_tracenl(buffer);
//			_tracenl(_T("String Length = %d"),strlen());
//			
//
////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#ifdef _DEBUG
bool _trace(TCHAR *format, ...);
bool _tracenl(TCHAR *format, ...);
#define TRACE _trace
#define TRACELN _traceln
#else
#define TRACE false && _trace
#endif

#ifdef _DEBUG
bool _trace(TCHAR *format, ...)
{
	TCHAR buffer[1000];

	va_list argptr;
	va_start(argptr, format);
	wvsprintf(buffer, format, argptr);
	va_end(argptr);

	OutputDebugString(buffer);

	return true;
}

bool _traceln(TCHAR *format, ...)
{
	TCHAR buffer[1000];

	va_list argptr;
	va_start(argptr, format);
	wvsprintf(buffer, format, argptr);
	va_end(argptr);

	OutputDebugString(buffer);
	
	return _trace((LPTSTR)TEXT("\n"));
}
#endif
