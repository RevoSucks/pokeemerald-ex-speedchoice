#ifndef PGEGEN_GLOBAL_H
#define PGEGEN_GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#ifdef _MSC_VER

#define FATAL_ERROR(format, ...)          \
do                                        \
{                                         \
    fprintf(stderr, "FATAL: "format, __VA_ARGS__); \
    exit(1);                              \
} while (0)

#else

#define FATAL_ERROR(format, ...)            \
do                                          \
{                                           \
    fprintf(stderr, "FATAL: "format, ##__VA_ARGS__); \
    exit(1);                                \
} while (0)

#endif // _MSC_VER

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))

#endif //PGEGEN_GLOBAL_H
