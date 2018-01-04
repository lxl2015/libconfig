#ifndef LXL_MISC_H
#define LXL_MISC_H

#include "lxl_common.h"
#include <stdarg.h>


char *lxl_strdup(const char *fmt, ...);
char *lxl_strdup2(char *old, char *str);
void lxl_log(char *out);
void lxl_err(char *out);


#endif
