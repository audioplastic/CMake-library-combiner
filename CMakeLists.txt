cmake_minimum_required(VERSION 2.8.7)

option(COMBI "Set the method for combining the libs." 0)

add_library(b b.cpp b.h)
add_library(a a.cpp a.h)
add_executable(main main.cpp)

set(C_LIB ${CMAKE_BINARY_DIR}/libcombi.a)

if(COMBI EQUAL 0)
    # Building a library from all of the object files compiled as part of this build
    # This will break if the sub-libs have complex directory structures
    add_custom_target(combined_custom
            COMMAND ar -qcs ${C_LIB} $<TARGET_NAME:a>.dir/*.o $<TARGET_NAME:b>.dir/*.o
            WORKING_DIRECTORY ${CMAKE_BUILD_DIR}CMakeFiles
            DEPENDS a b
            )
elseif(COMBI EQUAL 1)
    # Extracting the objects from prebuilt archives
    # Notice that it is important to put each in its own dir to prevent any overwriting
    # This is the most foolproof way that works on OSX and Linux
    set(OPREFIX object_)
    add_custom_target(combined_custom
            COMMAND mkdir ${OPREFIX}$<TARGET_NAME:a> || true && cd ${OPREFIX}$<TARGET_NAME:a> &&  ar -x $<TARGET_FILE:a>
            COMMAND mkdir ${OPREFIX}$<TARGET_NAME:b> || true && cd ${OPREFIX}$<TARGET_NAME:b> &&  ar -x $<TARGET_FILE:b>
            COMMAND ar -rs ${C_LIB} ${OPREFIX}*/*.o
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS a b
            )
else()
    # The Mac libtool way
    add_custom_target(combined_custom
            COMMAND libtool -static -o ${C_LIB} $<TARGET_FILE:a> $<TARGET_FILE:b>
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS a b
            )
endif()


add_library(combined STATIC IMPORTED GLOBAL)
add_dependencies(combined combined_custom)

set_target_properties(combined
        PROPERTIES
        IMPORTED_LOCATION ${C_LIB}
        )

target_link_libraries(main combined)