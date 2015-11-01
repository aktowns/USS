#include "builtins/builtin_equality.h"
#include "uss.h"

ussval_t* builtin_eq(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("==", a, 2);
    int r = ussval_eq(a->cell[0], a->cell[1]);

    ussval_del(a);
    return ussval_new_num(r);
}

ussval_t* builtin_ne(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("!=", a, 2);
    int r = !ussval_eq(a->cell[0], a->cell[1]);

    ussval_del(a);
    return ussval_new_num(r);
}
