#include <parser.h>
#include "tools/usslint.h"
#include "../ext/mpc.h"

int main(int argc, char** argv) {
    mpc_result_t r;
    parser_t *parser = parser_toplevel();

    if (argc <= 1) {
        fprintf(stderr, "FILE EXPECTED\n");
    } else {
        if (mpc_parse_contents(argv[1], parser->uss, &r)) {
            mpc_ast_delete(r.output);

            fprintf(stderr, "LINT OK\n");
        } else {
            char *err_msg = mpc_err_string(r.error);
            mpc_err_delete(r.error);

            fprintf(stderr, "LINT FAIL: %s\n", err_msg);
        }
    }

    parser_cleanup(parser);
    return 0;
}
