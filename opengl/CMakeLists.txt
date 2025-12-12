cmake_minimum_required(VERSION 3.11.0)
project(HelloGL VERSION 0.1.0)

# 使用 C++ 17 标准
set(CMAKE_CXX_STANDARD 17)

# 设置代码文件
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)

# 添加头文件
set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include/)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)
include_directories(${HEADER_DIR} ${LIB_DIR})

# 添加目标链接
set(GLFW_LINK ${LIB_DIR}libglfw.3.dylib)
link_libraries(${GLFW_LINK})



# 执行编译命令
set(SOURCES ${SRC_DIR}/glad.c ${SRC_DIR}/TextRenderer.cpp ${SRC_DIR}/UI.cpp  ${SRC_DIR}/Player.cpp ${SRC_DIR}/Game.cpp ${SRC_DIR}/main.cpp)
set(HEADERS ${SRC_DIR}/Shader.h ${SRC_DIR}/TextRenderer.h ${SRC_DIR}/UI.h ${SRC_DIR}/Player.h ${SRC_DIR}/Game.h)
# set(SOURCES ${SRC_DIR}glad.c ${SRC_DIR}main.cpp)

add_executable(HelloGL ${SOURCES})

# 链接系统的 OpenGL 框架
if (APPLE)
    target_link_libraries(HelloGL "-framework OpenGL")
endif()

set(RESOURCE_DIRS
    shaders
    asserts
)

foreach(dir ${RESOURCE_DIRS})
    add_custom_command(TARGET HelloGL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/${dir}
        ${CMAKE_CURRENT_BINARY_DIR}/${dir}
    )
endforeach()

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

