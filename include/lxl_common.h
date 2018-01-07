#ifndef LXL_COMMON_H
#define LXL_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>



#ifndef SUCCEED
    #define SUCCEED     0
#endif

#ifndef  FAIL
    #define FAIL        -1
#endif



#define MAX_STRING_LEN      2048
#define MAX_BUFFER_LEN      65536


#ifndef YES
    #define YES 1
#endif

#ifndef NO
    #define NO 0
#endif

#ifndef __UINT64_C
    #ifdef UINT64_C
        #define __UINT64_C(c)	(UINT64_C(c))
    #else
        #define __UINT64_C(c)	(c ## ULL)
    #endif
#endif


#define lxl_free(ptr)       \
                            \
do                          \
{                           \
    /* code */              \
    if (ptr)                \
    {                       \
        free(ptr);          \
        ptr = NULL;         \
    }                       \
} while (0);                









/* config file parsing options */
#define LXL_CFG_FILE_REQUIRED   0
#define LXL_CFG_FILE_OPTIONAL   1

#define LXL_CFG_NOT_STRICT  0
#define LXL_CFG_STRICT      1



#define LXL_KIBIBYTE        1024
#define LXL_MEBIBYTE        1048576
#define LXL_GIBIBYTE        1073741824
#define LXL_TEBIBYTE        __UINT64_C(1099511627776)

#define SEC_PER_MIN         60
#define SEC_PER_HOUR        3600
#define SEC_PER_DAY         86400
#define SEC_PER_WEEK        (7 * SEC_PER_DAY)
#define SEC_PER_MONTH       (30 * SEC_PER_DAY)
#define SEC_PER_YEAR        (365 * SEC_PER_DAY)

#	ifndef PATH_SEPARATOR
#		define PATH_SEPARATOR	'/'
#	endif

#endif
