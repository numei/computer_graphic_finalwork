# Computer Graphics Final Work - OpenGL Project

An OpenGL-based game project built with C++17, GLFW, GLAD, and GLM.

## Project Structure

```
computer_graphic_finalwork/
├── opengl/              # Main OpenGL project
│   ├── src/            # Source files
│   ├── include/        # Header files (GLFW, GLM, GLAD)
│   ├── lib/            # Library files (GLFW)
│   ├── shaders/        # Shader files
│   ├── asserts/        # Assets (fonts, etc.)
│   ├── CMakeLists.txt  # CMake configuration
│   ├── build_windows.bat  # Windows build script
│   ├── install_glfw.bat   # GLFW installation helper (Windows)
│   └── execute.sh      # macOS/Linux build script
└── blender/            # Blender model files
```

## Prerequisites

### Windows

1. **CMake** (version 3.11 or higher)
   - Download from: https://cmake.org/download/
   - During installation, check "Add CMake to system PATH"
   - Or use Visual Studio 2019/2022 (includes CMake support)

2. **C++ Compiler**
   - Visual Studio 2019/2022 with "Desktop development with C++" workload
   - Or MinGW-w64

3. **GLFW Library** (Windows version)
   - The project includes an installation script: `opengl/install_glfw.bat`
   - Or download manually from: https://www.glfw.org/download.html

### macOS

1. **CMake**
   ```bash
   brew install cmake
   ```

2. **Xcode Command Line Tools**
   ```bash
   xcode-select --install
   ```

3. **GLFW Library**
   ```bash
   brew install glfw
   ```
   Or use the pre-built library in `opengl/lib/` (macOS version included)

### Linux

1. **CMake**
   ```bash
   sudo apt-get install cmake
   ```

2. **Build Tools**
   ```bash
   sudo apt-get install build-essential
   ```

3. **GLFW Library**
   ```bash
   sudo apt-get install libglfw3-dev
   ```

## Building the Project

### Windows

#### Method 1: Using Build Script (Recommended)

1. Open PowerShell or Command Prompt
2. Navigate to the project directory:
   ```powershell
   cd opengl
   ```
3. Run the build script:
   ```powershell
   .\build_windows.bat
   ```
   The script will:
   - Check for CMake
   - Configure the project
   - Build in Release mode
   - Run the executable

#### Method 2: Using Visual Studio

1. Open Visual Studio 2019/2022
2. Select `File` → `Open` → `CMake...`
3. Navigate to and select `opengl/CMakeLists.txt`
4. Visual Studio will automatically configure the project
5. Press `F5` to build and run, or:
   - `Build` → `Build All` (Ctrl+Shift+B)
   - `Build` → `Rebuild All` (for clean build)

#### Method 3: Manual Build

1. Open PowerShell in the `opengl` directory
2. Create build directory:
   ```powershell
   mkdir build
   cd build
   ```
3. Configure with CMake:
   ```powershell
   cmake ..
   ```
4. Build:
   ```powershell
   cmake --build . --config Release
   ```
5. Run:
   ```powershell
   .\Release\HelloGL.exe
   ```

### macOS

#### Method 1: Using Build Script

1. Open Terminal
2. Navigate to the project directory:
   ```bash
   cd opengl
   ```
3. Make the script executable (first time only):
   ```bash
   chmod +x execute.sh
   ```
4. Run the build script:
   ```bash
   ./execute.sh
   ```

#### Method 2: Manual Build

1. Open Terminal in the `opengl` directory
2. Create build directory:
   ```bash
   mkdir -p build
   cd build
   ```
3. Configure with CMake:
   ```bash
   cmake ..
   ```
4. Build:
   ```bash
   make
   ```
5. Run:
   ```bash
   ./HelloGL
   ```

### Linux

Follow the same steps as macOS (Method 2: Manual Build).

## Installing GLFW (Windows)

If you encounter GLFW linking errors on Windows:

### Automatic Installation

1. Run the installation script:
   ```powershell
   cd opengl
   .\install_glfw.bat
   ```
2. Choose option 1 for automatic download
3. The script will download and install GLFW Windows libraries

### Manual Installation

1. Download GLFW from: https://www.glfw.org/download.html
   - Select "Windows pre-compiled binaries"
   - Or from: https://github.com/glfw/glfw/releases
   - Download `glfw-3.4.bin.WIN64.zip` (or latest version)

2. Extract the archive

3. Copy files to project:
   - `lib-vc2022/glfw3.lib` → `opengl/lib/glfw3.lib`
   - `lib-vc2022/glfw3.dll` → `opengl/lib/glfw3.dll`
   - `include/GLFW/glfw3.h` → `opengl/include/GLFW/glfw3.h`

4. Reconfigure CMake project in Visual Studio

## Dependencies

The project uses the following libraries (included in `opengl/lib/` and `opengl/include/`):

- **GLFW 3.4**: Window and input management
- **GLAD**: OpenGL function loader
- **GLM**: OpenGL Mathematics library
- **stb_truetype**: Font rendering

## Controls

- **W/A/S/D**: Move player
- **Mouse**: Look around (first-person mode)
- **V**: Toggle first-person/third-person camera
- **Mouse Wheel**: Zoom (third-person mode)
- **ESC**: Exit game

## Troubleshooting

### Windows: "CMake command not found"

- Install CMake and add it to PATH
- Or use Visual Studio which includes CMake support
- Restart PowerShell/Command Prompt after installing CMake

### Windows: GLFW linking errors

- Run `opengl/install_glfw.bat` to install Windows GLFW libraries
- Ensure `glfw3.lib` and `glfw3.dll` are in `opengl/lib/`
- Reconfigure CMake project after installing GLFW

### macOS: Library not found errors

- Install GLFW via Homebrew: `brew install glfw`
- Or ensure `libglfw.3.dylib` exists in `opengl/lib/`

### Build errors after modifying CMakeLists.txt

- Delete `build/` or `out/` folder
- Reconfigure CMake project
- Rebuild from scratch

### Visual Studio: CMake configuration errors

- Ensure CMake is installed and in PATH
- Check that all required libraries are in `opengl/lib/`
- Try deleting `opengl/out/` folder and reopening the project

## Project Features

- OpenGL 3.3 Core Profile rendering
- First-person and third-person camera modes
- Text rendering with custom font
- Game UI system
- Player movement and collision detection
- Falling object physics simulation

## Notes

- The project currently uses simple cube primitives for rendering
- Blender models (`.blend` files) need to be exported to OBJ/FBX format for use in OpenGL
- Shader files are in `opengl/shaders/`
- Assets (fonts) are in `opengl/asserts/`

## License

See LICENSE file for details.
