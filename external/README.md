# Third-Party Dependencies

This directory contains external libraries used by the Vulkan Game project. Each library has its own license and attribution requirements.

## Directory Structure

```
external/
├── CThreads/          # Cross-platform threading library
├── stb/               # STB single-file libraries
├── timer_lib/         # High-resolution timer library
└── tinyobjloader/     # Tiny OBJ file loader
```

## Libraries

### CThreads
- **Description**: Cross-platform threading library
- **Version**: Latest (2024)
- **Author**: The PerformanC Organization
- **License**: BSD 3-Clause License
- **Repository**: https://github.com/PerformanC/CThreads
- **Files**: `CThreads/cthreads.c`, `CThreads/cthreads.h`
- **License File**: `CThreads/LICENSE`

### STB Image
- **Description**: Image loading/decoding from file/memory (JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC)
- **Version**: v2.30
- **Author**: Sean Barrett
- **License**: MIT License / Public Domain (dual license - using MIT)
- **Repository**: https://github.com/nothings/stb
- **Files**: `stb/stb_image.h`
- **Website**: http://nothings.org/stb

### Timer Library
- **Description**: Cross-platform high-resolution timer for performance measurement
- **Version**: v1.0
- **Author**: Mattias Jansson
- **License**: Public Domain
- **Repository**: https://github.com/mjansson/timer_lib
- **Files**: `timer_lib/timer.c`, `timer_lib/timer.h`
- **Documentation**: `timer_lib/README`

### Tiny OBJ Loader
- **Description**: Single-file Wavefront OBJ loader (header-only)
- **Version**: v2.0.0
- **Author**: Syoyo Fujita and contributors
- **License**: MIT License
- **Repository**: https://github.com/tinyobjloader/tinyobjloader
- **Files**: `tinyobjloader/tiny_obj_loader.h`

## License Compliance

This project complies with all third-party license requirements. See individual license files in each subdirectory for full license text and terms.

**For complete license information, see [`THIRD_PARTY_LICENSES.md`](../THIRD_PARTY_LICENSES.md) in the project root.**

## Dependency Management Options

Currently, dependencies are managed by including source files directly in the repository. For future improvements, consider these alternatives:

### Option 1: Git Submodules (Recommended for libraries like this)
```bash
# Convert to git submodules
git submodule add https://github.com/PerformanC/CThreads.git external/CThreads
git submodule add https://github.com/nothings/stb.git external/stb
git submodule add https://github.com/mjansson/timer_lib.git external/timer_lib
git submodule add https://github.com/tinyobjloader/tinyobjloader.git external/tinyobjloader

# Clone with submodules
git clone --recursive <your-repo-url>

# Update submodules
git submodule update --remote
```

**Pros**: Version control integration, easy updates, clear dependency tracking  
**Cons**: Requires additional git commands, can complicate builds

### Option 2: Package Managers

#### CPM.cmake (CMake Package Manager)
```cmake
include(cmake/CPM.cmake)

CPMAddPackage(
  NAME stb
  GITHUB_REPOSITORY nothings/stb
  GIT_TAG master
  DOWNLOAD_ONLY YES
)

if(stb_ADDED)
  add_library(stb INTERFACE)
  target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()
```

#### vcpkg (Microsoft C++ Library Manager)
```bash
vcpkg install glfw3 vulkan-memory-allocator
```

#### Conan (C++ Package Manager)
```python
[requires]
glfw/3.3.8
vulkan-headers/1.3.268

[generators]
CMakeDeps
CMakeToolchain
```

**Pros**: Automatic dependency resolution, version management, binary caching  
**Cons**: Additional tools required, potential complexity for simple projects

### Option 3: FetchContent (Modern CMake)
```cmake
include(FetchContent)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG master
)

FetchContent_MakeAvailable(stb)
```

**Pros**: Built into CMake, automatic downloading, good for header-only libraries  
**Cons**: Download time on first build, requires internet connection

### Current Approach: Direct Inclusion
**Pros**: Simple, no external dependencies, works offline, guaranteed compatibility  
**Cons**: Manual updates, larger repository size, potential license tracking complexity

### Recommendation
For this Vulkan game project, the current direct inclusion approach is acceptable because:
- Small number of dependencies
- Mostly header-only libraries
- Stable, infrequently updated libraries
- Simplified build process for contributors

Consider migrating to **git submodules** if:
- Dependencies need frequent updates
- Repository size becomes a concern
- You want clearer separation of third-party code

## Adding New Dependencies

When adding new external dependencies:

1. Place them in appropriately named subdirectories under `external/`
2. Update this README with library information
3. Update `CMakeLists.txt` to include new paths
4. Ensure license compliance and attribution
