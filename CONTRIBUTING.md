# Learning from Vulkan Game

This is an **educational project** created for learning low-level graphics programming with Vulkan. The project is complete as a learning exercise and is **no longer actively maintained**.

## 📚 Project Purpose

This codebase serves as a **reference implementation** for:
- Modern Vulkan API usage in C
- Real-time shadow mapping techniques  
- Multi-threaded game engine architecture
- Cross-platform graphics development
- Low-level GPU programming concepts

## 🎯 How to Use This Project

### For Learning
- **Study the code**: Explore the Vulkan rendering pipeline implementation
- **Run and modify**: Build the project and experiment with changes
- **Reference material**: Use as a starting point for your own graphics projects
- **Educational resource**: Understand real-world Vulkan application structure

### For Your Projects
- **Fork freely**: Create your own version without restrictions
- **Copy code**: Take any parts you find useful (MIT licensed)
- **Extend features**: Add new rendering techniques or game mechanics
- **Commercial use**: No restrictions - use in personal or commercial projects

## 🔧 Getting Started

### Build the Project
```bash
# Clone and build
git clone https://github.com/antohaev100/Vulkan-Game.git
cd Vulkan-Game

# Configure and build (see README.md for platform-specific setup)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/vulkan_game
```

### Explore the Codebase
```
src/vk/          # Vulkan rendering engine - start here
├── vk_instance.c    # Vulkan initialization
├── vk_pipeline.c    # Graphics pipeline and shaders
├── vk_geometry.c    # Mesh generation and rendering
└── vk_active.c      # Main render loop and shadow mapping

src/physics/     # Basic physics and collision detection
src/input/       # GLFW input handling
src/utils/       # Math utilities and helper functions
```

## 💡 Learning Opportunities

### Beginner Level
- **Basic Vulkan setup**: Instance, device, and swapchain creation
- **Shader compilation**: GLSL to SPIR-V pipeline
- **Vertex data**: Buffer creation and vertex input descriptions
- **Command buffers**: Recording and submitting GPU commands

### Intermediate Level
- **Multi-pass rendering**: Shadow mapping implementation
- **Dynamic geometry**: Runtime mesh generation
- **Memory management**: GPU memory allocation strategies
- **Multi-threading**: Render, physics, and input thread separation

### Advanced Level
- **Performance optimization**: GPU profiling and bottleneck analysis
- **Platform abstraction**: Cross-platform graphics development
- **Engine architecture**: Component-based system design
- **Vulkan debugging**: Validation layers and error handling

## 🚀 Extending the Project

### Potential Improvements
- **Advanced lighting**: PBR materials, global illumination
- **Post-processing**: Bloom, tone mapping, anti-aliasing
- **Asset loading**: Texture streaming, model formats
- **Scene management**: Frustum culling, LOD systems
- **Audio integration**: 3D positional audio
- **VR support**: OpenXR integration

### Architecture Enhancements
- **Entity-Component-System**: More flexible object management
- **Asset pipeline**: Hot-reload capabilities
- **Scripting system**: Lua or similar integration
- **Editor tools**: Scene editing and debugging tools

## 📖 Code Standards (Reference)

The project follows these conventions:

### C Code Style
- **C11 standard** compliance
- **Descriptive naming** for variables and functions
- **Comprehensive error checking** for all Vulkan calls
- **Modular design** with clear separation of concerns

### Vulkan Patterns
- Proper object lifetime management
- Validation layer integration
- Resource creation/destruction order
- Command buffer recording patterns

## 🤝 Community Usage

### No Maintenance Commitment
- **Use at your own risk**: No guarantees of updates or bug fixes
- **Community-driven**: Feel free to create your own forks and improvements
- **Learning resource**: Intended for educational purposes

### How to Get Help
- **Study the code**: Most questions can be answered by reading the implementation
- **Vulkan documentation**: Official Khronos Vulkan specification and tutorials
- **Graphics communities**: Reddit r/GraphicsProgramming, Discord servers, forums
- **Create your own issues**: Use GitHub issues in your fork for your own development

### Sharing Your Work
- **Fork and extend**: Create your own improved versions
- **Share knowledge**: Write tutorials or blog posts about what you learned
- **Reference this project**: Link back if you found it helpful
- **Give credit**: Attribution appreciated but not required (MIT license)

## 📄 License

This project is released under the **MIT License** - see [LICENCE.md](LICENCE.md).

**You are free to:**
- Use the code for any purpose (commercial or personal)
- Modify and distribute the code
- Create derivative works
- Use portions of the code in other projects

**No warranty or support is provided.**

---

**Happy learning and building! 🚀**

*This project demonstrates that educational code can be both learning-focused and production-quality.*