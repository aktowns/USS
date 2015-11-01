#ifndef USS_USS_STRINGS_H
#define USS_USS_STRINGS_H

#include "../ussval.h"

ussval_t* string_to_list(ussval_t* str);
ussval_t* string_from_list(ussval_t* list);

ussval_t* string_append(int count, ussval_t** vals);
ussval_t* string_slice(ussval_t* s, ussval_t* c, ussval_t* str);
ussval_t* string_substr(ussval_t* n, ussval_t* str);

#endif //USS_USS_STRINGS_H
