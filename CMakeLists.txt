cmake_minimum_required(VERSION 3.3)
project(uss)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ext/mpc.c parser.c ussval.c builtins/builtin_ffi.c ussenv.c builtin.c support/file.c support/uss_strings.c
        builtins/builtin_string.c support/jenkins.c builtins/builtin_ord.c builtins/builtin_math.c builtins/builtin_list.c
        builtins/builtin_equality.c)

set(TOOL_USS_FILES tools/uss.c)
set(TOOL_USSI_FILES tools/ussi.c)
set(TOOL_USSLINT_FILES tools/usslint.c)
set(TOOL_USSDOC_FILES tools/ussdoc.c)

set(USS_FILES stdlib/ffi.uss)

include_directories(include)

find_package(PkgConfig REQUIRED)

add_library(USS ${SOURCE_FILES})

add_executable(uss ${TOOL_USS_FILES})
add_executable(ussi ${TOOL_USSI_FILES})
add_executable(usslint ${TOOL_USSLINT_FILES})
add_executable(ussdoc ${TOOL_USSDOC_FILES})

pkg_search_module(LIBEDIT REQUIRED libedit)
pkg_search_module(LIBFFI REQUIRED libffi)
pkg_search_module(LIBGC REQUIRED bdw-gc)
pkg_search_module(LIBUNWIND REQUIRED libunwind)
pkg_searcH_module(LIBUV REQUIRED libuv)

target_link_libraries(USS ${LIBFFI_LIBRARIES} ${LIBGC_LIBRARIES} ${LIBUV_LIBRARIES} m dl)
target_include_directories(USS PUBLIC ${LIBFFI_INCLUDE_DIRS} ${LIBGC_INCLUDE_DIRS} ${LIBUV_INCLUDE_DIRS})
target_compile_options(USS PUBLIC ${LIBFFI_CFLAGS_OTHER} ${LIBGC_CFLAGS_OTHER} ${LIBUV_CFLAGS_OTHER})

target_link_libraries(ussi USS ${LIBEDIT_LIBRARIES})
target_include_directories(ussi PUBLIC ${LIBEDIT_INCLUDE_DIRS})
target_compile_options(ussi PUBLIC ${LIBEDIT_CFLAGS_OTHER})

target_link_libraries(ussdoc USS)
target_link_libraries(usslint USS)

target_link_libraries(uss USS)
target_include_directories(uss PUBLIC )
target_compile_options(uss PUBLIC -g)


