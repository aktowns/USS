#include <gc/gc.h>
#include <uss.h>
#include <histedit.h>
#include <stdbool.h>

#include "tools/ussi.h"

static int repl_line = 0;

char* prompt(__unused EditLine* e) {
    char *line = GC_MALLOC(1024);
    snprintf(line, 1024, "%i> ", repl_line);

    repl_line++;
    return line;
}

int main(int argc, char** argv) {
    GC_INIT();
    ussenv_t *e = ussenv_new();
    ussenv_common_builtins(e);

    char *prelude_path = join_path(STDLIB_PATH, true, "prelude.uss");

    ussval_t *prelude = ussval_eval_from_file(e, prelude_path);

    /* If the result is an error be sure to print it */
    if (prelude->type == UVAL_ERR) { ussval_println(prelude); }
    ussval_del(prelude);

    puts("USS Version 0.0.1");
    printf("boehm gc: heap size=%d bytes\n\n", (int) GC_get_heap_size());
    puts("Press Ctrl+c to Exit");

    EditLine *el;
    History *replHistory;
    int count;
    const char *line;
    int keepreading = 1;
    HistEvent ev;

    el = el_init(argv[0], stdin, stdout, stderr);
    el_set(el, EL_PROMPT, &prompt);
    el_set(el, EL_EDITOR, "emacs");
    replHistory = history_init();
    history(replHistory, &ev, H_SETSIZE, 800);
    el_set(el, EL_HIST, history, replHistory);

    while (keepreading) {
        line = el_gets(el, &count);
        if (count > 0) {
            history(replHistory, &ev, H_ENTER, line);

            parser_t *parser = parser_toplevel();
            mpc_result_t r;

            if (mpc_parse("<stdin>", line, parser->uss, &r)) {
                /* On success print and delete the AST */
                //mpc_ast_print(r.output);
                ussval_t *x = ussval_eval(e, ussval_read(r.output));
                ussval_println(x);
                ussval_del(x);
                mpc_ast_delete(r.output);
            } else {
                /* Otherwise print and delete the Error */
                mpc_err_print(r.error);
                mpc_err_delete(r.error);
            }

            parser_cleanup(parser);
        }
    }

    history_end(replHistory);
    el_end(el);

    ussenv_del(e);
    return 0;
}
