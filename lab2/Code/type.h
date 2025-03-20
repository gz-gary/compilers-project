#ifndef TYPE_H
#define TYPE_H

typedef struct struct_field_t struct_field_t;
typedef struct arglist_t arglist_t;
typedef struct type_t type_t;

struct struct_field_t {
    type_t *type;
    const char *name;
    struct_field_t *rest;
};

struct arglist_t {
    type_t *type;
    arglist_t *rest;
};

struct type_t {
    enum type_primitives_t { PRIM_BASIC, PRIM_ARRAY, PRIM_STRUCT, PRIM_FUNC, PRIM_INVALID } primitive;
    union {
        enum prim_basic_t { PRIM_BASIC_INT,  PRIM_BASIC_FLOAT } basic;

        type_t *elem_type;

        struct_field_t *struct_field;

        struct {
            arglist_t *arglist;
            type_t *return_type;
        };
    };
};

int type_check_arglist_equality(arglist_t *a, arglist_t *b);
int type_check_equality(type_t *a, type_t *b);
type_t* type_new_basic_int();
type_t* type_new_basic_float();
type_t* type_new_array(type_t *elem_type);
type_t* type_new_struct();
type_t* type_new_func(type_t *return_type);
type_t* type_new_invalid();
void type_add_struct_field(type_t *s, type_t *type, const char *name);
void type_add_func_arg(type_t *f, type_t *type, const char *name);
void log_type(type_t *type);

#endif