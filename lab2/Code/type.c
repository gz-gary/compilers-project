#include "type.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

static int check_fields_equality(struct_field_t *a, struct_field_t *b) {
    if (a == NULL) return b == NULL;
    if (b == NULL) return a == NULL;
    return type_check_equality(a->type, b->type) && check_fields_equality(a->rest, b->rest);
}

int type_check_equality(type_t *a, type_t *b) {
    if (a->primitive != b->primitive) return 0;
    switch (a->primitive) {
    case PRIM_BASIC:
        return a->basic == b->basic;
    case PRIM_ARRAY:
        return type_check_equality(a->elem_type, b->elem_type);
    case PRIM_STRUCT:
        return check_fields_equality(a->struct_field, b->struct_field);
    case PRIM_FUNC:
        return 1; // TODO: check function equality
    }
}

type_t *type_new_basic_int() {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_BASIC;
    type->basic = PRIM_BASIC_INT;
    return type;
}

type_t *type_new_basic_float() {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_BASIC;
    type->basic = PRIM_BASIC_FLOAT;
    return type;
}

type_t *type_new_array(type_t *elem_type) {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_ARRAY;
    type->elem_type = elem_type;
    return type;
}

type_t *type_new_struct() {
    type_t *type = malloc(sizeof(type_t));
    type->primitive = PRIM_STRUCT;
    type->struct_field = NULL;
    return type;
}

void type_add_struct_field(type_t *s, type_t *type, const char *name) {
    assert(s->primitive == PRIM_STRUCT);
    struct_field_t *field = malloc(sizeof(struct_field_t));
    field->type = type;
    field->name = name;
    field->rest = s->struct_field;
    s->struct_field = field;
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
        struct_field_t *field = type->struct_field;
        while (field != NULL) {
            printf("%s: ", field->name);
            log_type(field->type);
            printf(" ");
            field = field->rest;
        }
        printf("}");
        break;
    case PRIM_FUNC:
        break;
    }
}
