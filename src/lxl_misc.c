/**
 * @File      lxl_misc.c
 * @Function  
 * @Author    LXL ,1010361011@qq.com
 * @version   0.1
 * @Date      2018-01-03
 */
#include "lxl_misc.h"

#define _DEBUG_

/**
 * @Function  lxl_strdup 
 *
 * @Param     fmt
 * @Param     ...
 * strdup的二次封装，调用后，好释放内存
 * @Returns   
 */
char *lxl_strdup(const char *fmt, ...)
{
#ifdef _DEBUG_
    va_list args;
    char str[MAX_STRING_LEN] = {0};

    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);

    return strdup(str);
#else
    return NULL;
#endif

}


/**
 * @Function  lxl_strdup2 
 *
 * @Param     old
 * @Param     str
 * 变量替换
 * @Returns   
 */
char *lxl_strdup2(char *old, char *str)
{
    int retry;
    char *ptr = NULL;
    lxl_free(old);

    for(retry = 10; 0 < retry && NULL == ptr; ptr = strdup(str), retry --);


    if(NULL != ptr)
        return ptr;

    lxl_err(lxl_strdup("lxl_strdup2:out of memory."));
    exit(EXIT_FAILURE);
}



/**
 * @Function  lxl_log 
 *  
 * @Param     out
 * 输出日志信息
 */
void lxl_log(char *out)
{
#ifdef _DEBUG_
    if (NULL == out)
    {
        /* code */
        fprintf(stdout, "null");
    }
    else
    {
        fprintf(stdout, "%s\n", out);
        free(out);
        out = NULL;
    }
#endif
}



/**
 * @Function  lxl_err 
 *
 * @Param     out
 * 输出错误信息。
 */
void lxl_err(char *out)
{
#ifdef _DEBUG_
    if(NULL == out)
    {
        fprintf(stderr, "null");
    }
    else
    {
        fprintf(stderr, "%s\n", out);
        free(out);
        out = NULL;
    }
#endif
}
