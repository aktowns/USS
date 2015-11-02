#ifndef THH_LVAL_H
#define THH_LVAL_H

#include "parser.h"

struct ussval_t;
struct ussenv_t;
typedef struct ussval_t ussval_t;
typedef struct ussenv_t ussenv_t;

enum { UVAL_ERR, UVAL_NUM, UVAL_SYM, UVAL_STR, UVAL_CHAR, UVAL_FUN, UVAL_SEXPR, UVAL_QEXPR };

typedef ussval_t*(*ussbuiltin_t)(ussenv_t*, ussval_t*);

struct ussval_t {
    int type;

    /* Basic */
    int32_t num;
    char* err;
    char* sym;
    char* str;
    char chr;

    /* Function */
    ussbuiltin_t builtin;
    ussenv_t* env;
    ussval_t* formals;
    ussval_t* body;

    /* Expression */
    size_t count;
    ussval_t** cell;
};

ussval_t* ussval_new_num(int32_t x);
ussval_t* ussval_new_err(char* fmt, ...);
ussval_t* ussval_new_sym(char* s);
ussval_t* ussval_new_str(char* s);
ussval_t* ussval_new_char(char c);
ussval_t* ussval_new_builtin(ussbuiltin_t func);
ussval_t* ussval_new_lambda(ussval_t* formals, ussval_t* body);
ussval_t* ussval_new_sexpr(void);
ussval_t* ussval_new_qexpr(void);

void ussval_del(ussval_t* v);
ussval_t* lval_copy(ussval_t* v);
ussval_t* ussval_add(ussval_t* v, ussval_t* x);
ussval_t* ussval_join(ussval_t* x, ussval_t* y);
ussval_t* ussval_pop(ussval_t* v, int i);
ussval_t* ussval_take(ussval_t* v, int i);
void ussval_print_expr(ussval_t* v, char open, char close);
void ussval_print_str(ussval_t* v);
void ussval_print(ussval_t* v);
void ussval_println(ussval_t* v);
int ussval_eq(ussval_t* x, ussval_t* y);
ussval_t* ussval_call(ussenv_t* e, ussval_t* f, ussval_t* a);
ussval_t* ussval_eval_sexpr(ussenv_t* e, ussval_t* v);
ussval_t* ussval_eval(ussenv_t* e, ussval_t* v);
ussval_t* ussval_read_num(mpc_ast_t* t);
ussval_t* ussval_read_char(mpc_ast_t* t);
ussval_t* ussval_read_str(mpc_ast_t* t);
ussval_t* ussval_read(mpc_ast_t* t);
ussval_t* ussval_eval_from_file(ussenv_t* e, char* path);
char* usstype_name(int t);
#endif //THH_LVAL_H

