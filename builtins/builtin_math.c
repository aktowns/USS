#include "builtins/builtin_math.h"
#include "uss.h"

ussval_t *builtin_add(__unused ussenv_t *e, ussval_t *a) {
    for (int i = 0; i < a->count; i++) {
        UASSERT_TYPE("+", a, i, UVAL_NUM);
    }

    ussval_t *x = ussval_pop(a, 0);
    while (a->count > 0) {
        ussval_t *y = ussval_pop(a, 0);
        x->num += y->num;

        ussval_del(y);
    }

    ussval_del(a);
    return x;
}

ussval_t *builtin_sub(__unused ussenv_t *e, ussval_t *a) {
    for (int i = 0; i < a->count; i++) {
        UASSERT_TYPE("-", a, i, UVAL_NUM);
    }

    ussval_t *x = ussval_pop(a, 0);
    if (a->count == 0) {
        x->num = -x->num;
    }

    while (a->count > 0) {
        ussval_t *y = ussval_pop(a, 0);
        x->num -= y->num;

        ussval_del(y);
    }

    ussval_del(a);
    return x;
}

ussval_t *builtin_mul(__unused ussenv_t *e, ussval_t *a) {
    for (int i = 0; i < a->count; i++) {
        UASSERT_TYPE("*", a, i, UVAL_NUM);
    }

    ussval_t *x = ussval_pop(a, 0);
    while (a->count > 0) {
        ussval_t *y = ussval_pop(a, 0);
        x->num *= y->num;

        ussval_del(y);
    }

    ussval_del(a);
    return x;
}

ussval_t *builtin_div(__unused ussenv_t *e, ussval_t *a) {
    for (int i = 0; i < a->count; i++) {
        UASSERT_TYPE("/", a, i, UVAL_NUM);
    }

    ussval_t *x = ussval_pop(a, 0);
    while (a->count > 0) {
        ussval_t *y = ussval_pop(a, 0);

        if (y->num == 0) {
            ussval_del(x);
            ussval_del(y);
            x = ussval_new_err("Division By Zero.");
            break;
        }
        x->num /= y->num;

        ussval_del(y);
    }

    ussval_del(a);
    return x;
}
