#ifndef USS_BUILTIN_ORD_H
#define USS_BUILTIN_ORD_H

#include "../ussval.h"

ussval_t* builtin_gt(ussenv_t* e, ussval_t* a);
ussval_t* builtin_lt(ussenv_t* e, ussval_t* a);
ussval_t* builtin_ge(ussenv_t* e, ussval_t* a);
ussval_t* builtin_le(ussenv_t* e, ussval_t* a);

#endif //USS_BUILTIN_ORD_H
