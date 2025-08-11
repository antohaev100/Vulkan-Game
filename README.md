# Vulkan Game Engine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Educational](https://img.shields.io/badge/purpose-educational-blue.svg)]()
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-blue.svg)]()
[![Vulkan](https://img.shields.io/badge/Vulkan-1.0%2B-red.svg)](https://www.vulkan.org/)
[![Status](https://img.shields.io/badge/status-completed-green.svg)]()

A **completed educational project** demonstrating high-performance 3D graphics programming with the Vulkan API in C. This project serves as a comprehensive learning resource for modern graphics programming techniques, featuring real-time shadow mapping, multi-threaded architecture, and low-level GPU optimization.

> **� Educational Resource**: This project is complete and available for learning, study, and unrestricted use under MIT license. No ongoing maintenance or support is provided.

**🔓 Free to Use**: Fork, modify, learn from, and use in your own projects without restrictions.

## ✅ Implemented Features

### Graphics Rendering Engine
- **🎨 Vulkan API Integration**: Complete low-level graphics API implementation
- **🌑 Real-time Shadow Mapping**: Cube map shadows with multi-pass rendering
- **🎮 Dynamic Geometry**: Runtime generation of spheres, ellipsoids, cuboids, and cylinders
- **⚡ Memory Management**: Efficient GPU memory allocation and buffer management
- **🔧 Shader System**: GLSL to SPIR-V compilation with hot-reload support

### Engine Architecture
- **🧵 Multi-Threading**: Separate threads for rendering, physics, input, and audio
- **🎯 Cross-Platform**: Native support for Windows, Linux, and macOS
- **🛠️ Build System**: CMake with automatic shader compilation and dependency management
- **📊 Performance Monitoring**: Built-in timing and profiling capabilities
- **🔍 Debug Support**: Comprehensive Vulkan validation layers and error checking

### Learning Demonstrations
- **🎓 Modern C Programming**: C11 standard with clean, readable code
- **💡 Graphics Algorithms**: Shadow mapping, lighting, and rendering techniques
- **🏗️ Software Architecture**: Modular design with clear separation of concerns
- **⚙️ Performance Optimization**: GPU-focused optimization and memory management

## 📋 System Requirements

### Minimum Specifications
| Component | Requirement |
|-----------|-------------|
| **OS** | Windows 10, Linux (Ubuntu 18.04+), macOS 10.15+ |
| **Graphics** | Vulkan 1.0 compatible GPU |
| **GPU Examples** | NVIDIA GTX 600+, AMD Radeon HD 7000+, Intel HD 4000+ |
| **RAM** | 4 GB (8 GB recommended) |
| **Storage** | 200 MB available space |
| **Network** | Internet connection for dependency download |

### Development Requirements
| Tool | Version | Purpose |
|------|---------|---------|
| **CMake** | 3.16+ | Build system |
| **Vulkan SDK** | 1.0+ | Graphics API |
| **Compiler** | GCC 7.0+, Clang 6.0+, MSVC 2019+ | C11 compilation |
| **Git** | Latest | Version control and dependencies |

## 📦 Dependencies

This project uses carefully selected third-party libraries for optimal performance:

| Library | Purpose | Integration | License |
|---------|---------|-------------|---------|
| **Vulkan API** | Core graphics rendering | System/SDK | Apache 2.0 |
| **GLFW** | Window & input management | System library | zlib/libpng |
| **STB Image** | Image loading (textures) | Header-only | MIT/Public Domain |
| **Tiny OBJ Loader** | 3D model loading | Header-only | MIT |
| **CThreads** | Cross-platform threading | Source included | BSD 3-Clause |
| **Timer Library** | High-resolution timing | Source included | Public Domain |

> **📁 Dependency Management**: All dependencies are included in the repository or auto-discovered by CMake. See [`external/README.md`](external/README.md) for detailed information and [`THIRD_PARTY_LICENSES.md`](THIRD_PARTY_LICENSES.md) for complete license compliance.

## 🚀 Quick Start

### 1️⃣ Get the Code
```bash
git clone https://github.com/antohaev100/Vulkan-Game.git
cd Vulkan-Game
```

### 2️⃣ Install Prerequisites

<details>
<summary><b>🪟 Windows (MSYS2/MinGW-w64)</b></summary>

```bash
# Install MSYS2 from https://www.msys2.org/
# Open UCRT64 terminal and install dependencies:

pacman -S mingw-w64-ucrt-x86_64-vulkan-devel \
          mingw-w64-ucrt-x86_64-glfw \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-ninja \
          mingw-w64-ucrt-x86_64-gcc

# Ensure correct PATH
export PATH="/ucrt64/bin:$PATH"
```
</details>

<details>
<summary><b>🐧 Linux (Ubuntu/Debian)</b></summary>

```bash
# Install Vulkan SDK
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list \
    http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
sudo apt update && sudo apt install vulkan-sdk

# Install build tools and libraries
sudo apt install build-essential cmake ninja-build libglfw3-dev \
                 libvulkan1 mesa-vulkan-drivers vulkan-utils

# Verify installation
vulkaninfo --summary
```
</details>

<details>
<summary><b>🍎 macOS</b></summary>

```bash
# Install Vulkan SDK from: https://vulkan.lunarg.com/sdk/home
# Download and install the .dmg package

# Install dependencies via Homebrew
brew install cmake ninja glfw

# Add to shell profile (~/.zshrc or ~/.bash_profile)
export VULKAN_SDK="$HOME/VulkanSDK/x.x.x.x/macOS"
export PATH="$VULKAN_SDK/bin:$PATH"
export DYLD_LIBRARY_PATH="$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH"
export VK_ICD_FILENAMES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
```
</details>

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

### 3️⃣ Build & Run

```bash
# Configure build (choose one)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug    # Development with validation
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release  # Optimized performance

# Build project (includes automatic shader compilation)
cmake --build build --parallel

# Run the application
./build/vulkan_game        # Linux/macOS
build\vulkan_game.exe      # Windows
```

### 🎛️ Build Configuration Options

| Configuration | Purpose | Features |
|---------------|---------|----------|
| **Debug** | Development | Validation layers, debug symbols, error checking |
| **Release** | Performance | Optimized code, minimal validation |

### 🔨 Available Build Targets

```bash
cmake --build build                        # Build everything
cmake --build build --target validate_shaders  # Check shader syntax
cmake --build build --target clean_shaders     # Clean compiled shaders  
cmake --build build --target clean             # Clean all build files
```
## 💻 Development

### 📁 Project Architecture
```
vulkan_game/
├── 📁 src/                     # Core source code
│   ├── 🎨 vk/                 # Vulkan rendering engine
│   │   ├── vk_instance.c      # Vulkan initialization
│   │   ├── vk_device.c        # Device and queue management  
│   │   ├── vk_swapchain.c     # Presentation surface
│   │   ├── vk_pipeline.c      # Graphics pipeline creation
│   │   ├── vk_geometry.c      # Mesh generation and vertex data
│   │   ├── vk_shader.c        # Shader compilation and management
│   │   └── vk_*.c             # Additional Vulkan systems
│   ├── 🎮 input/              # Input handling (GLFW integration)
│   ├── ⚡ physics/            # Physics simulation and collision
│   ├── 🔧 utils/              # Utility functions and math
│   └── 🚀 main.c              # Application entry point
├── 📁 include/vulkan_game/    # Public headers and APIs
├── 📁 shaders/                # GLSL shader source files
│   ├── scene.vert/frag        # Main rendering shaders
│   └── shadow.vert/frag       # Shadow mapping shaders
├── 📁 external/               # Third-party dependencies
├── 📁 build/                  # Build output (auto-generated)
└── 📁 docs/                   # Documentation (if present)
```
│   ├── vk/                # Vulkan rendering engine
│   ├── input/             # Input handling (GLFW)
│   ├── physics/           # Physics simulation
│   ├── utils/             # Utility functions
### 🛠️ Development Guidelines

#### Code Standards
- **Language**: C11 standard compliance
- **Style**: Descriptive variable names, comprehensive error checking
- **Architecture**: Modular design with clear separation of concerns
- **Performance**: GPU-optimized algorithms and memory management

#### Shader Development
- **Auto-compilation**: `.vert`, `.frag` → `.spv` during build
- **Validation**: Built-in syntax checking with `validate_shaders` target
- **Hot-reload**: Modify shaders and rebuild for immediate changes
- **Debugging**: Use Vulkan validation layers for detailed error reporting

#### Vulkan Best Practices
- Always check function return values
- Properly destroy objects in reverse creation order
- Use validation layers during development
- Profile GPU memory usage and draw calls

## 🔧 Troubleshooting

<details>
<summary><b>🚫 Build Issues</b></summary>

**"glslangValidator not found"**
```bash
# Ensure Vulkan SDK is installed and accessible
echo $VULKAN_SDK                    # Should show SDK path
which glslangValidator              # Should find binary
export PATH="$VULKAN_SDK/bin:$PATH" # Add to PATH if needed
```

**"Vulkan library not found"**
```bash
# Verify Vulkan installation
vulkaninfo --summary               # Should show GPU information
pkg-config --libs vulkan           # Linux: check library paths

# Reconfigure CMake with clean cache
rm -rf build && cmake -B build -DCMAKE_BUILD_TYPE=Debug
```
</details>

<details>
<summary><b>⚡ Runtime Issues</b></summary>

**Application crashes on startup**
```bash
# Enable detailed error reporting
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/vulkan_game  # Check console output for errors
```

**"Vulkan device not found"**
- Update GPU drivers to latest version
- Install platform-specific Vulkan runtime:
  - **Windows**: Included with driver updates
  - **Linux**: `mesa-vulkan-drivers` or vendor drivers
  - **macOS**: MoltenVK (included with Vulkan SDK)

**Performance issues**
```bash
# Build optimized version
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
</details>

<details>
<summary><b>🖥️ Platform-Specific Issues</b></summary>

**Windows (MSYS2)**
- Use **UCRT64** environment (not MINGW64)
- Ensure PATH includes `/ucrt64/bin`
- Install packages with `mingw-w64-ucrt-x86_64-` prefix

**Linux**
- Install both development headers and runtime libraries
- For Wayland: May need additional libraries (`libwayland-dev`)
- For older distros: Use AppImage Vulkan SDK

**macOS**
- Vulkan runs through MoltenVK translation layer
- Ensure `VK_ICD_FILENAMES` environment variable is set
</details>

## 📚 Learning from this Project

### 🎓 Educational Value
This completed project serves as a comprehensive **learning resource** for graphics programming:

- **Study the Implementation**: Explore modern Vulkan API usage patterns
- **Understand Architecture**: See how to structure a multi-threaded graphics engine
- **Learn Optimization**: Analyze GPU memory management and performance techniques
- **Reference Material**: Use as a starting point for your own graphics projects

### � Free to Use & Extend
- **Fork Freely**: Create your own version without restrictions (MIT License)
- **Copy Code**: Take any parts you find useful for your projects
- **Commercial Use**: No limitations - use in personal or commercial applications
- **Educational**: Perfect for learning, teaching, or portfolio demonstration

### 🚀 Extension Ideas
- **Advanced Rendering**: PBR materials, post-processing effects, global illumination
- **VR Support**: OpenXR integration for virtual reality applications
- **Asset Pipeline**: Model loading, texture streaming, material systems
- **Editor Tools**: Scene editing, debugging tools, profiling integration

> **📖 See [`CONTRIBUTING.md`](CONTRIBUTING.md) for detailed learning guidance and code exploration tips.**

## 📋 Project Status

This project is **complete** as an educational resource demonstrating:

**✅ Implemented Features**: Vulkan rendering engine, shadow mapping, multi-threading, cross-platform support  
**🎓 Learning Objectives**: Modern graphics programming, GPU optimization, engine architecture  
**🔒 Maintenance Status**: No ongoing development - available for study and extension

> **📊 See [`SCOPE.md`](SCOPE.md) for comprehensive technical documentation and implementation details.**

## 📄 License

This project is licensed under the **MIT License** - see [`LICENCE.md`](LICENCE.md) for details.

### Third-Party Licenses
Third-party dependencies have their own licenses. Complete license information and compliance details are available in [`THIRD_PARTY_LICENSES.md`](THIRD_PARTY_LICENSES.md).

## 🙏 Acknowledgments

- **Vulkan Community**: For comprehensive documentation and support
- **Khronos Group**: For the Vulkan API specification and tools
- **Library Authors**: Sean Barrett (STB), GLFW team, and all open-source contributors
- **Graphics Programming Community**: For tutorials, examples, and shared knowledge

---

<div align="center">

**🎮 Built with ❤️ for the graphics programming community**

[![GitHub stars](https://img.shields.io/github/stars/antohaev100/Vulkan-Game?style=social)](https://github.com/antohaev100/Vulkan-Game)
[![GitHub forks](https://img.shields.io/github/forks/antohaev100/Vulkan-Game?style=social)](https://github.com/antohaev100/Vulkan-Game/fork)

*This is a completed educational project showcasing modern graphics programming.  
Available for learning, study, and unrestricted use under MIT license.*

</div>
