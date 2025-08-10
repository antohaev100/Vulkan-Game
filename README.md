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

**Required Components:**
- Vulkan SDK 1.3+ with validation layers
- GLFW 3.4+
- CMake 3.16+
- C compiler with C11 support

### Platform-Specific Setup

#### Windows (MSYS2/MinGW-w64) - Recommended
```bash
# Install MSYS2 from https://www.msys2.org/
# Open UCRT64 terminal and install dependencies:

pacman -S mingw-w64-ucrt-x86_64-vulkan-devel
pacman -S mingw-w64-ucrt-x86_64-glfw
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-ninja
pacman -S mingw-w64-ucrt-x86_64-gcc

# Set environment
export PATH="/ucrt64/bin:$PATH"
```

#### Linux (Ubuntu/Debian)
```bash
# Install Vulkan SDK
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
sudo apt update
sudo apt install vulkan-sdk

# Install development tools and libraries
sudo apt install build-essential cmake ninja-build
sudo apt install libglfw3-dev
sudo apt install libvulkan1 mesa-vulkan-drivers vulkan-utils

# Verify installation
vulkaninfo --summary
```

#### macOS
```bash
# Install Vulkan SDK
# Download from: https://vulkan.lunarg.com/sdk/home
# Install the .dmg package

# Install dependencies via Homebrew
brew install cmake ninja glfw

# Add to shell profile (~/.zshrc or ~/.bash_profile)
export VULKAN_SDK="$HOME/VulkanSDK/x.x.x.x/macOS"
export PATH="$VULKAN_SDK/bin:$PATH"
export DYLD_LIBRARY_PATH="$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH"
export VK_ICD_FILENAMES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
export VK_LAYER_PATH="$VULKAN_SDK/share/vulkan/explicit_layer.d"

# Reload shell
source ~/.zshrc  # or ~/.bash_profile
```

### Build Instructions

1. **Clone and Navigate**
   ```bash
   git clone https://github.com/antohaev100/Vulkan-Game.git
   cd Vulkan-Game
   ```

2. **Configure Build**
   ```bash
   # Debug build with validation layers (recommended for development)
   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
   
   # Release build (optimized for performance)
   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
   
   # Alternative without Ninja (uses system default)
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   ```

3. **Build the Project**
   ```bash
   # Build everything (including automatic shader compilation)
   cmake --build build
   
   # Parallel build (faster)
   cmake --build build --parallel
   ```

4. **Run the Game**
   ```bash
   # Windows
   build\vulkan_game.exe
   
   # Linux/macOS
   ./build/vulkan_game
   ```

### Build Configuration Options

- **Debug Mode**: Enables Vulkan validation layers, detailed error checking, and debug symbols
- **Release Mode**: Optimized performance, minimal validation
- **Shader Auto-compilation**: All `.vert` and `.frag` shaders automatically compile to `.spv`
- **Cross-platform**: Same build commands work on Windows, Linux, and macOS

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
- Descriptive variable names
- Comprehensive error checking for Vulkan calls

## Troubleshooting

### Common Issues

#### Build Problems

**"glslangValidator not found"**
- Ensure Vulkan SDK is installed and `VULKAN_SDK` environment variable is set
- Windows: Add `%VULKAN_SDK%\Bin` to PATH
- Linux/macOS: Add `$VULKAN_SDK/bin` to PATH

**"Vulkan library not found"**
- Verify Vulkan SDK installation
- Check that `vulkaninfo` command works
- On Linux: Install mesa-vulkan-drivers
- On macOS: Ensure MoltenVK is properly configured

**CMake configuration fails**
```bash
# Clear cache and reconfigure
rm -rf build/
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

#### Runtime Problems

**"Vulkan device not found" or validation layer errors**
- Update graphics drivers to latest version
- For integrated Intel graphics: Install Intel Graphics Driver
- For NVIDIA: Install latest Game Ready drivers
- For AMD: Install latest Adrenalin drivers

**Application crashes on startup**
- Run with validation layers for detailed error information:
  ```bash
  # Debug build automatically enables validation layers
  cmake -B build -DCMAKE_BUILD_TYPE=Debug
  cmake --build build
  ./build/vulkan_game
  ```

#### Platform-Specific Issues

**Windows (MSYS2):**
- Use UCRT64 environment, not MINGW64
- Ensure PATH includes `/ucrt64/bin`
- Install packages with `mingw-w64-ucrt-x86_64-` prefix

**Linux:**
- Install both development headers and runtime libraries
- For Wayland: May need additional Wayland support libraries
- For older distributions: Use AppImage Vulkan SDK

**macOS:**
- Vulkan runs through MoltenVK translation layer
- Some Vulkan features may be unsupported
- Ensure VK_ICD_FILENAMES points to MoltenVK

### Getting Help

1. Check the console output for detailed error messages
2. Verify your system meets the minimum requirements
3. Test with validation layers enabled (Debug build)
4. Check GitHub Issues for similar problems
5. Run `vulkaninfo` to verify Vulkan installation

### Performance Optimization

**For better performance:**
- Use Release build configuration
- Update to latest graphics drivers
- Close unnecessary background applications
- On laptops: Use discrete GPU if available
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