#ifndef THH_LENV_H
#define THH_LENV_H

#include "ussval.h"

struct ussenv_t {
    ussenv_t* par;
    int count;
    char** syms;
    ussval_t** vals;
};

ussenv_t* ussenv_new(void);
void ussenv_del(ussenv_t* e);
ussenv_t* ussenv_copy(ussenv_t* e);
ussval_t* ussenv_get(ussenv_t* e, ussval_t* k);
void ussenv_put(ussenv_t* e, ussval_t* k, ussval_t* v);
void ussenv_def(ussenv_t* e, ussval_t* k, ussval_t* v);
void ussenv_add_builtin(ussenv_t* e, char* name, ussbuiltin_t func);

#endif //THH_LENV_H
