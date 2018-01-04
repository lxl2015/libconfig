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



int	is_utf8(const char *text)
{
	unsigned int	utf32;
	unsigned char	*utf8;
	size_t		i, mb_len, expecting_bytes = 0;

	while ('\0' != *text)
	{
		/* single ASCII character */
		if (0 == (*text & 0x80))
		{
			text++;
			continue;
		}

		/* unexpected continuation byte or invalid UTF-8 bytes '\xfe' & '\xff' */
		if (0x80 == (*text & 0xc0) || 0xfe == (*text & 0xfe))
			return FAIL;

		/* multibyte sequence */

		utf8 = (unsigned char *)text;

		if (0xc0 == (*text & 0xe0))		/* 2-bytes multibyte sequence */
			expecting_bytes = 1;
		else if (0xe0 == (*text & 0xf0))	/* 3-bytes multibyte sequence */
			expecting_bytes = 2;
		else if (0xf0 == (*text & 0xf8))	/* 4-bytes multibyte sequence */
			expecting_bytes = 3;
		else if (0xf8 == (*text & 0xfc))	/* 5-bytes multibyte sequence */
			expecting_bytes = 4;
		else if (0xfc == (*text & 0xfe))	/* 6-bytes multibyte sequence */
			expecting_bytes = 5;

		mb_len = expecting_bytes + 1;
		text++;

		for (; 0 != expecting_bytes; expecting_bytes--)
		{
			/* not a continuation byte */
			if (0x80 != (*text++ & 0xc0))
				return FAIL;
		}

		/* overlong sequence */
		if (0xc0 == (utf8[0] & 0xfe) ||
				(0xe0 == utf8[0] && 0x00 == (utf8[1] & 0x20)) ||
				(0xf0 == utf8[0] && 0x00 == (utf8[1] & 0x30)) ||
				(0xf8 == utf8[0] && 0x00 == (utf8[1] & 0x38)) ||
				(0xfc == utf8[0] && 0x00 == (utf8[1] & 0x3c)))
		{
			return FAIL;
		}

		utf32 = 0;

		if (0xc0 == (utf8[0] & 0xe0))
			utf32 = utf8[0] & 0x1f;
		else if (0xe0 == (utf8[0] & 0xf0))
			utf32 = utf8[0] & 0x0f;
		else if (0xf0 == (utf8[0] & 0xf8))
			utf32 = utf8[0] & 0x07;
		else if (0xf8 == (utf8[0] & 0xfc))
			utf32 = utf8[0] & 0x03;
		else if (0xfc == (utf8[0] & 0xfe))
			utf32 = utf8[0] & 0x01;

		for (i = 1; i < mb_len; i++)
		{
			utf32 <<= 6;
			utf32 += utf8[i] & 0x3f;
		}


		if (utf32 > 0x10ffff || 0xd800 == (utf32 & 0xf800))
			return FAIL;
	}

	return SUCCEED;
}


void	ltrim(char *str, const char *charlist)
{
	char	*p;

	if (NULL == str || '\0' == *str)
		return;

	for (p = str; '\0' != *p && NULL != strchr(charlist, *p); p++)
		;

	if (p == str)
		return;

	while ('\0' != *p)
		*str++ = *p++;

	*str = '\0';
}



int	rtrim(char *str, const char *charlist)
{
	char	*p;
	int	count = 0;

	if (NULL == str || '\0' == *str)
		return count;

	for (p = str + strlen(str) - 1; p >= str && NULL != strchr(charlist, *p); p--)
	{
		*p = '\0';
		count++;
	}

	return count;
}