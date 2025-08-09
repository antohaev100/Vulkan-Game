cmake_minimum_required(VERSION 3.10)

# Set the project name
project(HelloWorld C)

# Collect all .c files in the src directory
file(GLOB SOURCES "src/*.c" "src/*/*.c" "includes/*/*.c")


# Add the include directories
include_directories(
    shaders
    headers
    includes/CThreads
    includes/stb
    includes/timer_lib
    includes/tinyobjloader
    "C:/VulkanSDK/1.3.296.0/Include"
    "C:/glfw-3.4.bin.WIN64/include"
)


# Add the library directories
link_directories(
    "C:/VulkanSDK/1.3.296.0/Lib"
    "C:/glfw-3.4.bin.WIN64/lib-mingw-w64"
)

# Find OpenMP
#find_package(OpenMP REQUIRED)

# Add the executable
add_executable(hello_world ${SOURCES})

# Link the libraries
target_link_libraries(hello_world
    glfw3
    vulkan-1
#    OpenMP::OpenMP_C
)

# Set the path to glslangValidator
set(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE "C:/VulkanSDK/1.3.296.0/Bin/glslangValidator.exe")

# Compile shaders
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/build/shaders/scene.vert.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${CMAKE_SOURCE_DIR}/shaders/scene.vert -o ${CMAKE_SOURCE_DIR}/build/shaders/scene.vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/scene.vert
    COMMENT "Compiling vertex shader"
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/build/shaders/scene.frag.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${CMAKE_SOURCE_DIR}/shaders/scene.frag -o ${CMAKE_SOURCE_DIR}/build/shaders/scene.frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/scene.frag
    COMMENT "Compiling fragment shader"
)

# Compile shadow shaders
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/build/shaders/shadow.vert.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${CMAKE_SOURCE_DIR}/shaders/shadow.vert -o ${CMAKE_SOURCE_DIR}/build/shaders/shadow.vert.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/shadow.vert
    COMMENT "Compiling shadow vertex shader"
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/build/shaders/shadow.frag.spv
    COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${CMAKE_SOURCE_DIR}/shaders/shadow.frag -o ${CMAKE_SOURCE_DIR}/build/shaders/shadow.frag.spv
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders/shadow.frag
    COMMENT "Compiling shadow fragment shader"
)

add_custom_target(
    Shaders
    DEPENDS 
        ${CMAKE_SOURCE_DIR}/build/shaders/scene.vert.spv 
        ${CMAKE_SOURCE_DIR}/build/shaders/scene.frag.spv
        ${CMAKE_SOURCE_DIR}/build/shaders/shadow.vert.spv
        ${CMAKE_SOURCE_DIR}/build/shaders/shadow.frag.spv
)

add_dependencies(hello_world Shaders)

# Set compiler optimization flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")