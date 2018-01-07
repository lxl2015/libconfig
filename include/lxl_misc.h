#ifndef LXL_MISC_H
#define LXL_MISC_H

#include "lxl_common.h"
#include <stdarg.h>

#define is_uint64_n(str, n, value) \
        is_uint_n_range(str, n, value, 8, 0x0, __UINT64_C(0xFFFFFFFFFFFFFFFF))


		
uint64_t suffix2factor(char c);
int is_uint_n_range(const char *str, size_t n, void *value, size_t size, uint64_t min, uint64_t max);
void lxl_err(char *out);
char *lxl_dsprintf(char *dest, const char *f, ...);
char *lxl_dvsprintf(char *dest, const char *f, va_list args);
void lxl_log(char *out);
char *lxl_strdup2(char *old, char *str);
char *lxl_strdup(const char *fmt, ...);
#endif
