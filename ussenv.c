#include <gc.h>

#include "ussenv.h"

ussenv_t* ussenv_new(void) {
    ussenv_t* e = GC_MALLOC(sizeof(ussenv_t));
    e->par = NULL;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void ussenv_del(ussenv_t* e) {
    for (int i = 0; i < e->count; i++) {
        GC_FREE(e->syms[i]);
        ussval_del(e->vals[i]);
    }
    GC_FREE(e->syms);
    GC_FREE(e->vals);
    GC_FREE(e);
}

ussenv_t* ussenv_copy(ussenv_t* e) {
    ussenv_t* n = GC_MALLOC(sizeof(ussenv_t));
    n->par = e->par;
    n->count = e->count;
    n->syms = GC_MALLOC(sizeof(char*) * n->count);
    n->vals = GC_MALLOC(sizeof(ussval_t*) * n->count);
    for (int i = 0; i < e->count; i++) {
        n->syms[i] = GC_MALLOC(strlen(e->syms[i]) + 1);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = lval_copy(e->vals[i]);
    }
    return n;
}

ussval_t* ussenv_get(ussenv_t* e, ussval_t* k) {
    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            return lval_copy(e->vals[i]);
        }
    }

    if (e->par) {
        return ussenv_get(e->par, k);
    } else {
        return ussval_new_err("Unbound Symbol '%s'", k->sym);
    }
}

void ussenv_put(ussenv_t* e, ussval_t* k, ussval_t* v) {
    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            ussval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }

    e->count++;
    e->vals = GC_REALLOC(e->vals, sizeof(ussval_t*) * e->count);
    e->syms = GC_REALLOC(e->syms, sizeof(char*) * e->count);
    e->vals[e->count-1] = lval_copy(v);
    e->syms[e->count-1] = GC_MALLOC(strlen(k->sym)+1);
    strcpy(e->syms[e->count-1], k->sym);
}

void ussenv_def(ussenv_t* e, ussval_t* k, ussval_t* v) {
    while (e->par) { e = e->par; }
    ussenv_put(e, k, v);
}

void ussenv_add_builtin(ussenv_t* e, char* name, ussbuiltin_t func) {
    ussval_t* k = ussval_new_sym(name);
    ussval_t* v = ussval_new_builtin(func);
    ussenv_put(e, k, v);
    ussval_del(k);
    ussval_del(v);
}
