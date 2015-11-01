#include <ffi.h>
#include <dlfcn.h>
#include <gc.h>
#include <stdint.h>

#include "builtins/builtin_ffi.h"
#include "uss.h"
#include "ext/uthash.h"

static ussval_t* ffi_string;
static ussval_t* ffi_int32;
static ussval_t* ffi_unit;

struct library {
    uint32_t handle;
    void* libhandle;
    char* path;

    UT_hash_handle hh;
};

struct ffi_function_def {
    uint32_t handle;
    char* name;
    int32_t arity;
    ussval_t** args;
    ussval_t* return_type;
    int32_t library_handle;

    UT_hash_handle hh;
};

static struct library *loaded_libraries = NULL;
static struct ffi_function_def *defined_functions = NULL;

ussval_t* builtin_ffi_open(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("ffi-open", a, 1);
    UASSERT_TYPE("ffi-open", a, 0, UVAL_STR);

    ussval_t* library = ussval_pop(a, 0);

    struct library *lib = GC_MALLOC(sizeof(struct library));
    lib->handle = jenkins_hash(library->str, strlen(library->str));
    lib->libhandle = dlopen(library->str, RTLD_LAZY);
    lib->path = GC_MALLOC(strlen(library->str) + 1);
    strncpy(lib->path, library->str, strlen(library->str) + 1);

    if (!lib->handle) {
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        return LVAL_NIL;
    }

    HASH_ADD_INT(loaded_libraries, handle, lib);

    ussval_del(a);

    return ussval_new_num(lib->handle);
}


ffi_type* ffi_type_from_sym(ussval_t* a) {
    if (strcmp(a->sym, ffi_int32->sym) == 0) {
        return &ffi_type_sint32;
    } else if (strcmp(a->sym, ffi_string->sym) == 0) {
        return &ffi_type_pointer;
    } else if (strcmp(a->sym, ffi_unit->sym) == 0) {
        return &ffi_type_void;
    }

    fprintf(stderr, "error: didn't find a ffi-sym\n");
    return &ffi_type_void;
}

ffi_type** ffi_types_from_syms(ffi_type** args, ussval_t** a, int32_t len) {
    for (int i = 0; i < len; i++) {
        args[i] = ffi_type_from_sym(a[i]);
    }

    return args;
}

int uss_type_from_sym(ussval_t* a) {
    if (strcmp(a->sym, ffi_int32->sym) == 0) {
        return UVAL_NUM;
    } else if (strcmp(a->sym, ffi_string->sym) == 0) {
        return UVAL_STR;
    } else if (strcmp(a->sym, ffi_unit->sym) == 0) {
        return UVAL_QEXPR;
    }

    fprintf(stderr, "error: didn't find a ffi-sym\n");
    return UVAL_QEXPR;
}

int* uss_types_from_syms(int* args, ussval_t** a, int32_t len) {
    for (int i = 0; i < len; i++) {
        args[i] = uss_type_from_sym(a[i]);
    }

    return args;
}


ussval_t* builtin_ffi_call(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_TYPE("ffi-call", a, 0, UVAL_NUM);
    ussval_t* lval_handle = ussval_pop(a, 0);
    int32_t handle = lval_handle->num;

    //fprintf(stderr, "ffi_callee entered with handle: %i\n", handle);

    struct ffi_function_def *func_def;
    HASH_FIND_INT(defined_functions, &handle, func_def);

    UASSERT_NUM(func_def->name, a, func_def->arity);

    int arg_types[func_def->arity];
    uss_types_from_syms(arg_types, func_def->args, func_def->arity);

    ffi_type* ffi_types[func_def->arity];
    ffi_types_from_syms(ffi_types, func_def->args, func_def->arity);

    void* args[func_def->arity];
    for (int i = 0; i < func_def->arity; i++) {
        int type = arg_types[i];

        UASSERT_TYPE(func_def->name, a, i, type);

        ussval_t* func_arg = a->cell[i];
        if (type == UVAL_STR) {
            args[i] = &func_arg->str;
        } else if (type == UVAL_NUM) {
            args[i] = &func_arg->num;
        } else {
            fprintf(stderr, "error doing ffi-conversion %i is not implemented", type);
            return LVAL_NIL;
        }
    }

    struct library *lib = NULL;

    HASH_FIND_INT(loaded_libraries, &func_def->library_handle, lib);

    if (lib == NULL) {
        fprintf(stderr, "failed to find library with handle %i\n", func_def->library_handle);
        return LVAL_NIL;
    }

    void* func = dlsym(lib->libhandle, func_def->name);
    char* err = dlerror();
    if (err) {
        fprintf(stderr, "dlsym failed: %s\n", err);
        return LVAL_NIL;
    }

    ffi_type* return_type = ffi_type_from_sym(func_def->return_type);

    ffi_cif cif;
    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, (unsigned int) func_def->arity, return_type, ffi_types);
    if (status != FFI_OK) {
        fprintf(stderr, "ffi_prep_cif failed: %d\n", status);
        return LVAL_NIL;
    }

    void* return_val;

    ffi_call(&cif, FFI_FN(func), &return_val, args);

    int type = uss_type_from_sym(func_def->return_type);
    if (type == UVAL_NUM) {
        return ussval_new_num(((int32_t)return_val));
    } else if (type == UVAL_STR) {
        return ussval_new_str(return_val);
    }

    return LVAL_NIL;
}

ussval_t* builtin_ffi_def(__unused ussenv_t* e, ussval_t* a) {
    UASSERT_NUM("ffi-open", a, 4);
    UASSERT_TYPE("ffi-open", a, 0, UVAL_QEXPR);
    UASSERT_TYPE("ffi-open", a, 1, UVAL_QEXPR);
    UASSERT_TYPE("ffi-open", a, 2, UVAL_SYM);
    UASSERT_TYPE("ffi-open", a, 3, UVAL_NUM);

    ussval_t* func_def = ussval_pop(a, 0);
    ussval_t* func_args = ussval_pop(a, 0);
    ussval_t* func_return = ussval_pop(a, 0);
    ussval_t* library_handle = ussval_pop(a, 0);

    struct library *lib = NULL;

    HASH_FIND_INT(loaded_libraries, &library_handle->num, lib);

    if (lib == NULL) {
        fprintf(stderr, "failed to find library with handle %i\n", library_handle->num);
        return LVAL_NIL;
    }

    UASSERT_NUM("ffi-open-name", func_def, 1);
    UASSERT_TYPE("ffi-open-name", func_def, 0, UVAL_SYM);
    ussval_t* lval_func_name = ussval_pop(func_def, 0);
    char* func_name = lval_func_name->sym;

    struct ffi_function_def* function_def = GC_MALLOC(sizeof(struct ffi_function_def));
    function_def->name = func_name;
    function_def->arity = func_args->count;
    function_def->return_type = func_return;
    function_def->library_handle = library_handle->num;
    function_def->args = func_args->cell;
    function_def->handle = jenkins_hash(func_name, sizeof(func_name)) + library_handle->num;

    //fprintf(stderr, "registered %s with handle %i\n", func_name, function_def->handle);

    HASH_ADD_INT(defined_functions, handle, function_def);

    return ussval_new_num(function_def->handle);
}

void builtin_ffi_add_types(ussenv_t* e) {
    ffi_string = ussval_new_sym("ffi-string");
    ffi_int32 = ussval_new_sym("ffi-int32");
    ffi_unit = ussval_new_sym("ffi-unit");

    ussenv_put(e, ffi_string, ffi_string);
    ussenv_put(e, ffi_int32, ffi_int32);
    ussenv_put(e, ffi_unit, ffi_unit);
}
