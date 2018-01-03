/**
 * @File      lxl_misc.c
 * @Function  
 * @Author    LXL ,1010361011@qq.com
 * @version   0.1
 * @Date      2018-01-03
 */
#include "lxl_misc.h"



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
    va_list args;
    char str[MAX_STRING_LEN] = {0};

    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);

    return strdup(str);

}

/**
 * @Function  lxl_log 
 *
 * @Param     out
 */
void lxl_log(char *out)
{
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
}



/**
 * @Function  lxl_err 
 *
 * @Param     out
 */
void lxl_err(char *out)
{
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
}
