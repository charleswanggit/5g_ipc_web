#ifndef _AK_LOG_H_
#define _AK_LOG_H_

#define MODULE_NAME_APP                "Application"

#include <stdio.h>

/**
 * print functions
 * 1. error print, call this when sys-func fail or self-define error occur
 * 2. warning print, warning conditions
 * 3. notice print, normal but significant
 * 4. normal print, normal message or tips
 * 5. info print, use for cycle print like watch-dog feed print
 * 6. debug print, use for debug
 */
enum LOG_LEVEL {
	LOG_LEVEL_RESERVED = 0,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_NOTICE,
	LOG_LEVEL_NORMAL,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_NUM,
};

#define COLOR_NONE 				"\033[0m"
#define COLOR_RED               "\033[0;31m"
#define COLOR_GREEN             "\033[0;32m"
#define COLOR_YELLOW            "\033[1;33m"
#define COLOR_LIGHT_CYAN        "\033[1;36m"
#define COLOR_WHITE             "\033[1;37m"

#define _INFO(__fmt...)  akae_log_tag_info("akae_wlan", ##__fmt)
#define _WARN(__fmt...)  akae_log_tag_warning("akae_wlan", ##__fmt)
#define _ERR(__fmt...)   akae_log_tag_error("akae_wlan", ##__fmt)

#if 0
#ifndef _PRINT_LEVEL_H_
#define _PRINT_LEVEL_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include<stdio.h>
 
enum _LEVEL_N{	
	ERROR=0,	
	INFO,
	WARN,
	DEBUG,	
	ALWAYS,	
};
 
#define COLOR_NONE 				"\033[0m"
#define COLOR_RED               "\033[0;31m"
#define COLOR_GREEN             "\033[0;32m"
#define COLOR_YELLOW            "\033[1;33m"
#define COLOR_LIGHT_CYAN        "\033[1;36m"
#define COLOR_WHITE             "\033[1;37m"
 
#ifndef PRINT_LEVEL
#define PRINT_LEVEL WARN
#endif
 
 
#define print_level(level,fmt,arg...) do {\
	if(level <= PRINT_LEVEL)\
	{\
		const char* pszColor = NULL;\
		switch(level)\
		{\
			case ERROR:\
				pszColor = COLOR_RED;\
				break;\
			case INFO:\
				pszColor = COLOR_LIGHT_CYAN;\
				break;\
			case WARN:\
				pszColor = COLOR_YELLOW;\
				break;\
			case DEBUG:\
				pszColor = COLOR_GREEN;\
				break;\
			default:\
				pszColor = COLOR_WHITE;\
				break;\
		}\
		printf("%s %s>%s:#%d, "COLOR_NONE" "fmt, pszColor, __FILE__, __FUNCTION__, __LINE__, ##arg);\
	}\
	} while(0)
 
		
#ifdef __cplusplus
}
#endif
 
#endif
#endif

#if 1
/**
 * ak_print: print function we defined for debugging
 * @module[IN]: module id
 * @level[IN]: print level [0,5]
 * @fmt[IN]: format like printf()
 * @...[IN]: variable arguments list
 * return: we return 0 always.
 */
#define ak_print(level,fmt,arg...) do {\
        if(level <= LOG_LEVEL_DEBUG)\
        {\
            const char* pszColor = NULL;\
            switch(level)\
            {\
                case LOG_LEVEL_ERROR:\
                    pszColor = COLOR_RED;\
                    break;\
                case LOG_LEVEL_INFO:\
                    pszColor = COLOR_LIGHT_CYAN;\
                    break;\
                case LOG_LEVEL_WARNING:\
                    pszColor = COLOR_YELLOW;\
                    break;\
                case LOG_LEVEL_DEBUG:\
                    pszColor = COLOR_GREEN;\
                    break;\
                default:\
                    pszColor = COLOR_WHITE;\
                    break;\
            }\
            printf( "%s %s/%s:#%d, " COLOR_NONE " " fmt , pszColor, __FILE__, __FUNCTION__, __LINE__, ##arg);\
        }\
    } while(0)


#define mtc_print(level,fmt,arg...) do {\
	if(level <= LOG_LEVEL_NUM)\
	{\
		const char* pszColor = NULL;\
		switch(level)\
		{\
			case LOG_LEVEL_ERROR:\
				pszColor = COLOR_RED;\
				break;\
			case LOG_LEVEL_INFO:\
				pszColor = COLOR_LIGHT_CYAN;\
				break;\
			case LOG_LEVEL_WARNING:\
				pszColor = COLOR_YELLOW;\
				break;\
			case LOG_LEVEL_DEBUG:\
				pszColor = COLOR_GREEN;\
				break;\
			default:\
				pszColor = COLOR_WHITE;\
				break;\
		}\
		printf( "%s %s/%s:#%d, " COLOR_NONE " " fmt , pszColor, __FILE__, __FUNCTION__, __LINE__, ##arg);\
	}\
} while(0)

    
#define mtc_print_error(fmt, arg...) \
	mtc_print(LOG_LEVEL_ERROR, fmt, ##arg)

#define mtc_print_warning(fmt, arg...) \
	mtc_print(LOG_LEVEL_WARNING, fmt, ##arg)
#define mtc_print_notice(fmt, arg...) \
	mtc_print(LOG_LEVEL_NOTICE, fmt, ##arg)
#define mtc_print_normal(fmt, arg...) \
	mtc_print(LOG_LEVEL_NORMAL, fmt, ##arg)
#define mtc_print_info(fmt, arg...) \
	mtc_print(LOG_LEVEL_INFO, fmt, ##arg)
#define mtc_print_debug(fmt, arg...) \
	mtc_print(LOG_LEVEL_DEBUG, fmt, ##arg)

#define mtc_print_error_ex(fmt, arg...) \
	mtc_print(LOG_LEVEL_ERROR, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define mtc_print_warning_ex(fmt, arg...) \
	mtc_print(LOG_LEVEL_WARNING, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define mtc_print_notice_ex(fmt, arg...) \
	mtc_print(LOG_LEVEL_NOTICE, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define mtc_print_normal_ex(fmt, arg...) \
	mtc_print(LOG_LEVEL_NORMAL, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define mtc_print_info_ex(fmt, arg...) \
	mtc_print(LOG_LEVEL_INFO, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_debug_ex(fmt, arg...) \
	mtc_print(LOG_LEVEL_DEBUG,  "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#endif
/**
 * ak_print_get_version - get log version
 * return: version string
 * notes:
 */
const char* ak_print_get_version(void);

/**
 * ak_print_set_level - set current print level
 * @module_id[IN]: module id
 * @level[IN]: current level
 * return: old level
 */
int ak_print_set_level(int module_id, int level);

/**
 * ak_print_set_syslog_level - set print log level
 * @module[IN] : module id
 * @level[IN]: level to set, value in enum LOG_LEVEL
 * return: always return 0
 */
int ak_print_set_syslog_level(int module_id, int level);

#endif

/* end of file */
