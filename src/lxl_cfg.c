/**
 * @File      lxl_cfg.c
 * @Function  
 * @Author    LXL ,1010361011@qq.com
 * @version   0.1
 * @Date      2018-01-02
 */


#include "lxl_cfg.h"
#include "lxl_misc.h"
#include "lxl_str.h"


int __parse_cfg_file(const char *cfg_file, struct cfg_line *cfg, int level, int optional, int strict);




/**
 * @Function  parse_glob        解析匹配文件夹下的配置文件
 *
 * @Param     glob              在指定的Include包含的匹配
 * @Param     path              解析路径或文件
 * @Param     pattern           解析模式，如果path是路径
 *
 * @Returns   
 */
static int parse_glob(const char *glob, char **path, char **pattern)
{
	const char	*p;

	if (NULL == (p = strchr(glob, '*')))
	{
		*path = lxl_strdup2(NULL, glob);
		*pattern = NULL;

		goto trim;
	}

	if (NULL != strchr(p + 1, PATH_SEPARATOR))
	{
		lxl_err(lxl_strdup("%s: glob pattern should be the last component of the path", glob));
		return FAIL;
	}

	do
	{
		if (glob == p)
		{
			lxl_err(lxl_strdup("%s: path should be absolute", glob));
			return FAIL;
		}

		p--;
	}
	while (PATH_SEPARATOR != *p);

	*path = lxl_strdup2(NULL, glob);
	(*path)[p - glob] = '\0';

	*pattern = lxl_strdup2(NULL, p + 1);
trim:
	if (0 != rtrim(*path, "/") && NULL == *pattern)
		*pattern = lxl_strdup2(NULL, "*");			/* make sure path is a directory */

	if ('\0' == (*path)[0] && '/' == glob[0])			/* retain forward slash for "/" */
	{
		(*path)[0] = '/';
		(*path)[1] = '\0';
	}
	return SUCCEED;
}



/**
 * @Function  match_glob    检查是否是一个文件
 *
 * @Param     file
 * @Param     pattern
 *
 * @Returns   
 */
static int match_glob(const char *file, const char *pattern)
{
    const char	*f, *g, *p, *q;

    f = file;
    p = pattern;

    while (1)
    {
        /* corner case */

        if ('\0' == *p)
            return '\0' == *f ? SUCCEED : FAIL;

        /* find a set of literal characters */

        while ('*' == *p)
            p++;

        for (q = p; '\0' != *q && '*' != *q; q++)
            ;

        /* if literal characters are at the beginning... */

        if (pattern == p)
        {
            if (0 != strncmp(f, p, q - p))
                return FAIL;

            f += q - p;
            p = q;

            continue;
        }

        /* if literal characters are at the end... */

        if ('\0' == *q)
        {
            for (g = f; '\0' != *g; g++)
                ;

            if (g - f < q - p)
                return FAIL;
            return 0 == strcmp(g - (q - p), p) ? SUCCEED : FAIL;
        }

        /* if literal characters are in the middle... */

        while (1)
        {
            if ('\0' == *f)
                return FAIL;
            if (0 == strncmp(f, p, q - p))
            {
                f += q - p;
                p = q;

                break;
            }

            f++;
        }
    }
}





/**
 * @Function  parse_cfg_dir         检查配置文件夹下所有配置文件
 *
 * @Param     path                  配置文件夹路径
 * @Param     pattern               解析模式
 * @Param     cfg                   指向配置参数的结构体
 * @Param     level                 包含配置文件层数
 * @Param     strict                将未知参数视为错误
 *
 * @Returns   
 */
static int parse_cfg_dir(const char *path, const char *pattern, struct cfg_line *cfg, int level, int strict)
{
    DIR		*dir;
    struct dirent	*d;
    struct stat	sb;
    char		*file = NULL;
    int		ret = FAIL;

    if (NULL == (dir = opendir(path)))
    {
        lxl_err(lxl_strdup("%s: %s", path, strerror(errno)));
        goto out;
    }

    while (NULL != (d = readdir(dir)))
    {
        file = lxl_dsprintf(file, "%s/%s", path, d->d_name);

        if (0 != stat(file, &sb) || 0 == S_ISREG(sb.st_mode))
            continue;

        if (NULL != pattern && SUCCEED != match_glob(d->d_name, pattern))
            continue;

        if (SUCCEED != __parse_cfg_file(file, cfg, level, LXL_CFG_FILE_REQUIRED, strict))
            goto close;
    }

    ret = SUCCEED;
close:
    if (0 != closedir(dir))
    {
        lxl_err(lxl_strdup("%s: %s", path, strerror(errno)));
        ret = FAIL;
    }

    lxl_free(file);
out:
    return ret;
}





/**
 * @Function  parse_cfg_object      解析 "Include=..." 的配置文件
 *
 * @Param     cfg_file              配置文件的名称包含路径
 * @Param     cfg                   指向配置参数的结构体
 * @Param     level                 包含配置文件层数
 * @Param     strict                将未知参数视为错误
 *
 * @Returns   
 */
static int parse_cfg_object(const char *cfg_file, struct cfg_line *cfg, int level, int strict)
{
    int		ret = FAIL;
    char		*path = NULL, *pattern = NULL;
    struct stat	sb;

    if (SUCCEED != parse_glob(cfg_file, &path, &pattern))
        goto clean;

    if (0 != stat(path, &sb))
    {
        lxl_err(lxl_strdup("%s: %s", path, strerror(errno)));
        goto clean;
    }

    if (0 == S_ISDIR(sb.st_mode))
    {
        if (NULL == pattern)
        {
            ret = __parse_cfg_file(path, cfg, level, LXL_CFG_FILE_REQUIRED, strict);
            goto clean;
        }

        lxl_err(lxl_strdup("%s: base path is not a directory", cfg_file));
        goto clean;
    }

    ret = parse_cfg_dir(path, pattern, cfg, level, strict);
clean:
    lxl_free(pattern);
    lxl_free(path);

    return ret;
}


/**
 * @Function  __parse_cfg_file      解析配置文件
 *
 * @Param     cfg_file              配置文件的名称包含路径
 * @Param     cfg                   指向配置参数的结构体
 * @Param     level                 包含配置文件层数
 * @Param     optional              不要将缺少的配置文件视为错误
 * @Param     strict                将未知参数视为错误
 *
 * @Returns   
 */
int __parse_cfg_file(const char *cfg_file, struct cfg_line *cfg, int level, int optional, int strict)
{
    FILE    *file = NULL;
    int lineno,param_valid, i;
    char line[MAX_STRING_LEN + 3], *parameter, *value;
    uint64_t    var;
    size_t  len;


    if (++level > LXL_MAX_INCLUDE_LEVEL)
    {
        /* code */
        lxl_err(lxl_strdup("Recursion detected! Skipped processing of '%s',Max recursion:%d.\n", cfg_file, LXL_MAX_INCLUDE_LEVEL));
        return FAIL;
    }

    if(NULL != cfg_file)
    {
        if (NULL == (file = fopen(cfg_file, "r")))
        {
            /* code */
            goto cannot_open;
        }

        for(lineno = 1; NULL != fgets(line, sizeof(line), file) ; lineno ++)
        {
            len = strlen(line);

            /*
             * 检查每行的数据最大值是否超过2048
             * */
            if (MAX_STRING_LEN < len && NULL == strchr("\r\n",line[MAX_STRING_LEN]))
            {
                goto line_too_long;
            }

            ltrim(line, LXL_CFG_LTRIM_CHARS);
            rtrim(line, LXL_CFG_RTRIM_CHARS);

            if( '#' == *line || '\0' == *line )
                continue;

            /*
             * 检查是否是utf-8字符集，仅支持utf-8
             * */
            if (SUCCEED != is_utf8(line))
            {
                /* code */
                goto no_utf8;
            }

            parameter = line;
            if(NULL == (value = strchr(line, '=')))
                goto non_key_value;

            *value++ = '\0';

            rtrim(parameter, LXL_CFG_RTRIM_CHARS);
            ltrim(value, LXL_CFG_LTRIM_CHARS);

            lxl_log(lxl_strdup("cfg: para:[%s] val [%s]", parameter, value));

            if(0 == strcmp(parameter, "Include"))
            {
                if(FAIL == parse_cfg_object(value, cfg, level, strict))
                {
                    fclose(file);
                    goto error;
                }

                continue;
            }

            param_valid = 0;

            for(i = 0 ; NULL != cfg[i].parameter; i++)
            {
                if (0 != strcmp(cfg[i].parameter, parameter))
                {
                    /* code */
                    continue;
                }

                param_valid = 1;
                lxl_log(lxl_strdup("accepted configuration parameter: '%s' = '%s'",parameter, value));

                switch (cfg[i].type)
                {
                    case TYPE_INT:
                        if( FAIL == str2uint64(value, "KMGT", &var) )
                            goto incorrect_config;

                        if(cfg[i].min > var || (0 != cfg[i].max && var > cfg[i].max))
                            goto incorrect_config;

                        *((int *) cfg[i].variable) = (int)var;
                        break;
                    case TYPE_STRING_LIST:

                        trim_str_list(value, ',');
                    case TYPE_STRING:
                        *((char **)cfg[i].variable) = lxl_strdup2(*((char **)cfg[i].variable), value); 
                        break;
                    case TYPE_MULTISTRING:
                        strarr_add(cfg[i].variable, value);
                        break;
                    case TYPE_UINT64:
                        if(FAIL == str2uint64(value , "KMGT", &var))
                            goto incorrect_config;

                        if(cfg[i].min > var || (var > cfg[i].max && 0 != cfg[i].max))
                            goto incorrect_config;
                        *((uint64_t *)cfg[i].variable) = var;
                        break;
                    default:
                        assert(0);
                }
            }
            if(0 == param_valid && LXL_CFG_STRICT == strict)
                goto unknown_parameter;
        }

        fclose(file);
    }
    if(1 != level)
        return SUCCEED;

    for(i = 0; NULL != cfg[i].parameter; i++)
    {
        if(PARM_MAND != cfg[i].mandatory)
            continue;

        switch (cfg[i].type)
        {
            case TYPE_INT:
                if(0 == *((int *)cfg[i].variable))
                    goto missing_mandatory;
                break;
            case TYPE_STRING:
            case TYPE_STRING_LIST:
                if(NULL == (*(char **)cfg[i].variable))
                    goto missing_mandatory;
                break;
            default:
                assert(0);
        }
    }
    return SUCCEED;



cannot_open:
    if( 0 != optional )
        return SUCCEED;
    lxl_err(lxl_strdup("cannot open config file [%s]:[%s]\n", cfg_file, strerror(errno)));
    goto error;
line_too_long:
    fclose(file);

    lxl_err(lxl_strdup("line %d exceeds %d byte length limit int config file [%s]\n", lineno, MAX_STRING_LEN, cfg_file));
    goto error;

no_utf8:

    fclose(file);
    lxl_err(lxl_strdup("no-UTF-8 character at line %d \"%s\" in config file [%s]\n", lineno, line, cfg_file));
    goto error;

non_key_value:

    fclose(file);
    lxl_err(lxl_strdup("invalid entry \"%s\" (not following \"parameter=value\" notation) in config file [%s], line %d\n", line, cfg_file, lineno));
    goto error;

incorrect_config:
    fclose(file);
    lxl_err(lxl_strdup("wrong value of \"%s\" in config file [%s],line %d\n", cfg[i].parameter, cfg_file, lineno));

    goto error;

unknown_parameter:
    fclose(file);

    lxl_err(lxl_strdup("unknown parameter \"%s\" in config file [%s], line %d\n", parameter, cfg_file, lineno));

    goto error;

missing_mandatory:
    lxl_err(lxl_strdup("missing mandatory parameter \"%s\" in config file [%s]\n", cfg[i].parameter, cfg_file));

error:
    exit(EXIT_FAILURE);


}



/**
 * @Function  lxl_parse_cfg_file 
 *
 * @Param     cfg_file  配置文件的路径
 * @Param     cfg       配置文件的参数列表
 * @Param     optional  定义是否是配置文件中是否包含其他配置文件
 * @Param     strict    将未知参数视为错误
 *
 * @Returns   
 */
int lxl_parse_cfg_file(const char *cfg_file, struct cfg_line *cfg, int optional, int strict)
{
    return __parse_cfg_file(cfg_file, cfg, 0, optional, strict);
}
