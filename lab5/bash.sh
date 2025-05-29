gcc -o parser main.o treap.o IR_function_append_sentence.o IR_symbol_allocator.o IR_function_build_graph.o IR_display.o -lfl -ly

gcc -o parser ./IR_function_append_sentence.o ./IR_symbol_allocator.o ./IR_function_build_graph.o ./treap.o ./IR_display.o ./IR_parse.o ./IR.o ./main.o ./syntax.tab.o ./IR_stmt.o -lfl -ly

gcc -o parser main.o treap.o IR_function_append_sentence.o IR_symbol_allocator.o IR_function_build_graph.o IR_display.o IR.o IR_parse.o IR_lex.yy.o IR_syntax.tab.o -lfl -ly


echo "+ CC" main.c
mkdir -p build/obj/./src/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/main.o src/main.c
echo "+ CC" treap.c
mkdir -p build/obj/./src/container/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/container/treap.o src/container/treap.c
echo "+ CC" IR_function_append_sentence.c
mkdir -p build/obj/./src/IR/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR/IR_function_append_sentence.o src/IR/IR_function_append_sentence.c
echo "+ CC" IR_symbol_allocator.c
mkdir -p build/obj/./src/IR/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR/IR_symbol_allocator.o src/IR/IR_symbol_allocator.c
echo "+ CC" IR_function_build_graph.c
mkdir -p build/obj/./src/IR/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR/IR_function_build_graph.o src/IR/IR_function_build_graph.c
echo "+ CC" IR_display.c
mkdir -p build/obj/./src/IR/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR/IR_display.o src/IR/IR_display.c
echo "+ CC" IR.c
mkdir -p build/obj/./src/IR/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR/IR.o src/IR/IR.c
echo "+ CC" IR_stmt.c
mkdir -p build/obj/./src/IR/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR/IR_stmt.o src/IR/IR_stmt.c
echo "+ CC" IR_parse.c
mkdir -p build/obj/./src/IR_parse/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR_parse/IR_parse.o src/IR_parse/IR_parse.c
echo "+ CC" live_variable_analysis.c
mkdir -p build/obj/./src/IR_optimize/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR_optimize/live_variable_analysis.o src/IR_optimize/live_variable_analysis.c
echo "+ CC" solver.c
mkdir -p build/obj/./src/IR_optimize/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR_optimize/solver.o src/IR_optimize/solver.c
echo "+ CC" copy_propagation.c
mkdir -p build/obj/./src/IR_optimize/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR_optimize/copy_propagation.o src/IR_optimize/copy_propagation.c
echo "+ CC" IR_optimize.c
mkdir -p build/obj/./src/IR_optimize/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR_optimize/IR_optimize.o src/IR_optimize/IR_optimize.c
echo "+ CC" constant_propagation.c
mkdir -p build/obj/./src/IR_optimize/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR_optimize/constant_propagation.o src/IR_optimize/constant_propagation.c
echo "+ CC" local_optimize.c
mkdir -p build/obj/./src/IR_optimize/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR_optimize/local_optimize.o src/IR_optimize/local_optimize.c
echo "+ CC" available_expressions_analysis.c
mkdir -p build/obj/./src/IR_optimize/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/./src/IR_optimize/available_expressions_analysis.o src/IR_optimize/available_expressions_analysis.c
echo "+ LEX" IR_lexical.l
mkdir -p build/generate/IR_parse/
flex -o build/generate/IR_parse/IR_lex.yy.c src/IR_parse/IR_lexical.l
echo "+ YACC" IR_syntax.y
mkdir -p build/generate/IR_parse/
bison -d -v -o build/generate/IR_parse/IR_syntax.tab.c src/IR_parse/IR_syntax.y
echo "+ CC" IR_lex.yy.c
mkdir -p build/obj/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/IR_lex.yy.o build/generate/IR_parse/IR_lex.yy.c
echo "+ CC" IR_syntax.tab.c
mkdir -p build/obj/
gcc -MMD -c -Wall -I./src/IR_parse/include -I./build/generate/IR_parse -I./src/IR_optimize/include -I./include -O2 -o build/obj/IR_syntax.tab.o build/generate/IR_parse/IR_syntax.tab.c
echo "+ LD" parser
gcc -o parser build/obj/./src/main.o build/obj/./src/container/treap.o build/obj/./src/IR/IR_function_append_sentence.o build/obj/./src/IR/IR_symbol_allocator.o build/obj/./src/IR/IR_function_build_graph.o build/obj/./src/IR/IR_display.o build/obj/./src/IR/IR.o build/obj/./src/IR/IR_stmt.o build/obj/./src/IR_parse/IR_parse.o build/obj/./src/IR_optimize/live_variable_analysis.o build/obj/./src/IR_optimize/solver.o build/obj/./src/IR_optimize/copy_propagation.o build/obj/./src/IR_optimize/IR_optimize.o build/obj/./src/IR_optimize/constant_propagation.o build/obj/./src/IR_optimize/local_optimize.o build/obj/./src/IR_optimize/available_expressions_analysis.o build/obj/IR_lex.yy.o build/obj/IR_syntax.tab.o -lfl -ly