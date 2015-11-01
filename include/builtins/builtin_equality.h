#ifndef USS_BUILTIN_EQUALITY_H
#define USS_BUILTIN_EQUALITY_H

#include "../ussval.h"

ussval_t* builtin_eq(ussenv_t* e, ussval_t* a);
ussval_t* builtin_ne(ussenv_t* e, ussval_t* a);

#endif //USS_BUILTIN_EQUALITY_H
