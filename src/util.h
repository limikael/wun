#ifndef __WUN_UTIL_H__
#define __WUN_UTIL_H__

int vasprintf(char **str, const char *fmt, va_list args);
int asprintf(char **str, const char *fmt, ...);

#endif