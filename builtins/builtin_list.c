#include "builtins/builtin_list.h"
#include "uss.h"

ussval_t* builtin_list(__unused ussenv_t* e, ussval_t* a) {
    a->type = UVAL_QEXPR;
    return a;
}

ussval_t* builtin_head(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("head", a, 1);
    UASSERT_TYPE("head", a, 0, UVAL_QEXPR);
    UASSERT_NOT_EMPTY("head", a, 0);

    ussval_t *v = ussval_take(a, 0);
    while (v->count > 1) {
        ussval_del(ussval_pop(v, 1));
    }
    return v;
}

ussval_t* builtin_tail(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("tail", a, 1);
    UASSERT_TYPE("tail", a, 0, UVAL_QEXPR);
    UASSERT_NOT_EMPTY("tail", a, 0);

    ussval_t *v = ussval_take(a, 0);
    ussval_del(ussval_pop(v, 0));
    return v;
}

ussval_t* builtin_join(__unused ussenv_t* e, ussval_t* a) {
    for (int i = 0; i < a->count; i++) {
        UASSERT_TYPE("join", a, i, UVAL_QEXPR);
    }

    ussval_t *x = ussval_pop(a, 0);

    while (a->count) {
        ussval_t *y = ussval_pop(a, 0);
        x = ussval_join(x, y);
    }

    ussval_del(a);
    return x;
}

ussval_t* builtin_eval(ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("eval", a, 1);
    UASSERT_TYPE("eval", a, 0, UVAL_QEXPR);

    ussval_t* x = ussval_take(a, 0);
    x->type = UVAL_SEXPR;
    return ussval_eval(e, x);
}
