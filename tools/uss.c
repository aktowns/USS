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
    }

    ussenv_del(e);
    return 0;
}
