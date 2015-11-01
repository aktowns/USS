#include <gc/gc.h>

#include "support/uss_strings.h"

ussval_t* string_to_list(ussval_t* str) {
    size_t len = strlen(str->str);

    ussval_t* list = ussval_new_qexpr();
    for (int i = 0; i < len; i ++) {
        ussval_add(list, ussval_new_char(str->str[i]));
    }

    return list;
}

ussval_t* string_from_list(ussval_t* list) {
    char* str = GC_MALLOC(list->count + 1);

    for (int i=0; i < list->count; i++) {
        str[i] = list->cell[i]->chr;
    }
    str[list->count] = '\0';

    return ussval_new_str(str);
}

char* string_append_multi(int count, char** strings) {
    size_t len = 0;

    for (int i = 0; i < count; i++) {
        len += strlen(strings[i]) - 1;
    }

    char* ret_string = GC_MALLOC(len);

    size_t offset=0;
    for (int i = 0; i < count; i++) {
        offset += (i == 0) ? 0 : (strlen(strings[i]) + 1);
        strncat(ret_string, strings[i], strlen(strings[i]));
    }

    return ret_string;
}

ussval_t* string_append(int count, ussval_t** vals) {
    char* str[count];
    for (int i = 0; i < count; i++) {
        str[i] = vals[i]->str;
    }

    char* ret_char = string_append_multi(count, str);

    return ussval_new_str(ret_char);
}

ussval_t* string_slice(ussval_t* s, ussval_t* c, ussval_t* str) {
    int32_t string_start = s->num;
    int32_t string_count = c->num;
    size_t string_length = strlen(str->str);

    if (string_start > string_length) {
        return ussval_new_err("string substring start %i is > string length %i", string_start, string_length);
    } else if (string_count <= 0) {
        return ussval_new_err("string substring %i is <= 0", string_count);
    } else if (string_count > string_length) {
        return ussval_new_err("string substring is out of bounds %i > %i", string_count, string_length);
    }

    char* new_str = GC_MALLOC((size_t) string_count);
    memcpy(new_str, str->str+string_start, (size_t) string_count);
    new_str[string_count] = '\0';

    return ussval_new_str(new_str);
}

ussval_t* string_substr(ussval_t* n, ussval_t* str) {
    int32_t string_start = n->num;
    size_t string_length = strlen(str->str);

    return string_slice(n, ussval_new_num(string_length - string_start), str);;
}
