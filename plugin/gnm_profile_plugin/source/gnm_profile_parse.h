#pragma once
/* Leitor minimo do JSON "gnm-profiles" v1 (sem alocacao, sem libc alem de string.h/stdlib.h). */
#include <stdlib.h>
#include <string.h>

/* Retorna o fps do titleId, ou -1 se ausente/invalido. */
static inline int gnm_profile_fps(const char* json, const char* titleId) {
    char key[40];
    size_t n = strlen(titleId);
    if (n == 0 || n > 12) return -1;
    key[0] = '"';
    memcpy(key + 1, titleId, n);
    key[n + 1] = '"';
    key[n + 2] = 0;
    const char* p = json;
    while ((p = strstr(p, "\"titleId\"")) != NULL) {
        const char* end = strchr(p, '}');
        if (!end) return -1;
        const char* id = strstr(p, key);
        if (id && id < end) {
            const char* f = strstr(p, "\"fps\"");
            if (!f || f > end) return -1;
            f = strchr(f, ':');
            if (!f || f > end) return -1;
            int v = atoi(f + 1);
            return (v >= 1 && v <= 240) ? v : -1;
        }
        p = end;
    }
    return -1;
}
