#include <stdbool.h>
#include <gc.h>

#include <uss.h>

#define STDLIB_PATH "/home/ashleyis/Development/projects/thh/stdlib"

int main(int argc, char** argv) {
    GC_INIT();
    ussenv_t *e = ussenv_new();
    ussenv_common_builtins(e);

    char *prelude_path = join_path(STDLIB_PATH, true, "prelude.uss");

    ussval_t *prelude = ussval_eval_from_file(e, prelude_path);

    /* If the result is an error be sure to print it */
    if (prelude->type == UVAL_ERR) { ussval_println(prelude); }
    ussval_del(prelude);

    fprintf(stderr, "argv[0]=%s argv[1]=%s\n", argv[0], argv[1]);

    if (argc >= 2) {
        /* loop over each supplied filename (starting from 1) */
        for (int i = 1; i < argc; i++) {
            ussval_t *x = ussval_eval_from_file(e, argv[i]);

            /* If the result is an error be sure to print it */
            if (x->type == UVAL_ERR) {
                ussval_println(x);
            }

            ussval_del(x);
        }
    } else {
        char buf[4096];

        parser_t *parser = parser_toplevel();
        mpc_result_t r;

        ussval_t *x = UVAL_NIL;
        while(fgets(buf, sizeof(buf), stdin)) {
            if (mpc_parse("<stdin>", buf, parser->uss, &r)) {
                // mpc_ast_print(r.output);
                x = ussval_eval(e, ussval_read(r.output));
                mpc_ast_delete(r.output);
            } else {
                mpc_err_print(r.error);
                mpc_err_delete(r.error);
            }
        }

        ussval_println(x);
        parser_cleanup(parser);
    }

    ussenv_del(e);
    return 0;
}
