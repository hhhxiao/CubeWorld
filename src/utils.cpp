//
// Created by xhy on 2024/5/3.
//

#include "utils.h"
#include <cstdarg>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
// #define KGRN "\x1B[32m"
#define KGRN ""
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
// #define KWHT ""
#define KWHT "\x1B[37m"
#include <cstdio>
#include <cstdlib>

void log(const char *file_name, const char *function_name, size_t line, const char *fmt, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, "[%s%zu @ %s]: ", file_name, line, function_name);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    fflush(stdout);
#endif
}

void error_msg(const char *file_name, const char *function_name, size_t line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, KRED "[ERROR] [%s%zu @ %s]: ", file_name, line, function_name);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, KWHT "\n");
    fflush(stdout);
}

void M_Assert(const char *expr_str, bool expr, const char *file, int line, const char *msg, ...) {
    if (!expr) {
        fprintf(stderr, KRED "Assert failed:\t");
        va_list args;
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        fprintf(stderr, "\nExpected: %s\n", expr_str);
        fprintf(stderr, "At Source: %s:%d\n", file, line);
        abort();
    }
}
