cmake_minimum_required(VERSION 3.30)
project(EmeraldIsle)

find_package(OpenGL REQUIRED)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(external)

include_directories(
        external/glfw-3.1.2/include/
        external/glm-0.9.7.1/
        external/glad-opengl-3.3/include/
        external/tinygltf-2.9.3/
        external/
        EmeraldIsle/
)

add_executable(main
        EmeraldIsle/main.cpp
        EmeraldIsle/render/shader.cpp
        EmeraldIsle/feature/Forest.cpp
        EmeraldIsle/fileTool/FileHandler.cpp
        EmeraldIsle/feature/Island.cpp
        EmeraldIsle/feature/Pannel.cpp
        EmeraldIsle/feature/Sea.cpp
        EmeraldIsle/feature/Robot.cpp
)

target_link_libraries(main
        ${OPENGL_LIBRARY}
        glfw
        glad
)