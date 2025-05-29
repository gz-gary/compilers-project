#ifndef __IR_PARSE_H__
#define __IR_PARSE_H__

#include <stdio.h>
#include <stdbool.h>
#include "IR.h"
#include "macro.h"

extern void IR_yyrestart ( FILE *input_file );
extern int IR_yylex();
extern int IR_yyparse();
extern int IR_yyerror(const char *msg);

extern void IR_parse(const char *input_IR_path);

extern IR_var get_IR_var(const char *id);
extern IR_label get_IR_label(const char *id);

#endif // __IR_PARSE_H__