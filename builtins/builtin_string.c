#include <gc/gc.h>

#include "builtins/builtin_string.h"
#include "uss.h"

ussval_t* builtin_string_to_list(ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("string-to-list", a, 1);
    UASSERT_TYPE("string-to-list", a, 0, UVAL_STR);

    ussval_t* str = ussval_pop(a, 0);

    return string_to_list(str);
}

ussval_t* builtin_string_from_list(ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("string-from-list", a, 1);
    UASSERT_TYPE("string-from-list", a, 0, UVAL_QEXPR);

    ussval_t* list = ussval_pop(a, 0);

    for (int i=0; i < list->count; i++) {
        UASSERT_TYPE("string-from-list", list, i, UVAL_CHAR);
    }

    return string_from_list(list);
}

ussval_t* builtin_string_concat(ussenv_t* e, ussval_t* a) {
    for (int i = 0; i < a->count; i++) {
        UASSERT_TYPE("++", a, i, UVAL_STR);
    }

    return string_append(a->count, a->cell);
}

ussval_t* builtin_string_slice(ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("string-slice", a, 3);
    UASSERT_TYPE("string-slice", a, 0, UVAL_NUM);
    UASSERT_TYPE("string-slice", a, 1, UVAL_NUM);
    UASSERT_TYPE("string-slice", a, 2, UVAL_STR);

    ussval_t* start = ussval_pop(a, 0);
    ussval_t* count = ussval_pop(a, 0);
    ussval_t* str = ussval_pop(a, 0);

    return string_slice(start, count, str);
}

ussval_t* builtin_string_substr(ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("string-substr", a, 2);
    UASSERT_TYPE("string-substr", a, 0, UVAL_NUM);
    UASSERT_TYPE("string-substr", a, 1, UVAL_STR);

    ussval_t* substr = ussval_pop(a, 0);
    ussval_t* str = ussval_pop(a, 0);

    return string_substr(substr, str);
}
