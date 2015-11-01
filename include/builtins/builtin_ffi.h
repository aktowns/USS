#ifndef THH_BUILTIN_FFI_H
#define THH_BUILTIN_FFI_H

#include "../ussval.h"

ussval_t* builtin_ffi_open(ussenv_t* e, ussval_t* a);
ussval_t* builtin_ffi_call(ussenv_t* e, ussval_t* a);
ussval_t* builtin_ffi_def(ussenv_t* e, ussval_t* a);

void builtin_ffi_add_types(ussenv_t* e);

#endif //THH_BUILTIN_FFI_H
