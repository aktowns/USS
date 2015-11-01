#ifndef USS_USS_H
#define USS_USS_H

#include "ussval.h"
#include "ussenv.h"
#include "parser.h"
#include "builtin.h"
#include "builtins/builtin_equality.h"
#include "builtins/builtin_ffi.h"
#include "builtins/builtin_list.h"
#include "builtins/builtin_math.h"
#include "builtins/builtin_ord.h"
#include "builtins/builtin_string.h"
#include "support/file.h"
#include "support/jenkins.h"
#include "support/uss_strings.h"

#define __unused __attribute__((unused))

#define UASSERT(args, cond, fmt, ...) \
  if (!(cond)) { ussval_t* err = ussval_new_err(fmt, ##__VA_ARGS__); ussval_del(args); return err; }

#define UASSERT_TYPE(func, args, index, expect) \
  UASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, usstype_name(args->cell[index]->type), usstype_name(expect))

#define UASSERT_NUM(func, args, num) \
  UASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)

#define UASSERT_NOT_EMPTY(func, args, index) \
  UASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

#define LVAL_NIL ussval_new_qexpr()


#endif //USS_USS_H
