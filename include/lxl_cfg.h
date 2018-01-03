#ifndef LXL_CFG_H

#define LXL_CFG_H

#include "lxl_common.h"




struct cfg_line
{
    char        *parameter;
    void        *variable;
    int         type;
    int         tmandatory;
    uint64_t    min;
    uint64_t    max;
};

#define LXL_MAX_INCLUDE_LEVEL       10
#define LXL_CFG_LTRIM_CHARS         "\t "
#define LXL_CFG_RTRIM_CHARS LXL_CFG_LTRIM_CHARS "\r\n"



typedef enum
{
    TYPE_INT = 0,
    TYPE_STRING,
    TYPE_MULTISTRING,
    TYPE_UINT64,
    TYPE_STRING_LIST
}TYPE_ENUM;



#endif
