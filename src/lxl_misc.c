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
 * @Function  lxl_strdup 		strdup的二次封装，调用后，好释放内存
 *
 * @Param     fmt
 * @Param     ...
 * 
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
 * @Function  lxl_strdup2 	变量替换
 *
 * @Param     old
 * @Param     str
 * 
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
 * @Function  lxl_log 		输出日志信息
 *  
 * @Param     out
 * 
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

char *lxl_dvsprintf(char *dest, const char *f, va_list args)
{
    char    *string = NULL;
    int n, size = MAX_STRING_LEN >> 1;

    va_list curr;

    while (1)
    {
        string = malloc(size);

        va_copy(curr, args);
        n = vsnprintf(string, size, f, curr);
        va_end(curr);

        if (0 <= n && n < size)
            break;

        /* result was truncated */
        if (-1 == n)
            size = size * 3 / 2 + 1;    /* the length is unknown */
        else
            size = n + 1;   /* n bytes + trailing '\0' */

        lxl_free(string);

    }

    lxl_free(dest);

    return string;

}


char *lxl_dsprintf(char *dest, const char *f, ...)
{
    char    *string;
    va_list args;

    va_start(args, f);

    string = lxl_dvsprintf(dest, f, args);

    va_end(args);

    return string;

}



/**
 * @Function  lxl_err 		输出错误信息
 *
 * @Param     out
 * 
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



/**
 * @Function  is_uint_n_range       检查字符串是否在指定的范围内
 *
 * @Param     str                   待检查的字符串
 * @Param     n                     字符串长度
 * @Param     value                 要写入的转换值的缓冲区，可以是NULL
 * @Param     size                  缓冲区的大小
 * @Param     min                   指定范围的最小值
 * @Param     max                   指定范围的最大值
 *
 * @Returns   
 */
int is_uint_n_range(const char *str, size_t n, void *value, size_t size, uint64_t min, uint64_t max)
{
    uint64_t        value_uint64 = 0, c;
    const uint64_t  max_uint64 = ~(uint64_t)__UINT64_C(0);

    if ('\0' == *str || 0 == n || sizeof(uint64_t) < size || (0 == size && NULL != value))
        return FAIL;

    while ('\0' != *str && 0 < n--)
    {
        if (0 == isdigit(*str))
            return FAIL;    /* not a digit */

        c = (uint64_t)(unsigned char)(*str - '0');

        if ((max_uint64 - c) / 10 < value_uint64)
            return FAIL;    /* maximum value exceeded */

        value_uint64 = value_uint64 * 10 + c;

        str++;

    }

    if (min > value_uint64 || value_uint64 > max)
        return FAIL;

    if (NULL != value)
    {
        /* On little endian architecture the output value will be stored starting from the first bytes */
        /* of 'value' buffer while on big endian architecture it will be stored starting from the last */
        /* bytes. We handle it by storing the offset in the most significant byte of short value and   */
        /* then use the first byte as source offset.                                                   */
        unsigned short  value_offset = (unsigned short)((sizeof(uint64_t) - size) << 8);

        memcpy(value, (unsigned char *)&value_uint64 + *((unsigned char *)&value_offset), size);

    }

    return SUCCEED;

}



/**
 * @Function  suffix2factor 
 *
 * @Param     c         KMGTsmhdw转化
 *
 * @Returns   
 */
uint64_t suffix2factor(char c)
{
    switch (c)
    {
        case 'K':
            return LXL_KIBIBYTE;
        case 'M':
            return LXL_MEBIBYTE;
        case 'G':
            return LXL_GIBIBYTE;
        case 'T':
            return LXL_TEBIBYTE;
        case 's':
            return 1;
        case 'm':
            return SEC_PER_MIN;
        case 'h':
            return SEC_PER_HOUR;
        case 'd':
            return SEC_PER_DAY;
        case 'w':
            return SEC_PER_WEEK;
        default:
            return 1;

    }

}



