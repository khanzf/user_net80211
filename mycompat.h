#ifndef COMPAT_H
#define COMPAT_H

/* ==================== Platform Detection ==================== */
#if defined(__FreeBSD__)
#  include <sys/endian.h>
#  define OS_FREEBSD 1
#  define OS_LINUX   0
#elif defined(__linux__)
#  define _GNU_SOURCE
#  include <endian.h>
#  define OS_FREEBSD 0
#  define OS_LINUX   1
#else
#  error "Unsupported OS"
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* ==================== Common Kernel Macros ==================== */
#ifndef __packed
#define __packed        __attribute__((__packed__))
#endif

#ifndef __aligned
#define __aligned(x)    __attribute__((aligned(x)))
#endif

#ifndef __predict_true
#define __predict_true(x)   __builtin_expect(!!(x), 1)
#endif

#ifndef __predict_false
#define __predict_false(x)  __builtin_expect(!!(x), 0)
#endif

/* ==================== Endianness ==================== */
#if OS_FREEBSD
#  include <sys/endian.h>
#else
#  include <endian.h>
#endif

/* Ensure all common macros exist on both platforms */
#ifndef htole16
#define htole16(x)  htole16(x)
#endif
#ifndef le16toh
#define le16toh(x)  le16toh(x)
#endif

#ifndef htole32
#define htole32(x)  htole32(x)
#endif
#ifndef le32toh
#define le32toh(x)  le32toh(x)
#endif

/* OpenBSD-style compatibility */
#define letoh16(x)  le16toh(x)
#define letoh32(x)  le32toh(x)

/* ==================== Other helpers ==================== */
#define howmany(x, y)   (((x) + ((y) - 1)) / (y))
#define nitems(x)       (sizeof(x) / sizeof((x)[0]))

#ifndef MIN
#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)   ((a) > (b) ? (a) : (b))
#endif

#define KASSERT(x) do { \
    if (!(x)) { \
        printf("KASSERT failed: %s\n", #x); \
        exit(1); \
    } \
} while(0)

#endif /* COMPAT_H */