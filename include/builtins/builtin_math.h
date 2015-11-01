#ifndef USS_BUILTIN_MATH_H
#define USS_BUILTIN_MATH_H

#include "../ussval.h"

ussval_t* builtin_add(ussenv_t* e, ussval_t* a);
ussval_t* builtin_sub(ussenv_t* e, ussval_t* a);
ussval_t* builtin_mul(ussenv_t* e, ussval_t* a);
ussval_t* builtin_div(ussenv_t* e, ussval_t* a);

#endif //USS_BUILTIN_MATH_H
