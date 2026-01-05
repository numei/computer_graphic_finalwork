cmake_minimum_required(VERSION 3.11.0)
project(HelloGL VERSION 0.1.0)

# Use C++ 17 standard
set(CMAKE_CXX_STANDARD 17)

# Set source files directory
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)

# Add header files
set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include/)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)
include_directories(${HEADER_DIR} ${LIB_DIR})

# Compile sources
set(SOURCES ${SRC_DIR}/glad.c ${SRC_DIR}/TextRenderer.cpp ${SRC_DIR}/UI.cpp  ${SRC_DIR}/Player.cpp ${SRC_DIR}/Game.cpp ${SRC_DIR}/main.cpp)
set(HEADERS ${SRC_DIR}/Shader.h ${SRC_DIR}/TextRenderer.h ${SRC_DIR}/UI.h ${SRC_DIR}/Player.h ${SRC_DIR}/Game.h)
# set(SOURCES ${SRC_DIR}glad.c ${SRC_DIR}main.cpp)

add_executable(HelloGL ${SOURCES})

# Link libraries (must be after add_executable)
if(WIN32)
    # Windows: Find GLFW library
    # First try CMake's find_package (if using vcpkg)
    find_package(glfw3 QUIET)
    if(glfw3_FOUND)
        # Use vcpkg or system-installed GLFW
        target_link_libraries(HelloGL glfw)
        message(STATUS "Using vcpkg/system GLFW")
    else()
        # Manually find GLFW in lib folder
        # Check if glfw3.lib exists directly
        if(EXISTS "${LIB_DIR}/glfw3.lib")
            set(GLFW_LIBRARY "${LIB_DIR}/glfw3.lib")
            message(STATUS "Found GLFW: ${GLFW_LIBRARY}")
            target_link_libraries(HelloGL ${GLFW_LIBRARY})
            # If DLL exists, copy it to output directory
            if(EXISTS "${LIB_DIR}/glfw3.dll")
                add_custom_command(TARGET HelloGL POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${LIB_DIR}/glfw3.dll"
                    $<TARGET_FILE_DIR:HelloGL>)
                message(STATUS "Will copy glfw3.dll to output directory")
            endif()
        else()
            # Try find_library as fallback
            find_library(GLFW_LIBRARY 
                NAMES glfw3 glfw
                PATHS ${LIB_DIR}
                NO_DEFAULT_PATH
            )
            if(GLFW_LIBRARY)
                message(STATUS "Found GLFW: ${GLFW_LIBRARY}")
                target_link_libraries(HelloGL ${GLFW_LIBRARY})
                get_filename_component(GLFW_DLL_PATH "${GLFW_LIBRARY}" DIRECTORY)
                if(EXISTS "${GLFW_DLL_PATH}/glfw3.dll")
                    add_custom_command(TARGET HelloGL POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${GLFW_DLL_PATH}/glfw3.dll"
                        $<TARGET_FILE_DIR:HelloGL>)
                    message(STATUS "Will copy glfw3.dll to output directory")
                endif()
            else()
                message(FATAL_ERROR 
                    "GLFW library not found!\n"
                    "Please download Windows GLFW from: https://www.glfw.org/download.html\n"
                    "Or use vcpkg: vcpkg install glfw3:x64-windows\n"
                    "Then copy glfw3.lib to ${LIB_DIR}")
            endif()
        endif()
    endif()
elseif(APPLE)
    # macOS platform
    set(GLFW_LINK ${LIB_DIR}libglfw.3.dylib)
    if(EXISTS ${GLFW_LINK})
        target_link_libraries(HelloGL ${GLFW_LINK})
    else()
        message(FATAL_ERROR "GLFW library not found: ${GLFW_LINK}")
    endif()
else()
    # Linux platform
    find_package(glfw3 REQUIRED)
    target_link_libraries(HelloGL glfw)
endif()

# Link system OpenGL framework
if (APPLE)
    target_link_libraries(HelloGL "-framework OpenGL")
elseif(WIN32)
    # Windows: OpenGL loaded through glad, usually no additional linking needed
    # If needed, can link opengl32
    # target_link_libraries(HelloGL opengl32)
else()
    # Linux
    find_package(OpenGL REQUIRED)
    target_link_libraries(HelloGL OpenGL::GL)
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

