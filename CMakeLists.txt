cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(VulkanGame 
    VERSION 1.0.0
    DESCRIPTION "A game made in C with the Vulkan API"
    LANGUAGES C
)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler-specific flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Warning flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wformat-security")
    
    # Debug flags
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -DDEBUG")
    
    # Release flags
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
    
    # Address sanitizer for debug builds (optional)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND ENABLE_ASAN)
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
    endif()
endif()

# Find required packages
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

# Find OpenMP (commented out as in original)
#find_package(OpenMP REQUIRED)

# Collect all .c files in the src directory
file(GLOB_RECURSE SOURCES 
    "src/*.c" 
    "external/CThreads/*.c"
    "external/timer_lib/*.c"
)

# Create the executable
add_executable(vulkan_game ${SOURCES})

# Set target properties
set_target_properties(vulkan_game PROPERTIES
    OUTPUT_NAME "vulkan_game"
)

# Add include directories using target-based approach
target_include_directories(vulkan_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/CThreads
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/external/timer_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyobjloader
)

# Link libraries using modern CMake targets
target_link_libraries(vulkan_game PRIVATE
    Vulkan::Vulkan
    glfw
#    OpenMP::OpenMP_C
)

# Create build directory for shaders
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

# Find glslangValidator
find_program(GLSLANG_VALIDATOR
    NAMES glslangValidator glslangValidator.exe
    HINTS 
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        $ENV{VULKAN_SDK}/bin
        $ENV{VULKAN_SDK}/Bin
    REQUIRED
)

# Function to compile shaders
function(compile_shader SHADER_SOURCE SHADER_OUTPUT)
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/shaders/${SHADER_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
        COMMAND ${GLSLANG_VALIDATOR} -V ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER_SOURCE} 
                -o ${CMAKE_BINARY_DIR}/shaders/${SHADER_OUTPUT}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER_SOURCE}
        COMMENT "Compiling shader: ${SHADER_SOURCE}"
        VERBATIM
    )
endfunction()

# Compile all shaders
compile_shader(scene.vert scene.vert.spv)
compile_shader(scene.frag scene.frag.spv)
compile_shader(shadow.vert shadow.vert.spv)
compile_shader(shadow.frag shadow.frag.spv)

# Create shader compilation target
add_custom_target(compile_shaders ALL
    DEPENDS 
        ${CMAKE_BINARY_DIR}/shaders/scene.vert.spv 
        ${CMAKE_BINARY_DIR}/shaders/scene.frag.spv
        ${CMAKE_BINARY_DIR}/shaders/shadow.vert.spv
        ${CMAKE_BINARY_DIR}/shaders/shadow.frag.spv
    COMMENT "Compiling all shaders"
)

# Make executable depend on shaders
add_dependencies(vulkan_game compile_shaders)

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Vulkan found: ${Vulkan_FOUND}")
message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "GLFW found: ${glfw3_FOUND}")
message(STATUS "GLSL Validator: ${GLSLANG_VALIDATOR}")