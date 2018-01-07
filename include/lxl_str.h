#ifndef LXL_STR_H
#define LXL_STR_H
#include "lxl_common.h"


int	is_utf8(const char *text);
void ltrim(char *str, const char *charlist);
int	rtrim(char *str, const char *charlist);
int str2uint64(const char *str, const char *suffixes, uint64_t *value);
void trim_str_list(char *list, char delimiter);
void strarr_add(char ***arr, const char *entry);
#endif
