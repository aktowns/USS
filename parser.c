#include <parser.h>
#include "parser.h"

const char* grammar =
    "                                                              \
        number    : /-?[0-9]+/ ;                                   \
        character : /\\$(.)/ ;                                     \
        symbol    : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;             \
        qsymbol   : /'[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;            \
        typeann   : <symbol> ':' /[a-zA-Z0-9-]+/ ;                 \
        string    : /\"(\\\\.|[^\"])*\"/ ;                         \
        comment   : /(;|#!)[^\\r\\n]*/ ;                           \
        list      : '[' <expr>* ']' ;                              \
        sexpr     : '(' <expr>* ')' ;                              \
        qexpr     : /({|\\|)/ <expr>* /(}|\\|)/' ;                 \
        expr      : <number>  | <character> | <symbol> | <typeann> \
                  | <qsymbol> | <string> | <comment> | <list>      \
                  | <sexpr>  | <qexpr>;                            \
        uss   : /^/ <expr>* /$/ ;                                  \
    ";

parser_t* parser_toplevel() {
    parser_t *parser  = malloc(sizeof(parser_t));
    parser->number    = mpc_new("number");
    parser->character = mpc_new("character");
    parser->symbol    = mpc_new("symbol");
    parser->qsymbol   = mpc_new("qsymbol");
    parser->typeann   = mpc_new("typeann");
    parser->string    = mpc_new("string");
    parser->comment   = mpc_new("comment");
    parser->list      = mpc_new("list");
    parser->sexpr     = mpc_new("sexpr");
    parser->qexpr     = mpc_new("qexpr");
    parser->expr      = mpc_new("expr");
    parser->uss       = mpc_new("uss");

    mpca_lang(MPCA_LANG_DEFAULT, grammar, parser->number, parser->character, parser->symbol, parser->qsymbol, parser->typeann,
              parser->string, parser->comment, parser->list, parser->sexpr, parser->qexpr, parser->expr, parser->uss);

    return parser;
}

void parser_cleanup(parser_t* parser) {
    mpc_cleanup(12, parser->number, parser->character, parser->symbol, parser->qsymbol, parser->typeann, parser->string, parser->comment,
                parser->list, parser->sexpr, parser->qexpr, parser->expr, parser->uss);
    free(parser);
}
