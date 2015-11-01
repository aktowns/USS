#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdbool.h>

#include "builtin.h"
#include "uss.h"

ussval_t* builtin_lambda(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("\\", a, 2);
    UASSERT_TYPE("\\", a, 0, UVAL_QEXPR);
    UASSERT_TYPE("\\", a, 1, UVAL_QEXPR);

    for (int i = 0; i < a->cell[0]->count; i++) {
        UASSERT(a, (a->cell[0]->cell[i]->type == UVAL_SYM), "Cannot define non-symbol. Got %s, Expected %s.",
                usstype_name(a->cell[0]->cell[i]->type), usstype_name(UVAL_SYM));
    }

    ussval_t* formals = ussval_pop(a, 0);
    ussval_t* body = ussval_pop(a, 0);
    ussval_del(a);

    return ussval_new_lambda(formals, body);
}

ussval_t* builtin_var(ussenv_t* e, ussval_t* a, char* func) {
    UASSERT_TYPE(func, a, 0, UVAL_QEXPR);

    ussval_t* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        UASSERT(a, (syms->cell[i]->type == UVAL_SYM), "Function '%s' cannot define non-symbol. Got %s, Expected %s.",
                func, usstype_name(syms->cell[i]->type), usstype_name(UVAL_SYM));
    }

    UASSERT(a, (syms->count == a->count-1), "Function '%s' passed too many arguments for symbols. Got %i, Expected %i.",
            func, syms->count, a->count-1);

    for (int i = 0; i < syms->count; i++) {
        if (strcmp(func, "def") == 0) {
            ussenv_def(e, syms->cell[i], a->cell[i + 1]);
        } else if (strcmp(func, "=") == 0) {
            ussenv_put(e, syms->cell[i], a->cell[i + 1]);
        }
    }

    ussval_del(a);
    return ussval_new_sexpr();
}

ussval_t* builtin_path_join(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("path-join", a, 2);
    UASSERT_TYPE("path-join", a, 0, UVAL_STR);
    UASSERT_TYPE("path-join", a, 1, UVAL_STR);

    ussval_t* l = ussval_pop(a, 0);
    ussval_t* r = ussval_pop(a, 0);

    return ussval_new_str(join_path(l->str, true, r->str));
}

ussval_t* builtin_def(ussenv_t* e, ussval_t* a) {
    return builtin_var(e, a, "def");
}

ussval_t* builtin_put(ussenv_t* e, ussval_t* a) {
    return builtin_var(e, a, "=");
}

ussval_t* builtin_if(ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("if", a, 3);
    UASSERT_TYPE("if", a, 0, UVAL_NUM);
    UASSERT_TYPE("if", a, 1, UVAL_QEXPR);
    UASSERT_TYPE("if", a, 2, UVAL_QEXPR);

    ussval_t* x;
    a->cell[1]->type = UVAL_SEXPR;
    a->cell[2]->type = UVAL_SEXPR;

    if (a->cell[0]->num) {
        x = ussval_eval(e, ussval_pop(a, 1));
    } else {
        x = ussval_eval(e, ussval_pop(a, 2));
    }

    ussval_del(a);
    return x;
}

ussval_t* builtin_load(ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("load", a, 1);
    UASSERT_TYPE("load", a, 0, UVAL_STR);

    /* Parse File given by string name */
    mpc_result_t r;
    parser_t *parser = parser_toplevel();

    if (mpc_parse_contents(a->cell[0]->str, parser->uss, &r)) {
        puts(a->cell[0]->str);
        ussenv_put(e, ussval_new_sym("file-path"), a->cell[0]);
        ussenv_put(e, ussval_new_sym("file-dir"), ussval_new_str(dirname(a->cell[0]->str)));

        /* Read contents */
        ussval_t *expr = ussval_read(r.output);
        mpc_ast_delete(r.output);

        /* Evaluate each Expression */
        while (expr->count) {
            ussval_t *x = ussval_eval(e, ussval_pop(expr, 0));
            /* If Evaluation leads to error print it */
            if (x->type == UVAL_ERR) {
                ussval_println(x);
            }
            ussval_del(x);
        }

        /* Delete expressions and arguments */
        ussval_del(expr);
        ussval_del(a);

        parser_cleanup(parser);

        //lenv_put(e, lval_sym("file-path"), LVAL_NIL);
        //lenv_put(e, lval_sym("file-dir"), LVAL_NIL);

        /* Return empty list */
        return ussval_new_sexpr();

    } else {
        /* Get Parse Error as String */
        char *err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        /* Create new error message using it */
        ussval_t *err = ussval_new_err("Could not load Library %s", err_msg);
        free(err_msg);
        ussval_del(a);

        parser_cleanup(parser);
        /* Cleanup and return error */
        return err;
    }
}

ussval_t* builtin_print(__unused ussenv_t* e, ussval_t* a) {
    /* Print each argument followed by a space */
    for (int i = 0; i < a->count; i++) {
        ussval_print(a->cell[i]); putchar(' ');
    }

    /* Print a newline and delete arguments */
    putchar('\n');
    ussval_del(a);

    return ussval_new_sexpr();
}

ussval_t* builtin_error(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("error", a, 1);
    UASSERT_TYPE("error", a, 0, UVAL_STR);

    /* Construct Error from first argument */
    ussval_t* err = ussval_new_err(a->cell[0]->str);

    /* Delete arguments and return */
    ussval_del(a);
    return err;
}

void ussenv_common_builtins(ussenv_t* e) {
    /* Variable Functions */
    ussenv_add_builtin(e, "\\",  builtin_lambda);
    ussenv_add_builtin(e, "def", builtin_def);
    ussenv_add_builtin(e, "=",   builtin_put);

    /* List Functions */
    ussenv_add_builtin(e, "list", builtin_list);
    ussenv_add_builtin(e, "head", builtin_head);
    ussenv_add_builtin(e, "tail", builtin_tail);
    ussenv_add_builtin(e, "eval", builtin_eval);
    ussenv_add_builtin(e, "join", builtin_join);

    /* Mathematical Functions */
    ussenv_add_builtin(e, "+", builtin_add);
    ussenv_add_builtin(e, "-", builtin_sub);
    ussenv_add_builtin(e, "*", builtin_mul);
    ussenv_add_builtin(e, "/", builtin_div);

    /* Comparison Functions */
    ussenv_add_builtin(e, "if", builtin_if);
    ussenv_add_builtin(e, "==", builtin_eq);
    ussenv_add_builtin(e, "!=", builtin_ne);
    ussenv_add_builtin(e, ">",  builtin_gt);
    ussenv_add_builtin(e, "<",  builtin_lt);
    ussenv_add_builtin(e, ">=", builtin_ge);
    ussenv_add_builtin(e, "<=", builtin_le);

    /* String Functions */
    ussenv_add_builtin(e, "load",  builtin_load);
    ussenv_add_builtin(e, "error", builtin_error);
    ussenv_add_builtin(e, "print", builtin_print);

    ussenv_add_builtin(e, "path-join", builtin_path_join);

    //lenv_add_builtin(e, "++", builtin_string_concat);
    //lenv_add_builtin(e, "string-concat", builtin_string_concat);
    ussenv_add_builtin(e, "string-slice", builtin_string_slice);
    ussenv_add_builtin(e, "string-substr", builtin_string_substr);
    ussenv_add_builtin(e, "string-to-list", builtin_string_to_list);
    ussenv_add_builtin(e, "string-from-list", builtin_string_from_list);

    /* FFI */
    builtin_ffi_add_types(e);
    ussenv_add_builtin(e, "ffi-open", builtin_ffi_open);
    ussenv_add_builtin(e, "ffi-def", builtin_ffi_def);
    ussenv_add_builtin(e, "ffi-call", builtin_ffi_call);
}
