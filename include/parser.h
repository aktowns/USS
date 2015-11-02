#ifndef THH_PARSER_H
#define THH_PARSER_H

#include "../ext/mpc.h"

typedef struct {
    mpc_parser_t* number;
    mpc_parser_t* character;
    mpc_parser_t* symbol;
    mpc_parser_t* qsymbol;
    mpc_parser_t* typeann;
    mpc_parser_t* string;
    mpc_parser_t* comment;
    mpc_parser_t* list;
    mpc_parser_t* sexpr;
    mpc_parser_t* qexpr;
    mpc_parser_t* expr;
    mpc_parser_t* uss;
} parser_t;

parser_t* parser_toplevel();
void parser_cleanup(parser_t* parser);

#endif //THH_PARSER_H
