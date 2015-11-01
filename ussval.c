#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gc.h>

#include "ussval.h"
#include "uss.h"

ussval_t* ussval_new(int type) {
    ussval_t* v = GC_MALLOC(sizeof(ussval_t));
    v->type = type;

    return v;
}

ussval_t* ussval_new_num(int32_t x) {
    ussval_t* v = ussval_new(UVAL_NUM);

    v->num = x;
    return v;
}

ussval_t* ussval_new_err(char* fmt, ...) {
    ussval_t* v = ussval_new(UVAL_ERR);

    va_list va;
    va_start(va, fmt);
    v->err = GC_MALLOC(512);
    vsnprintf(v->err, 511, fmt, va);
    v->err = GC_REALLOC(v->err, strlen(v->err)+1);
    va_end(va);

    return v;
}

ussval_t* ussval_new_sym(char* s) {
    ussval_t* v = ussval_new(UVAL_SYM);

    v->sym = GC_MALLOC(strlen(s) + 1);
    strcpy(v->sym, s);

    return v;
}

ussval_t* ussval_new_str(char* s) {
    ussval_t* v = ussval_new(UVAL_STR);

    v->str = GC_MALLOC(strlen(s) + 1);
    strcpy(v->str, s);

    return v;
}

ussval_t* ussval_new_char(char c) {
    ussval_t* v = ussval_new(UVAL_CHAR);

    v->chr = c;
    return v;
}

ussval_t* ussval_new_builtin(ussbuiltin_t func) {
    ussval_t* v = ussval_new(UVAL_FUN);

    v->builtin = func;
    return v;
}

ussval_t* ussval_new_lambda(ussval_t* formals, ussval_t* body) {
    ussval_t* v = ussval_new(UVAL_FUN);

    v->builtin = NULL;
    v->env = ussenv_new();
    v->formals = formals;
    v->body = body;

    return v;
}

ussval_t* ussval_new_sexpr(void) {
    ussval_t* v = ussval_new(UVAL_SEXPR);

    v->count = 0;
    v->cell = NULL;
    return v;
}

ussval_t* ussval_new_qexpr(void) {
    ussval_t* v = ussval_new(UVAL_QEXPR);

    v->count = 0;
    v->cell = NULL;
    return v;
}

void ussval_del(ussval_t* v) {
    switch (v->type) {
        case UVAL_NUM:
        case UVAL_CHAR:
            break;
        case UVAL_FUN:
            if (!v->builtin) {
                ussenv_del(v->env);
                ussval_del(v->formals);
                ussval_del(v->body);
            }
            break;
        case UVAL_ERR:
            GC_FREE(v->err);
            break;
        case UVAL_SYM:
            GC_FREE(v->sym);
            break;
        case UVAL_STR:
            GC_FREE(v->str);
            break;
        case UVAL_QEXPR:
        case UVAL_SEXPR:
            for (int i = 0; i < v->count; i++) {
                ussval_del(v->cell[i]);
            }
            GC_FREE(v->cell);
            break;
        default:break;
    }

    GC_FREE(v);
}

ussval_t* lval_copy(ussval_t* v) {
    ussval_t* x = ussval_new(v->type);

    switch (v->type) {
        case UVAL_FUN:
            if (v->builtin) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->env = ussenv_copy(v->env);
                x->formals = lval_copy(v->formals);
                x->body = lval_copy(v->body);
            }
            break;
        case UVAL_NUM:
            x->num = v->num;
            break;
        case UVAL_CHAR:
            x->chr = v->chr;
            break;
        case UVAL_ERR:
            x->err = GC_MALLOC(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;
        case UVAL_SYM:
            x->sym = GC_MALLOC(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;
        case UVAL_STR:
            x->str = GC_MALLOC(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;
        case UVAL_SEXPR:
        case UVAL_QEXPR:
            x->count = v->count;
            x->cell = GC_MALLOC(sizeof(ussval_t *) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(v->cell[i]);
            }
            break;
        default:break;
    }
    return x;
}

ussval_t* ussval_add(ussval_t* v, ussval_t* x) {
    v->count++;
    v->cell = GC_REALLOC(v->cell, sizeof(ussval_t *) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

ussval_t* ussval_join(ussval_t* x, ussval_t* y) {
    for (int i = 0; i < y->count; i++) {
        x = ussval_add(x, y->cell[i]);
    }
    GC_FREE(y->cell);
    GC_FREE(y);
    return x;
}

ussval_t* ussval_pop(ussval_t* v, int i) {
    ussval_t *x = v->cell[i];
    memmove(&v->cell[i],
            &v->cell[i + 1], sizeof(ussval_t *) * (v->count - i - 1));
    v->count--;
    v->cell = GC_REALLOC(v->cell, sizeof(ussval_t *) * v->count);
    return x;
}

ussval_t* ussval_take(ussval_t* v, int i) {
    ussval_t *x = ussval_pop(v, i);
    ussval_del(v);
    return x;
}

void ussval_print_expr(ussval_t* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        ussval_print(v->cell[i]);
        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void ussval_print_str(ussval_t* v) {
    /* Make a Copy of the string */
    char *escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);
    /* Pass it through the escape function */
    escaped = mpcf_escape(escaped);
    /* Print it between " characters */
    printf("\"%s\"", escaped);

    free(escaped);
}

void ussval_print(ussval_t* v) {
    switch (v->type) {
        case UVAL_FUN:
            if (v->builtin) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                ussval_print(v->formals);
                putchar(' ');
                ussval_print(v->body);
                putchar(')');
            }
            break;
        case UVAL_NUM:
            printf("%i", v->num);
            break;
        case UVAL_CHAR:
            printf("\\%c", v->chr);
            break;
        case UVAL_ERR:
            printf("Error: %s", v->err);
            break;
        case UVAL_SYM:
            printf("%s", v->sym);
            break;
        case UVAL_STR:
            ussval_print_str(v);
            break;
        case UVAL_SEXPR:
            ussval_print_expr(v, '(', ')');
            break;
        case UVAL_QEXPR:
            if (v->count == 0) {
                printf("nil");
            } else {
                ussval_print_expr(v, '{', '}');
            }
            break;
        default:
            break;
    }
}

int ussval_eq(ussval_t* x, ussval_t* y) {
    if (x->type != y->type) {
        return 0;
    }

    switch (x->type) {
        case UVAL_NUM:
            return (x->num == y->num);
        case UVAL_CHAR:
            return (x->chr == y->chr);
        case UVAL_ERR:
            return (strcmp(x->err, y->err) == 0);
        case UVAL_SYM:
            return (strcmp(x->sym, y->sym) == 0);
        case UVAL_STR:
            return (strcmp(x->str, y->str) == 0);
        case UVAL_FUN:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return ussval_eq(x->formals, y->formals) && ussval_eq(x->body, y->body);
            }
        case UVAL_QEXPR:
        case UVAL_SEXPR:
            if (x->count != y->count) { return 0; }
            for (int i = 0; i < x->count; i++) {
                if (!ussval_eq(x->cell[i], y->cell[i])) { return 0; }
            }
            return 1;
        default:
            break;
    }
    return 0;
}

void ussval_println(ussval_t* v) {
    ussval_print(v);
    putchar('\n');
}

ussval_t* ussval_call(ussenv_t* e, ussval_t* f, ussval_t* a) {
    if (f->builtin) {
        return f->builtin(e, a);
    }

    size_t given = a->count;
    size_t total = f->formals->count;

    while (a->count) {

        if (f->formals->count == 0) {
            ussval_del(a);
            return ussval_new_err("Function passed too many arguments. Got %i, Expected %i.", given, total);
        }

        ussval_t *sym = ussval_pop(f->formals, 0);

        if (strcmp(sym->sym, "&") == 0) {

            if (f->formals->count != 1) {
                ussval_del(a);
                return ussval_new_err("Function format invalid. Symbol '&' not followed by single symbol.");
            }

            ussval_t *nsym = ussval_pop(f->formals, 0);
            ussenv_put(f->env, nsym, builtin_list(e, a));
            ussval_del(sym);
            ussval_del(nsym);
            break;
        }

        ussval_t *val = ussval_pop(a, 0);
        ussenv_put(f->env, sym, val);
        ussval_del(sym);
        ussval_del(val);
    }

    ussval_del(a);

    if (f->formals->count > 0 &&
        strcmp(f->formals->cell[0]->sym, "&") == 0) {

        if (f->formals->count != 2) {
            return ussval_new_err("Function format invalid. Symbol '&' not followed by single symbol.");
        }

        ussval_del(ussval_pop(f->formals, 0));

        ussval_t *sym = ussval_pop(f->formals, 0);
        ussval_t *val = ussval_new_qexpr();
        ussenv_put(f->env, sym, val);
        ussval_del(sym);
        ussval_del(val);
    }

    if (f->formals->count == 0) {
        f->env->par = e;
        return builtin_eval(f->env, ussval_add(ussval_new_sexpr(), lval_copy(f->body)));
    } else {
        return lval_copy(f);
    }
}

ussval_t* ussval_eval_sexpr(ussenv_t* e, ussval_t* v) {

    for (int i = 0; i < v->count; i++) {
        v->cell[i] = ussval_eval(e, v->cell[i]);
    }

    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == UVAL_ERR) {
            return ussval_take(v, i);
        }
    }

    if (v->count == 0) {
        return v;
    }
    if (v->count == 1) {
        return ussval_eval(e, ussval_take(v, 0));
    }

    ussval_t *f = ussval_pop(v, 0);
    if (f->type != UVAL_FUN) {
        ussval_t *err = ussval_new_err("S-Expression starts with incorrect type. Got %s, Expected %s.",
                             usstype_name(f->type), usstype_name(UVAL_FUN));
        ussval_del(f);
        ussval_del(v);
        return err;
    }

    ussval_t *result = ussval_call(e, f, v);
    ussval_del(f);
    return result;
}

ussval_t* ussval_eval(ussenv_t* e, ussval_t* v) {
    if (v->type == UVAL_SYM) {
        ussval_t *x = ussenv_get(e, v);
        ussval_del(v);
        return x;
    }
    if (v->type == UVAL_SEXPR) {
        return ussval_eval_sexpr(e, v);
    }
    return v;
}

ussval_t* ussval_read_num(mpc_ast_t* t) {
    errno = 0;
    int32_t x = (int32_t) strtol(t->contents, NULL, 10);
    return errno != ERANGE ? ussval_new_num(x) : ussval_new_err("Invalid Number.");
}

ussval_t* ussval_read_str(mpc_ast_t* t) {
    /* Cut off the final quote character */
    t->contents[strlen(t->contents)-1] = '\0';
    /* Copy the string missing out the first quote character */
    char* unescaped = malloc(strlen(t->contents+1)+1);
    strcpy(unescaped, t->contents+1);
    /* Pass through the unescape function */
    unescaped = mpcf_unescape(unescaped);
    /* Construct a new lval using the string */
    ussval_t* str = ussval_new_str(unescaped);
    /* Free the string and return */
    free(unescaped);
    return str;
}

ussval_t* ussval_read(mpc_ast_t* t) {
    ussval_t *x = NULL;

    if (strstr(t->tag, "number")) {
        return ussval_read_num(t);
    } else if (strstr(t->tag, "string")) {
        return ussval_read_str(t);
    } else if (strstr(t->tag, "qsymbol")) {
        x = ussval_new_qexpr();
        char contents[strlen(t->contents)];
        memcpy(contents, &t->contents[1], strlen(t->contents));
        return ussval_add(x, ussval_new_sym(contents));
    } else if (strstr(t->tag, "symbol")) {
        return ussval_new_sym(t->contents);
    } else if (strcmp(t->tag, ">") == 0) {
        x = ussval_new_sexpr();
    } else if (strstr(t->tag, "sexpr")) {
        x = ussval_new_sexpr();
    } else if (strstr(t->tag, "list")) {
        x = ussval_new_sexpr();
        x = ussval_add(x, ussval_new_sym("list"));
    } else if (strstr(t->tag, "qexpr")) {
        x = ussval_new_qexpr();
    }

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) {
            continue;
        } else if (strcmp(t->children[i]->contents, ")") == 0) {
            continue;
        } else if (strcmp(t->children[i]->contents, "}") == 0) {
            continue;
        } else if (strcmp(t->children[i]->contents, "{") == 0) {
            continue;
        } else if (strcmp(t->children[i]->contents, "[") == 0) {
            continue;
        } else if (strcmp(t->children[i]->contents, "]") == 0) {
            continue;
        } else if (strcmp(t->children[i]->tag, "regex") == 0) {
            continue;
        } else if (strstr(t->children[i]->tag, "comment")) {
            continue;
        }
        x = ussval_add(x, ussval_read(t->children[i]));
    }

    return x;
}

ussval_t* ussval_eval_from_file(ussenv_t* e, char* path) {
    ussval_t *args = ussval_add(ussval_new_sexpr(), ussval_new_str(path));

    /* Pass to builtin load and get the result */
    return builtin_load(e, args);
}

char* usstype_name(int t) {
    switch (t) {
        case UVAL_FUN:
            return "Function";
        case UVAL_NUM:
            return "Number";
        case UVAL_CHAR:
            return "Character";
        case UVAL_ERR:
            return "Error";
        case UVAL_SYM:
            return "Symbol";
        case UVAL_STR:
            return "String";
        case UVAL_SEXPR:
            return "S-Expression";
        case UVAL_QEXPR:
            return "Q-Expression";
        default:
            return "Unknown";
    }
}
