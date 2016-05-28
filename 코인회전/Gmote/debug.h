#pragma once

#if !defined(NDEBUG)

#define BEEP(freq, len)             Beep((freq), (len))
#define BEEPS(freq, len, num)     { for (int i = 0; i < (num); i++) Beep((freq), (len)); }

	#if defined(WIN32)


		#include <windows.h>
		#include <tchar.h>

		#define PRINTF(format, ...)     {fprintf (stderr, format, ##__VA_ARGS__); TCHAR __debugStr[1000]; _stprintf(__debugStr, _T(format), ##__VA_ARGS__); OutputDebugString(__debugStr);}

		#define ASSERT_MSG(expr, msg)  \
				                        if (!(expr)) \
				                        {\
				                            std::string msgString = std::string(msg); \
				                            fprintf(stderr, "%s(%d): %s in [%s]\n", __FILE__, __LINE__, msgString.c_str(), __FUNCTION__); \
				                            TCHAR __debugStr[1000]; \
				                            _stprintf(__debugStr, _T("%s(%d): %s in [%s]\n"),__FILE__, __LINE__, msgString.c_str(), __FUNCTION__); \
				                            ::OutputDebugString(__debugStr); \
				                            ::exit(-1); \
				                        }


		#define ASSERT(expr)          if (!(expr)) {fprintf(stderr, "%s(%d): ASSERT(%s) failed in [%s]\n", __FILE__, __LINE__, #expr, __FUNCTION__); TCHAR __debugStr[1000]; _stprintf(__debugStr, _T("%s(%d): ASSERT(%s) failed in [%s]\n"), __FILE__, __LINE__, #expr, __FUNCTION__); ::OutputDebugString(__debugStr);::exit(-1);}
		#define REMAIN_ASSERT_MSG(expr, msg)  if (!(expr)) { std::string msgString = std::string(msg); fprintf(stderr, "%s(%d): %s in [%s]\n", __FILE__, __LINE__, msgString.c_str(), __FUNCTION__); TCHAR __debugStr[1000]; _stprintf(__debugStr, _T("%s(%d): %s in [%s]\n"),__FILE__, __LINE__, msgString.c_str(), __FUNCTION__); ::OutputDebugString(__debugStr);::exit(-1);}
		#define REMAIN_ASSERT(expr)          if (!(expr)) {fprintf(stderr, "%s(%d): ASSERT(%s) failed in [%s]\n", __FILE__, __LINE__, #expr, __FUNCTION__); TCHAR __debugStr[1000]; _stprintf(__debugStr, _T("%s(%d): ASSERT(%s) failed in [%s]\n"), __FILE__, __LINE__, #expr, __FUNCTION__); ::OutputDebugString(__debugStr);::exit(-1);}
		#define NOTICE(msg) {std::string msgString = std::string(msg); fprintf(stderr, "%s(%d): %s in [%s]\n", __FILE__, __LINE__, msgString.c_str(), __FUNCTION__); TCHAR __debugStr[1000]; _stprintf(__debugStr, _T("%s(%d): %s in [%s]\n"),__FILE__, __LINE__, msgString.c_str(), __FUNCTION__); ::OutputDebugString(__debugStr);}
		#define PRINT_CURRENT_FUNCTION { \
				                                fprintf(stderr, "Current Function [%s]\n", __FUNCTION__); \
				                                TCHAR __debugStr[1000]; \
				                                _stprintf(__debugStr, _T("Current Function [%s]\n"), __FUNCTION__); \
				                                ::OutputDebugString(__debugStr); \
				                             }
	#else // WIN32


		#define PRINTF(format, ...)     fprintf (stderr, format, ##__VA_ARGS__)
		#define ASSERT_MSG(expr, msg)  if (!(expr)) {fprintf(stderr, "%s(%d): \"%s\" %s", __FILE__, __LINE__, __FUNCTION__, msg); ::exit(-1);}
		#define ASSERT(expr)          if (!(expr)) {fprintf(stderr, "%s(%d): ASSERT(%s) failed in [%s]", __FILE__, __LINE__, #expr, __FUNCTION__); ::exit(-1);}
		#define NOTICE(msg) {fprintf(stderr, "%s(%d): %s in [%s]\n", __FILE__, __LINE__, msg, __FUNCTION__);}
		#define REMAIN_ASSERT_MSG(expr, msg)  if (!(expr)) {fprintf(stderr, "%s(%d): %s in [%s]\n", __FILE__, __LINE__, msg, __FUNCTION__); ::exit(-1);}
		#define REMAIN_ASSERT(expr)          if (!(expr)) {fprintf(stderr, "%s(%d): ASSERT(%s) failed in [%s]\n", __FILE__, __LINE__, #expr, __FUNCTION__); ::exit(-1);}
		#define NOTICE(msg) {fprintf(stderr, "%s(%d): %s in [%s]\n", __FILE__, __LINE__, msg, __FUNCTION__); }
		#define PRINT_CURRENT_FUNCTION fprintf(stderr, "Current Function [%s]\n", __FUNCTION__)

	#endif


#else

	#define BEEP(freq, len)             ((void)0)
	#define BEEPS(freq, len, num)     ((void)0)
	#define PRINTF(format, ...)          ((void)0)
	#define ASSERT(expr)               ((void)0)
	#define ASSERT_MSG(expr, msg)  ((void)0)
	#define REMAIN_ASSERT_MSG(expr, msg)  ((void)(expr))
	#define REMAIN_ASSERT(expr)  ((void)(expr))
	#define PRINT_CURRENT_FUNCTION ((void)0)


	#if defined(WIN32)
		#include <windows.h>
		#include <tchar.h>
		#define NOTICE(msg) {fprintf(stderr, "%s(%d): %s in [%s]\n", __FILE__, __LINE__, msg, __FUNCTION__); TCHAR __debugStr[1000]; _stprintf(__debugStr, _T("%s(%d): %s in [%s]\n"),__FILE__, __LINE__, msg, __FUNCTION__); ::OutputDebugString(__debugStr);}
	#else
		#define NOTICE(msg) {fprintf(stderr, "%s(%d): %s in [%s]\n", __FILE__, __LINE__, msg, __FUNCTION__);}
	#endif


#endif






