cmake_minimum_required(VERSION 3.10)
project(MiniDecaf)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Java)

include_directories(
    ${PROJECT_SOURCE_DIR}/generate/
    ${PROJECT_SOURCE_DIR}/third_party/
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/
    ${PROJECT_SOURCE_DIR}/src/
)

file(GLOB antlr4-cpp-src
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/*.cpp
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/atn/*.cpp
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/dfa/*.cpp
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/misc/*.cpp
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/support/*.cpp
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/tree/*.cpp
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/tree/pattern/*.cpp
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/tree/xpath/*.cpp
)
add_library(antlr4-cpp-runtime ${antlr4-cpp-src})

execute_process(
    WORKING_DIRECTORY
        ${PROJECT_SOURCE_DIR}/generate/
    # Note that this command runs in pure shell
    COMMAND
        java -jar ${PROJECT_SOURCE_DIR}/third_party/antlr-4.8-complete.jar -Dlanguage=Cpp -no-listener -visitor MiniDecaf.g4
)

file(GLOB src_dir
    ${PROJECT_SOURCE_DIR}/generate/MiniDecafLexer.cpp
    ${PROJECT_SOURCE_DIR}/generate/MiniDecafParser.cpp
    ${PROJECT_SOURCE_DIR}/generate/MiniDecafVisitor.cpp
    ${PROJECT_SOURCE_DIR}/generate/MiniDecafBaseVisitor.cpp
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(MiniDecaf ${src_dir})
target_link_libraries(MiniDecaf antlr4-cpp-runtime)

# Add custom target update_antlr, use `make update_antlr` to update your lexer & grammar rules anytime
add_custom_target(update_antlr
    WORKING_DIRECTORY
        ${PROJECT_SOURCE_DIR}/generate/
    COMMAND 
        java -jar ${PROJECT_SOURCE_DIR}/third_party/antlr-4.8-complete.jar -Dlanguage=Cpp -no-listener -visitor MiniDecaf.g4
    COMMENT 
        "Auto updates for developing antlr grammar"
)