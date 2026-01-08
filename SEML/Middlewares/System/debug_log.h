#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_
#include "SEML_common.h"

#define LOG_DEBUG_LEVEL 0
#define LOG_INFO_LEVEL 1
#define LOG_WARING_LEVEL 2
#define LOG_ERROR_LEVEL 3
#define LOG_ASSEST_LEVEL 4
#define LOG_NO_OUT_LEVEL 5

#define LOG_FILTER_LEVEL LOG_WARING_LEVEL

#define LOG_A(format, ...) LOG_ASSEST(format, ##__VA_ARGS__)
#define LOG_E(format, ...) LOG_ERROR(format, ##__VA_ARGS__)
#define LOG_W(format, ...) LOG_WARING(format, ##__VA_ARGS__)
#define LOG_I(format, ...) LOG_INFO(format, ##__VA_ARGS__)
#define LOG_D(format, ...) LOG_DEBUG(format, ##__VA_ARGS__)

#if LOG_ASSEST_LEVEL >= LOG_FILTER_LEVEL
#define LOG_ASSEST(format, ...) 
#else
#define LOG_ASSEST(format, ...)
#endif
#if LOG_ERROR_LEVEL >= LOG_FILTER_LEVEL
#define LOG_ERROR(format, ...)
#else
#define LOG_ERROR(format, ...)
#endif
#if LOG_WARING_LEVEL >= LOG_FILTER_LEVEL
#define LOG_WARING(format, ...)
#else
#define LOG_WARING(format, ...)
#endif
#if LOG_INFO_LEVEL >= LOG_FILTER_LEVEL
#define LOG_INFO(format, ...)
#else
#define LOG_INFO(format, ...)
#endif
#if LOG_DEBUG_LEVEL >= LOG_FILTER_LEVEL
#define LOG_DEBUG(format, ...)
#else
#define LOG_DEBUG(format, ...)
#endif

#endif