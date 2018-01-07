#ifndef LXL_CFG_H

#define LXL_CFG_H

#include "lxl_common.h"




struct cfg_line
{
    char        *parameter;
    void        *variable;
    int         type;
    int         mandatory;
    uint64_t    min;
    uint64_t    max;
};

#define LXL_MAX_INCLUDE_LEVEL       10
#define LXL_CFG_LTRIM_CHARS         "\t "
#define LXL_CFG_RTRIM_CHARS LXL_CFG_LTRIM_CHARS "\r\n"


#define PARM_OPT        0
#define PARM_MAND       1




typedef enum
{
    TYPE_INT = 0,
    TYPE_STRING,
    TYPE_MULTISTRING,
    TYPE_UINT64,
    TYPE_STRING_LIST
}TYPE_ENUM;

#ifndef S_ISREG
#	define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif

#ifndef S_ISDIR
#	define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif


#endif
