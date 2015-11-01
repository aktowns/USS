#include <stddef.h>
#include <string.h>
#include <gc/gc.h>

#include "support/file.h"

char *join_path(const char *path, int dirsep, const char *name) {
    size_t len = strlen(path);
    size_t namlen = strlen(name);

    char *buf = GC_MALLOC(len + namlen + (dirsep ? 1 : 0) + 1);

    if (!buf) {
        return 0;
    }

    memcpy(buf, path, (size_t) len);

    if (dirsep) {
        buf[len++] = '/';
    }

    memcpy(buf + len, name, namlen);
    buf[len + namlen] = '\0';
    return buf;
}
