#include "builtins/builtin_ord.h"
#include "uss.h"

ussval_t* builtin_gt(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM(">", a, 2);
    UASSERT_TYPE(">", a, 0, UVAL_NUM);
    UASSERT_TYPE(">", a, 1, UVAL_NUM);

    int r = (a->cell[0]->num > a->cell[1]->num);
    ussval_del(a);

    return ussval_new_num(r);
}

ussval_t* builtin_lt(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("<", a, 2);
    UASSERT_TYPE("<", a, 0, UVAL_NUM);
    UASSERT_TYPE("<", a, 1, UVAL_NUM);

    int r = (a->cell[0]->num < a->cell[1]->num);
    ussval_del(a);

    return ussval_new_num(r);
}

ussval_t* builtin_ge(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM(">=", a, 2);
    UASSERT_TYPE(">=", a, 0, UVAL_NUM);
    UASSERT_TYPE(">=", a, 1, UVAL_NUM);

    int r = (a->cell[0]->num >= a->cell[1]->num);
    ussval_del(a);

    return ussval_new_num(r);
}

ussval_t* builtin_le(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("<=", a, 2);
    UASSERT_TYPE("<=", a, 0, UVAL_NUM);
    UASSERT_TYPE("<=", a, 1, UVAL_NUM);

    int r = (a->cell[0]->num <= a->cell[1]->num);
    ussval_del(a);

    return ussval_new_num(r);
}
