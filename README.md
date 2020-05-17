# A toy decaf language compiler

## Requirements
This project depends on `flex`, `bison`, `make`, `g++` and `python3`. Make sure your `g++` compiler support c++11 syntax.

## Installation
Run 
```
make build-%
```
to build wanted version of compiler binary executable. Currently this project only provides display version, that is, the compiler reads a input source file and build an AST according to `parser/bnf` and display the AST of the file in the same way like the unix tree command.

To remove the built file, run 
```
make clean
```

## Testing
This project provides plenty of test cases, located in directory `test`. To test specific decaf source file, simply run
```
make test-%
```
where `%` represents the name without extension of files in `test` folder.

## Project Structure Overview
Folder `ast/` contains the definition of AST. The language keywords, allowed operations, flex template file and python script used to generate full version flex source file are placed in `lexer/`.

`parser/` consists of bnf of language grammar, script used to generate bison file, script used to merge handcrafted and machine-generated code, and compiler's main code.

All the testing source file are put in `test/`.

In `visitor/`, file `visitor/%_visitor_handwritten.cxx` contains the implementation of visit methods. `visitor/gen_visitor.py` use `visitor/%_visitor_tmpl.cxx` and `visitor/%_visitor_tmpl.h` to generate `build/src/%_visitor.cxx` and `build/include/%_visitor.h`. 

## Customization
To add a new syntax feature, you first append grammar rules in `parser/bnf` and `parser/gen_parser.py` will automatically generate declaration in declaration section of `build/gen/parser_gen.yxx`. You can write syntax action in `parser/parser_handwritten.yxx` and then use `parser/merge_hand_and_gen.py` to merge them together. After that you need to define the corresponding AST node of your syntax feature and add the class definition in `ast/ast_tmpl.h`. Note that this project has provided a script (`ast/gen_ast.py`) to automatically generate constructor and accept function, so you only need to provide fields declaration. Then you need to provide implementation of visit method in `visitor/%_visitor_handwritten.cxx`.

To define custom operations on the AST, you can define your own visitor class in `visitor/` folder. You can use the content of `build/src/%_visitor.cxx` as a template to write your own version of `visitor/%_visitor_handwritten.cxx`. After that, use command
```
make build-%
```
to build compiler.

To debug your custom compiler, use command
```
make gdb-%
```
where `%` represents the input source file name.