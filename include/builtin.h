#ifndef THH_BUILTIN_H
#define THH_BUILTIN_H

#include "ussval.h"

ussval_t* builtin_lambda(ussenv_t* e, ussval_t* a);
ussval_t* builtin_var(ussenv_t* e, ussval_t* a, char* func);
ussval_t* builtin_def(ussenv_t* e, ussval_t* a);
ussval_t* builtin_put(ussenv_t* e, ussval_t* a);
ussval_t* builtin_if(ussenv_t* e, ussval_t* a);
ussval_t* builtin_load(ussenv_t* e, ussval_t* a);
ussval_t* builtin_print(ussenv_t* e, ussval_t* a);
ussval_t* builtin_error(ussenv_t* e, ussval_t* a);

ussval_t* builtin_path_join(ussenv_t* e, ussval_t* a);
void ussenv_common_builtins(ussenv_t* e);
#endif //THH_BUILTIN_H
