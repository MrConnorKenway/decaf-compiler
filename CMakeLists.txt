cmake_minimum_required(VERSION 3.16)
project(DecafCompiler)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

add_custom_command(
        OUTPUT lexer_gen.l
        COMMAND lexer/gen_lexer.py ${CMAKE_CURRENT_BINARY_DIR}/lexer_gen.l
        DEPENDS ${ROOT}/lexer/*
        WORKING_DIRECTORY ${ROOT}
        COMMENT "generating flex file"
)

add_custom_command(
        OUTPUT parser_gen.yxx
        COMMAND parser/gen_parser.py ${CMAKE_CURRENT_BINARY_DIR}/parser_gen.yxx
        DEPENDS ${ROOT}/parser/*
        WORKING_DIRECTORY ${ROOT}
        COMMENT "generating bison file"
)

add_custom_command(
        OUTPUT parser.tab.cxx parser.tab.h
        COMMAND bison ${ROOT}/parser/parser_handwritten.yxx --defines=parser.tab.h --output=parser.tab.cxx -v
        DEPENDS ${ROOT}/parser/parser_handwritten.yxx
        COMMENT "generating parser source and header file"
)

add_custom_command(
        OUTPUT visitor.h
        COMMAND visitor/gen_visitor.py base --header-dst ${CMAKE_CURRENT_BINARY_DIR}/visitor.h
        DEPENDS ${ROOT}/visitor/visitor_tmpl.h ${ROOT}/visitor/gen_visitor.py
        WORKING_DIRECTORY ${ROOT}
        COMMENT "generating base visitor"
)

set(VISITOR_HEADER create_symbol_table_visitor.h display_visitor.h static_semantic_analysis_visitor.h)
foreach (header ${VISITOR_HEADER})
    string(REPLACE ".h" "" target ${header})
    string(REPLACE ".h" "_tmpl.h" tmpl_header ${header})
    string(REPLACE ".h" "_gen.cxx" gen_src ${header})
    add_custom_command(
            OUTPUT ${header} ${gen_src}
            COMMAND visitor/gen_visitor.py --header-dst ${CMAKE_CURRENT_BINARY_DIR}/${header} --src-dst ${CMAKE_CURRENT_BINARY_DIR}/${gen_src} ${target}
            DEPENDS ${ROOT}/visitor/${tmpl_header} ${ROOT}/visitor/gen_visitor.py visitor.h parser.tab.h ast.h
            WORKING_DIRECTORY ${ROOT}
            COMMENT "generating ${header}"
    )
endforeach ()

string(PREPEND "visitor.h" VISITOR_HEADER)

add_custom_command(
        OUTPUT ${ROOT}/parser/parser_handwritten.yxx
        COMMAND parser/merge_hand_and_gen.py ${CMAKE_CURRENT_BINARY_DIR}/parser_gen.yxx
        DEPENDS ${ROOT}/parser/bnf ${ROOT}/parser/merge_hand_and_gen.py ${ROOT}/parser/gen_parser.py ${ROOT}/parser/parser_tmpl.yxx
        WORKING_DIRECTORY ${ROOT}
        COMMENT "merging handwritten and generated bison file"
)

add_custom_command(
        OUTPUT lexer.cxx
        COMMAND flex -o lexer.cxx lexer_gen.l
        DEPENDS lexer_gen.l
        COMMENT "generating lexer source file"
)

add_custom_command(
        OUTPUT ast.h ast.cxx
        COMMAND ${ROOT}/ast/gen_ast.py --header-dst ${CMAKE_CURRENT_BINARY_DIR}/ast.h --src-dst ${CMAKE_CURRENT_BINARY_DIR}/ast.cxx
        DEPENDS ${ROOT}/ast/* visitor.h
        WORKING_DIRECTORY ${ROOT}
        COMMENT "generating ast header and source file"
)

include_directories(${ROOT})

add_library(ast SHARED ast.cxx)
add_library(visitors SHARED
        visitor/create_symbol_table_visitor_handwritten.cxx
        visitor/display_visitor_handwritten.cxx
        visitor/static_semantic_analysis_visitor_handwritten.cxx
        ${VISITOR_HEADER})
add_library(static_analyser SHARED kern/static_semantic_analyser.cxx)
add_library(symbol_table SHARED kern/symbol_table.cxx)
add_library(parser SHARED parser.tab.cxx ast.h)
add_library(lexer SHARED lexer.cxx parser.tab.h ast.h)
add_dependencies(symbol_table ast visitors)
add_dependencies(static_analyser ast visitors)

set(ALL_LIB ast visitors static_analyser symbol_table parser lexer)

add_executable(decaf kern/main.cxx)
target_link_libraries(decaf ${ALL_LIB})

