//
// Created by xhy on 2024/5/3.
//

#ifndef LEARNOPENGL_UTILS_H
#define LEARNOPENGL_UTILS_H

#define DEBUG

// #define FN (__builtin_strrchr(__FILE__, '\\') ? __builtin_strrchr(__FILE__, '\\') + 1 : __FILE__)

// #define LOGGER(...)
#define LOGGER(...) log("", __FUNCTION__, __LINE__, __VA_ARGS__)
#define ERROR(...) error_msg("", __FUNCTION__, __LINE__, __VA_ARGS__)

void log(const char *file_name, const char *function_name, size_t line, const char *fmt, ...);

void error_msg(const char *file_name, const char *function_name, size_t line, const char *fmt, ...);

#ifdef DEBUG
#define Assert(Expr, ...) M_Assert(#Expr, Expr, __FILE__, __LINE__, __VA_ARGS__)
#else
#define Assert(Expr, ...) assert(Expr);
#endif

void M_Assert(const char *expr_str, bool expr, const char *file, int line, const char *fmt, ...);

#endif  // LEARNOPENGL_UTILS_H
