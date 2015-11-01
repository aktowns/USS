#ifndef USS_BUILTIN_STRING_H
#define USS_BUILTIN_STRING_H

#include "../ussval.h"

ussval_t* builtin_string_to_list(ussenv_t* e, ussval_t* a);
ussval_t* builtin_string_from_list(ussenv_t* e, ussval_t* a);

ussval_t* builtin_string_concat(ussenv_t* e, ussval_t* a);
ussval_t* builtin_string_slice(ussenv_t* e, ussval_t* a);
ussval_t* builtin_string_substr(ussenv_t* e, ussval_t* a);

#endif //USS_BUILTIN_STRING_H
