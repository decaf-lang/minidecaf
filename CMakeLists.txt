cmake_minimum_required(VERSION 3.10)
project(MiniDecaf)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
    ${PROJECT_SOURCE_DIR}/generated/
    ${PROJECT_SOURCE_DIR}/third_party/
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-runtime/
    ${PROJECT_SOURCE_DIR}/src/
)
 
set(src_dir
    ${PROJECT_SOURCE_DIR}/generated/MiniDecafLexer.cpp
    ${PROJECT_SOURCE_DIR}/generated/MiniDecafParser.cpp
    ${PROJECT_SOURCE_DIR}/generated/MiniDecafVisitor.cpp
    ${PROJECT_SOURCE_DIR}/generated/MiniDecafBaseVisitor.cpp
    ${PROJECT_SOURCE_DIR}/src/CodeGenVisitor.cpp
    ${PROJECT_SOURCE_DIR}/src/VarAllocVisitor.cpp
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
add_executable(MiniDecaf ${src_dir} src/main.cpp)
target_link_libraries(MiniDecaf antlr4-cpp-runtime)

# add_custom_command(TARGET antlr4-cpp-runtime
#     PRE_BUILD 
#     WORKING_DIRECTORY
#         ${PROJECT_SOURCE_DIR}/generated/
#     COMMAND 
#         java -jar /usr/local/lib/antlr-4.8-complete.jar -Dlanguage=Cpp -no-listener -visitor MiniDecaf.g4
#     COMMENT 
#         "Auto updates for developing antlr grammar"
# )

add_custom_target(update_antlr
    WORKING_DIRECTORY
        ${PROJECT_SOURCE_DIR}/generated/
    COMMAND 
        java -jar /usr/local/lib/antlr-4.8-complete.jar -Dlanguage=Cpp -no-listener -visitor MiniDecaf.g4
    COMMENT 
        "Auto updates for developing antlr grammar"
)
