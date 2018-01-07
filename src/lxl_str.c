/**
 * @File      lxl_str.c
 * @Function  
 * @Author    LXL ,1010361011@qq.com
 * @version   0.1
 * @Date      2018-01-07
 */
#include "lxl_str.h"
#include "lxl_misc.h"
 


/**
 * @Function  strarr_add        添加一个字符串到动态字符串数组
 *
 * @Param     arr               指向字符串数组的指针
 * @Param     entry             添加字符串
 */
void strarr_add(char ***arr, const char *entry)
{
    int i;

    assert(entry);

    for (i = 0; NULL != (*arr)[i]; i++)
        ;

    *arr = realloc(*arr, sizeof(char *) * (i + 2));

    (*arr)[i] = lxl_strdup2((*arr)[i], entry);
    (*arr)[++i] = NULL;

}



/**
 * @Function  trim_str_list         删除字符串列表项分隔符周围的空格
 *
 * @Param     list                  列表（包含由分隔符分隔的项目的字符串）
 * @Param     delimiter             列表分隔符
 *
 * @Returns   
 */
void trim_str_list(char *list, char delimiter)
{
    /* NB! strchr(3): "terminating null byte is considered part of the string" */
    const char  *whitespace = " \t";
    char        *out, *in;

    out = in = list;

    while ('\0' != *in)
    {
        /* trim leading spaces from list item */
        while ('\0' != *in && NULL != strchr(whitespace, *in))
            in++;

        /* copy list item */
        while (delimiter != *in && '\0' != *in)
            *out++ = *in++;

        /* trim trailing spaces from list item */
        if (out > list)
        {
            while (NULL != strchr(whitespace, *(--out)))
                ;
            out++;

        }
        if (delimiter == *in)
            *out++ = *in++;

    }
    *out = '\0';

}




/**
 * @Function  str2uint64        字符串转化成uint64_t
 *
 * @Param     str               带转化的字符串
 * @Param     suffixes          处理KMGT
 * @Param     value             转换的值
 *
 * @Returns                     成功返回0
 */
int str2uint64(const char *str, const char *suffixes, uint64_t *value)
{
    size_t      sz;
    const char  *p;
    int     ret;
    uint64_t    factor = 1;

    sz = strlen(str);
    p = str + sz - 1;

    if (NULL != strchr(suffixes, *p))
    {
        factor = suffix2factor(*p);
        sz--;

    }

    if (SUCCEED == (ret = is_uint64_n(str, sz, value)))
        *value *= factor;

    return ret;

}




/**
 * @Function  is_utf8   检查字符串是否是utf-8格式的
 *
 * @Param     text      待检测的字符串
 *
 * @Returns   
 */
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


/**
 * @Function  ltrim         去掉字开头符串的字符
 *
 * @Param     str           待去掉开头字符的字符串
 * @Param     charlist      要去掉的字符
 *
 */
void ltrim(char *str, const char *charlist)
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






/**
 * @Function  rtrim     去掉末尾字符 
 *
 * @Param     str       去掉charlist的末尾字符
 * @Param     charlist  末尾字符
 *
 * @Returns             去掉末尾字符的个数   
 */
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