# Vulkan Game

A high-performance 3D game built from scratch in C using the Vulkan API. This project demonstrates modern graphics programming techniques with low-level GPU control, featuring advanced rendering capabilities including shadow mapping, physics simulation, and multi-threaded architecture.

## Features

- **Vulkan Rendering**: Direct GPU programming with Vulkan API for maximum performance
- **Shadow Mapping**: Real-time dynamic shadows for enhanced visual quality
- **Physics Engine**: Custom physics simulation with collision detection
- **Multi-threaded Architecture**: Separate threads for rendering, input, physics, and audio
- **Cross-platform Support**: Windows, Linux, and macOS compatibility
- **Modern Build System**: CMake with automatic shader compilation and dependency management

## System Requirements

### Minimum Requirements
- **OS**: Windows 10, Linux (Ubuntu 18.04+), or macOS 10.15+
- **Graphics**: Vulkan 1.0 compatible GPU
  - NVIDIA GeForce GTX 600 series or newer
  - AMD Radeon HD 7000 series or newer
  - Intel HD Graphics 4000 or newer (with driver support)
- **RAM**: 4 GB minimum, 8 GB recommended
- **Storage**: 100 MB available space

### Development Requirements
- **Compiler**: GCC 7.0+, Clang 6.0+, or MSVC 2019+
- **CMake**: Version 3.16 or newer
- **Vulkan SDK**: Version 1.0 or newer
- **Git**: For cloning and submodule management

## Dependencies

This project uses the following third-party libraries:
- **Vulkan**: Graphics API
- **GLFW**: Window management and input handling
- **STB Image**: Image loading (header-only)
- **Tiny OBJ Loader**: 3D model loading (header-only)
- **CThreads**: Cross-platform threading
- **Timer Library**: High-resolution timing

All dependencies are either included in the repository or automatically discovered by CMake.

## Building

### Prerequisites

1. **Install Vulkan SDK**
   ```bash
   # Download from https://vulkan.lunarg.com/
   # Set VULKAN_SDK environment variable (usually automatic)
   ```

2. **Install GLFW**
   - **Windows**: Download from https://www.glfw.org/download.html
   - **Linux**: `sudo apt install libglfw3-dev` (Ubuntu/Debian)
   - **macOS**: `brew install glfw` (Homebrew)

3. **Install Build Tools**
   ```bash
   # Windows (with MSYS2)
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
   
   # Linux
   sudo apt install build-essential cmake
   
   # macOS
   xcode-select --install
   brew install cmake
   ```

### Build Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/antohaev100/Vulkan-Game.git
   cd Vulkan-Game
   ```

2. **Create Build Directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**
   ```bash
   # Debug build (default)
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   
   # Release build (optimized)
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

4. **Build the Project**
   ```bash
   # Build everything (including shaders)
   cmake --build .
   
   # Or use make on Linux/macOS
   make -j$(nproc)
   ```

5. **Run the Game**
   ```bash
   # Windows
   .\vulkan_game.exe
   
   # Linux/macOS
   ./vulkan_game
   ```

### Build Targets

The build system provides several useful targets:

```bash
# Build everything (default)
cmake --build .

# Validate all shaders
cmake --build . --target validate_shaders

# Clean compiled shaders
cmake --build . --target clean_shaders

# Clean everything
cmake --build . --target clean
```

## Development

### Project Structure
```
├── src/                    # Source code
│   ├── vk/                # Vulkan rendering engine
│   ├── input/             # Input handling (GLFW)
│   ├── physics/           # Physics simulation
│   ├── utils/             # Utility functions
│   └── main.c             # Application entry point
├── include/vulkan_game/   # Public headers
├── shaders/               # GLSL shader source files
├── external/              # Third-party dependencies
└── build/                 # Build output (generated)
```

### Shader Development
Shaders are automatically compiled to SPIR-V during build:
- Edit `.vert`, `.frag`, `.comp`, `.geom`, `.tesc`, or `.tese` files in `shaders/`
- Compiled shaders are output to `build/shaders/`
- Use `validate_shaders` target to check for errors without full build

### Code Style
- C11 standard
- 4-space indentation
- Descriptive variable names
- Comprehensive error checking for Vulkan calls

## Troubleshooting

### Common Issues

**"glslangValidator not found"**
- Ensure Vulkan SDK is installed and `VULKAN_SDK` environment variable is set
- Add Vulkan SDK bin directory to your PATH

**"Vulkan not found"**
- Install graphics drivers with Vulkan support
- Verify GPU compatibility with `vulkaninfo` command

**"GLFW not found"**
- Install GLFW development libraries
- On Windows, ensure GLFW is in a standard location or set CMAKE_PREFIX_PATH

**Runtime crashes**
- Enable Vulkan validation layers in debug builds
- Check that shaders compiled successfully
- Verify GPU has sufficient memory

### Getting Help
- Check [Issues](https://github.com/antohaev100/Vulkan-Game/issues) for known problems
- Create a new issue with system information and error logs
- Include CMake configuration output and build logs

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the terms specified in [LICENCE.md](LICENCE.md).

Third-party dependencies have their own licenses - see [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md) for details.

## Acknowledgments

- Vulkan community and documentation
- STB libraries by Sean Barrett
- GLFW development team
- All contributors to the third-party libraries used

---

**Note**: This is a learning project focused on low-level graphics programming. It prioritizes educational value and performance over ease of use.