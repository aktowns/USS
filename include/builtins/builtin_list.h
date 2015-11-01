#ifndef USS_BUILTIN_LIST_H
#define USS_BUILTIN_LIST_H

#include "../ussval.h"

ussval_t* builtin_list(ussenv_t* e, ussval_t* a);
ussval_t* builtin_head(ussenv_t* e, ussval_t* a);
ussval_t* builtin_tail(ussenv_t* e, ussval_t* a);
ussval_t* builtin_join(ussenv_t* e, ussval_t* a);
ussval_t* builtin_eval(ussenv_t* e, ussval_t* a);

#endif //USS_BUILTIN_LIST_H
