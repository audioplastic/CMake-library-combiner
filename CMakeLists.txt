cmake_minimum_required(VERSION 2.8.7)

option(COMBI_MAC "The libtool way." OFF)
option(COMBI_EXTRACT "The ar -xtraction method" OFF)
option(COMBI_RECYCLE "Recycling the CMake build products (if all libs are made as part of the build)" OFF)

add_library(b b.cpp b.h)
add_library(a a.cpp a.h)
add_executable(main main.cpp)

set(C_LIB ${CMAKE_BINARY_DIR}/libcombi.a)

if (COMBI_MAC)
    add_custom_target(combined_custom
            COMMAND libtool -static -o ${C_LIB} $<TARGET_FILE:a> $<TARGET_FILE:b>
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS a b
            )
endif ()


if (COMBI_EXTRACT)
    set(oprefix object_)
    add_custom_target(combined_custom
            COMMAND mkdir ${oprefix}$<TARGET_NAME:a> || true && cd ${oprefix}$<TARGET_NAME:a> &&  ar -x $<TARGET_FILE:a>
            COMMAND mkdir ${oprefix}$<TARGET_NAME:b> || true && cd ${oprefix}$<TARGET_NAME:b> &&  ar -x $<TARGET_FILE:b>
            COMMAND ar -qcs ${C_LIB} ${oprefix}*/*.o
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS a b
            )
endif ()

if (COMBI_RECYCLE)
    add_custom_target(combined_custom
            COMMAND ar -qcs ${C_LIB} $<TARGET_NAME:a>.dir/*.o $<TARGET_NAME:b>.dir/*.o
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}CMakeFiles
            DEPENDS a b
            )
endif ()


add_library(combined STATIC IMPORTED GLOBAL)
add_dependencies(combined combined_custom)

set_target_properties(combined
        PROPERTIES
        IMPORTED_LOCATION ${C_LIB}
        )

target_link_libraries(main c)