# Ugaris C++ Demo Mod

A native mod written in modern C++ (C++17) demonstrating the Ugaris Client mod API. This showcases C++ features like classes, constexpr, atomics, and type-safe abstractions.

## Features

- **Modern C++17** - Uses constexpr, std::atomic, and modern idioms
- **Type Safety** - Strongly typed interfaces and helper classes
- **Cross-Platform** - CMake-based build for all platforms

## Commands

| Command | Description |
|---------|-------------|
| `#hello` | Display available commands |
| `#stats` | Show current player stats |
| `#overlay` | Toggle the HUD overlay |

## Installation

### Via Ugaris Launcher

1. Open the Ugaris Launcher
2. Go to **Options > Developer > Enable Mod Manager**
3. Navigate to the **Mods** section
4. Click **Install from URL**
5. Enter: `ugaris/ugaris-cpp-demo-mod`

## Building from Source

### Requirements

- CMake 3.16+
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Build Commands

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release
```

### Platform-Specific

**macOS Universal Binary:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
cmake --build build
```

## Project Structure

```
ugaris-cpp-demo-mod/
├── .github/workflows/build.yml
├── src/
│   └── main.cpp              # Main implementation
├── CMakeLists.txt            # Build configuration
├── mod.json
├── README.md
└── LICENSE
```

## C++ Features Demonstrated

### Constexpr Color Calculations

```cpp
constexpr uint16_t IRGB(uint16_t r, uint16_t g, uint16_t b) {
    return (r << 10) | (g << 5) | b;
}
```

### Thread-Safe State with Atomics

```cpp
std::atomic<bool> show_overlay{false};
std::atomic<uint32_t> frame_count{0};
```

### RAII Helper Classes

```cpp
class FormattedString {
    char buffer_[256];
public:
    template<typename... Args>
    FormattedString(const char* fmt, Args... args) {
        std::snprintf(buffer_, sizeof(buffer_), fmt, args...);
    }
    operator const char*() const { return buffer_; }
};
```

## Why C++ for Mods?

1. **Familiar** - Most game developers know C++
2. **Powerful** - Full access to OOP, templates, and the STL
3. **Compatible** - Easy interop with C APIs via `extern "C"`
4. **Performant** - Zero-overhead abstractions

## License

MIT License - See [LICENSE](LICENSE) file for details.
