#include "type.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static int check_name_equality(const char *a, const char *b) {
    if (a == NULL || b == NULL) return 0;
    return !strcmp(a, b);
}

static int check_fields_equality(field_t *a, field_t *b) {
    if (a == NULL) return b == NULL;
    if (b == NULL) return a == NULL;
    return type_check_equality(a->type, b->type) && check_fields_equality(a->next_field, b->next_field);
}

field_t *type_query_struct_field(type_t *struct_type, const char *field_name) {
    field_t *field = struct_type->first_field;
    while (field != NULL) {
        if (!strcmp(field->name, field_name)) return field;
        field = field->next_field;
    }
    return NULL;
}

int type_check_equality(type_t *a, type_t *b)
{
    if (a->primitive != b->primitive) return 0;
    switch (a->primitive) {
    case PRIM_BASIC:
        return a->basic == b->basic;
    case PRIM_ARRAY:
        return type_check_equality(a->elem_type, b->elem_type);
    case PRIM_STRUCT:
        return check_name_equality(a->struct_name, b->struct_name);
    case PRIM_FUNC:
        return 0; // we will never check func-type equality
    }
}

int type_check_int(type_t *a) {
    return a->primitive == PRIM_BASIC && a->basic == PRIM_BASIC_INT;
}

type_t *type_new_basic_int() {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_BASIC;
    type->basic = PRIM_BASIC_INT;
    type->size_in_bytes = 4;
    return type;
}

type_t *type_new_basic_float() {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_BASIC;
    type->basic = PRIM_BASIC_FLOAT;
    type->size_in_bytes = 4;
    return type;
}

type_t *type_new_array(type_t *elem_type, int elem_amount) {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_ARRAY;
    type->elem_type = elem_type;
    type->size_in_bytes = elem_amount * elem_type->size_in_bytes;
    return type;
}

type_t *type_new_struct(const char *name) {
    type_t *type = malloc(sizeof(type_t));
    type->struct_name = name; // 匿名结构体为NULL
    type->primitive = PRIM_STRUCT;
    type->first_field = NULL;
    type->last_field = NULL;
    return type;
}

type_t *type_new_func(type_t *return_type) {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_FUNC;
    type->firstarg = NULL;
    type->lastarg = NULL;
    type->return_type = return_type;
    return type;
}

void type_add_struct_field(type_t *s, type_t *type, const char *name) {
    assert(s->primitive == PRIM_STRUCT);
    s->size_in_bytes += type->size_in_bytes;
    field_t *field = malloc(sizeof(field_t));
    field->type = type;
    field->name = name;
    if (s->first_field) {
        field->offset = s->last_field->offset + s->last_field->type->size_in_bytes;
        s->last_field->next_field = field;
        s->last_field = field;
    } else {
        field->offset = 0;
        s->first_field = field;
        s->last_field = s->first_field;
    }
}

void type_add_func_arg(type_t *f, type_t *type) {
    assert(f->primitive == PRIM_FUNC);
    arglist_t *newarg = malloc(sizeof(arglist_t));
    newarg->type = type;
    newarg->nextarg = NULL;
    if (f->firstarg) {
        f->lastarg->nextarg = newarg;
        f->lastarg = newarg;
    } else {
        f->firstarg = newarg;
        f->lastarg = f->firstarg;
    }
}

type_t *type_new_invalid() {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_INVALID;
    return type;
}

void log_type(type_t *type) {
    switch (type->primitive) {
    case PRIM_INVALID:
        printf("invalid");
        break;
    case PRIM_BASIC:
        printf(type->basic == PRIM_BASIC_INT ? "int" : "float");
        break;
    case PRIM_ARRAY:
        log_type(type->elem_type);
        printf("[]");
        break;
    case PRIM_STRUCT:
        printf("struct{");
        field_t *field = type->first_field;
        while (field != NULL) {
            printf("%s: ", field->name);
            log_type(field->type);
            printf(" ");
            field = field->next_field;
        }
        printf("}");
        break;
    case PRIM_FUNC:
        log_type(type->return_type);
        printf("(");
        arglist_t *arg = type->firstarg;
        while (arg != NULL) {
            log_type(arg->type);
            printf(", ");
            arg = arg->nextarg;
        }
        printf(")");
        break;
    }
}
