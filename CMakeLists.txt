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

# if GLFW isn't found automatically
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)

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

# Find glslangValidator with better error handling
find_program(GLSLANG_VALIDATOR
    NAMES glslangValidator glslangValidator.exe
    HINTS 
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        $ENV{VULKAN_SDK}/bin
        $ENV{VULKAN_SDK}/Bin
        $ENV{VULKAN_SDK}/x86_64/bin
        $ENV{VK_SDK_PATH}/bin
        /usr/bin
        /usr/local/bin
    DOC "Vulkan GLSL compiler"
)

if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found! Please install Vulkan SDK or set VULKAN_SDK environment variable.")
endif()

message(STATUS "GLSL Validator: ${GLSLANG_VALIDATOR}")

# Automatically discover all shader files
file(GLOB_RECURSE SHADER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.comp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.geom"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.tesc"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.tese"
)

# Enhanced shader compilation function with validation and error reporting
function(compile_shader SHADER_PATH)
    # Get shader filename and create output path
    get_filename_component(SHADER_NAME ${SHADER_PATH} NAME)
    set(SHADER_OUTPUT "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv")
    
    # Get relative path for better display
    file(RELATIVE_PATH SHADER_REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_PATH})
    
    # Determine compilation flags based on build type
    set(SHADER_FLAGS "")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(SHADER_FLAGS "-Os")  # Optimize for size in release builds
    endif()
    
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
        COMMAND ${GLSLANG_VALIDATOR} 
            -V                          # Vulkan semantics
            --target-env vulkan1.0      # Target Vulkan 1.0
            -o ${SHADER_OUTPUT}         # Output file
            ${SHADER_PATH}              # Input file
            ${SHADER_FLAGS}             # Optional optimization flags
        DEPENDS ${SHADER_PATH}
        COMMENT "Compiling shader: ${SHADER_REL_PATH} -> ${SHADER_NAME}.spv"
        VERBATIM
    )
    
    # Add to global shader list
    set_property(GLOBAL APPEND PROPERTY SHADER_OUTPUTS ${SHADER_OUTPUT})
endfunction()

# Compile all discovered shaders
set_property(GLOBAL PROPERTY SHADER_OUTPUTS "")
foreach(SHADER_FILE ${SHADER_SOURCES})
    compile_shader(${SHADER_FILE})
endforeach()

# Get all compiled shader outputs
get_property(ALL_SHADER_OUTPUTS GLOBAL PROPERTY SHADER_OUTPUTS)

# Create shader compilation target with better reporting
add_custom_target(compile_shaders ALL
    DEPENDS ${ALL_SHADER_OUTPUTS}
    COMMENT "Compiling ${CMAKE_MATCH_COUNT} shader(s) to SPIR-V"
)

# Add shader validation target (optional, for development)
add_custom_target(validate_shaders
    COMMAND ${CMAKE_COMMAND} -E echo "Validating all shaders..."
    COMMENT "Running shader validation"
)

foreach(SHADER_FILE ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    add_custom_command(TARGET validate_shaders POST_BUILD
        COMMAND ${GLSLANG_VALIDATOR} -V --quiet ${SHADER_FILE}
        COMMENT "Validating ${SHADER_NAME}"
        VERBATIM
    )
endforeach()

# Make executable depend on shaders
add_dependencies(vulkan_game compile_shaders)

# Print shader compilation information
list(LENGTH SHADER_SOURCES SHADER_COUNT)
message(STATUS "Found ${SHADER_COUNT} shader file(s) to compile:")
foreach(SHADER_FILE ${SHADER_SOURCES})
    file(RELATIVE_PATH SHADER_REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_FILE})
    message(STATUS "  - ${SHADER_REL_PATH}")
endforeach()

# Make executable depend on shaders
add_dependencies(vulkan_game compile_shaders)

# Add target to clean compiled shaders
add_custom_target(clean_shaders
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Cleaning compiled shaders"
)

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Vulkan found: ${Vulkan_FOUND}")
message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "GLFW found: ${glfw3_FOUND}")
message(STATUS "GLSL Validator: ${GLSLANG_VALIDATOR}")
message(STATUS "Shader output directory: ${CMAKE_BINARY_DIR}/shaders")