#ifndef PGEGEN_UTIL_H
#define PGEGEN_UTIL_H

typedef int (*cmpfun)(const void *, const void *);

int msort(void * data, size_t count, size_t size, cmpfun cmp);

#endif //PGEGEN_UTIL_H
