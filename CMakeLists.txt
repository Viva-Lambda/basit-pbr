cmake_minimum_required (VERSION 3.0.2)
project("OpenGL Phong - PBR")

find_package(OpenGL REQUIRED)

include_directories(
    # my headers
    "./include/"
    )


set( GLFW_SHARED_LIB
    "${PROJECT_SOURCE_DIR}/libs/glfw/src/libglfw.so")

set (ASSIMP_SHARED_LIB
    "${PROJECT_SOURCE_DIR}/libs/assimp/libassimp.so.5")

set (CMAKE_CXX_FLAGS "-std=c++17")

set (FLAGS "-ldl -ggdb -Wall -Wextra")

set ( ALL_LIBS
    ${OpenGL}
    ${GLFW_SHARED_LIB}
    ${ASSIMP_SHARED_LIB}
    "-ldl"
    )

add_executable(myWin.out
    "src/glad.c"
    "src/window.cpp"
    )
add_executable(phong.out
    "src/glad.c"
    "src/phong/phong.cpp"
    )
add_executable(pbr.out 
    "src/glad.c"
    "src/pbr/simplepbr.cpp"
    )
add_executable(pbrtexture.out 
    "src/glad.c"
    "src/pbr/pbrtexture.cpp"
    )

target_link_libraries(myWin.out ${ALL_LIBS})
target_link_libraries(phong.out ${ALL_LIBS})
target_link_libraries(pbr.out ${ALL_LIBS})
target_link_libraries(pbrtexture.out ${ALL_LIBS})
install(TARGETS myWin.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
install(TARGETS phong.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
install(TARGETS pbr.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
install(TARGETS pbrtexture.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
