#ifndef TYPE_H
#define TYPE_H

#define type_func_noarg(func) (((func)->firstarg) == NULL)

typedef struct field_t field_t;
typedef struct arglist_t arglist_t;
typedef struct type_t type_t;

struct field_t {
    type_t *type;
    const char *name;
    field_t *next_field;
    int offset;
};

struct arglist_t {
    type_t *type;
    arglist_t *nextarg;
};

struct type_t {
    enum type_primitives_t { PRIM_BASIC, PRIM_ARRAY, PRIM_STRUCT, PRIM_FUNC, PRIM_INVALID } primitive;
    union {
        enum prim_basic_t { PRIM_BASIC_INT,  PRIM_BASIC_FLOAT } basic;

        type_t *elem_type;

        struct {
            const char *struct_name; // 实现名等价
            field_t *first_field;
            field_t *last_field;
        };

        struct {
            arglist_t *firstarg;
            arglist_t *lastarg;
            type_t *return_type;
        };
    };
    int size_in_bytes;
};

field_t* type_query_struct_field(type_t *struct_type, const char *field_name);
int type_check_equality(type_t *a, type_t *b);
// AND OR ... 两边表达式要求为INT类型
int type_check_int(type_t *a);
type_t* type_new_basic_int();
type_t* type_new_basic_float();
type_t* type_new_array(type_t *elem_type, int elem_amount);
type_t* type_new_struct(const char *name);
type_t* type_new_func(type_t *return_type);
type_t* type_new_invalid();
void type_add_struct_field(type_t *s, type_t *type, const char *name);
void type_add_func_arg(type_t *f, type_t *type);
void log_type(type_t *type);

#endif