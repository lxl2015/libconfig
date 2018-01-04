/**
 * @File      lxl_cfg.c
 * @Function  
 * @Author    LXL ,1010361011@qq.com
 * @version   0.1
 * @Date      2018-01-02
 */


#include "lxl_cfg.h"
#include "lxl_misc.h"



int __parse_cfg_file(const char *cfg_file, struct cfg_line *cfg, int level, int optional, int strict)
{
    FILE    *file = NULL;
    int lineno,param_valid, index;
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
            if (SUCCEED == is_utf8(line))
            {
                /* code */
                goto no_utf8;
            }

            parameter = line;
            if(NULL == (value = strchr(line, '=')))
                goto no_key_value;

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

                switch (cfg[i].tyoe)
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
                        lxl_strarr_add(cfg[i].variable, value);
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

            flcose(file);
    }



}



/**
 * @Function  lxl_parse_cfg_file 
 *
 * @Param     cfg_file  配置文件的路径
 * @Param     cfg       配置文件的参数列表
 * @Param     optional  定义是否是配置文件中是否包含其他配置文件
 * @Param     strict    
 *
 * @Returns   
 */
int lxl_parse_cfg_file(const char *cfg_file, struct cfg_line *cfg, int optional, int strict)
{
    return __parse_cfg_file(cfg_file, cfg, 0, optional, strict);
}
