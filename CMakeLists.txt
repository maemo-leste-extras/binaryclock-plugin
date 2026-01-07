cmake_minimum_required(VERSION 3.25)
project(binaryclock_menu_plugin C)

set(CMAKE_C_STANDARD 17)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")
include(CheckIncludeFiles)
include(CheckIncludeFile)
include(CheckFunctionExists)
include(GNUInstallDirs)
option(BUILD_EXAMPLE "Build the example application" OFF)

find_package(PkgConfig REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
            "Default build type: Debug" FORCE)
endif()
if(BUILD_EXAMPLE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
            "Default build type: Debug" FORCE)
endif()

execute_process(
        COMMAND pkg-config --variable=hildondesktoplibdir libhildondesktop-1
        OUTPUT_VARIABLE HILDON_WIDGET_LIB_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND pkg-config --variable=hildonstatusmenudesktopentrydir libhildondesktop-1
        OUTPUT_VARIABLE HILDON_WIDGET_DATA_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "HILDON_WIDGET_LIB_DIR: ${HILDON_WIDGET_LIB_DIR}")
message(STATUS "HILDON_WIDGET_DATA_DIR: ${HILDON_WIDGET_DATA_DIR}")
set(PATH_RESOURCES "${HILDON_WIDGET_DATA_DIR}/binaryclock-menu-plugin/")

configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/globals.h.cmake
        ${CMAKE_BINARY_DIR}/src/globals.h
        @ONLY # only @VARIABLE@ placeholders
)

pkg_check_modules(HILDON REQUIRED hildon-1)
pkg_check_modules(HILDON_DESKTOP REQUIRED libhildondesktop-1)
pkg_check_modules(LIBTIME REQUIRED libtime)
pkg_check_modules(GCONF REQUIRED gconf-2.0)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(GDK_PIXBUF REQUIRED gdk-pixbuf-2.0)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
pkg_check_modules(LIBTIME REQUIRED libtime)

set(SRC
        src/widget.c
)

add_library(binaryclock_menu_plugin SHARED ${SRC})

set(INCLUDE_DIRS
        ${HILDON_INCLUDE_DIRS}
        ${HILDON_DESKTOP_INCLUDE_DIRS}
        ${LIBTIME_INCLUDE_DIRS}
        ${GCONF_INCLUDE_DIRS}
        ${GDK_PIXBUF_INCLUDE_DIRS}
        ${GTK2_INCLUDE_DIRS}
        ${LIBTIME_INCLUDE_DIRS})

set(LINK_LIBS
        ${HILDON_LIBRARIES}
        ${HILDON_DESKTOP_LIBRARIES}
        ${LIBTIME_LIBRARIES}
        ${GDK_PIXBUF_LIBRARIES}
        ${GTK2_LIBRARIES}
        ${GCONF_LIBRARIES}
        ${LIBTIME_LIBRARIES}
        m)

set(COMPILE_OPTIONS
        ${HILDON_CFLAGS_OTHER}
        ${HILDON_DESKTOP_CFLAGS_OTHER}
        ${LIBTIME_CFLAGS_OTHER}
        ${GCONF_CFLAGS_OTHER}
        ${GDK_PIXBUF_CFLAGS_OTHER}
        ${GTK2_CFLAGS_OTHER}
        ${LIBTIME_CLFAGS_OTHER})

target_include_directories(binaryclock_menu_plugin PRIVATE ${INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src/)
target_link_libraries(binaryclock_menu_plugin PRIVATE ${LINK_LIBS})
target_compile_options(binaryclock_menu_plugin PRIVATE ${COMPILE_OPTIONS})
set_target_properties(binaryclock_menu_plugin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${HILDON_WIDGET_LIB_DIR}"
        OUTPUT_NAME "lib-binaryclock-menu-plugin"
        PREFIX ""
)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    target_compile_definitions(binaryclock_menu_plugin PRIVATE DEBUG=1)
endif()

# /usr/lib/x86_64-linux-gnu/hildon-desktop/lib-binaryclock-menu-plugin.so
install(TARGETS binaryclock_menu_plugin
        LIBRARY DESTINATION "${HILDON_WIDGET_LIB_DIR}"
)

install(FILES "${CMAKE_SOURCE_DIR}/cmake/binaryclock-menu-plugin.desktop"
        DESTINATION ${HILDON_WIDGET_DATA_DIR}
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/"
        DESTINATION "${HILDON_WIDGET_DATA_DIR}/binaryclock-menu-plugin"
        FILES_MATCHING PATTERN "*"
)
