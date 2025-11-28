# xe_xerces_example

[Korean README](README.ko.md)

## Overview
This project demonstrates how to validate an XML file against an XSD schema using **Xerces-C++** and **spdlog**.

The `CMakeLists.txt` is written to support:

- **Windows / MSVC (Visual Studio + vcpkg, `x64-windows-static`)**
- **Windows / MinGW-w64 (Qt / g++ + vcpkg, `x64-mingw-static`)**
- **Linux (Rocky / Ubuntu, system packages via `pkg-config`)** fileciteturn0file0

## Features
- XML validation using Xerces-C++
- Custom SAX2 error handler
- Logging through spdlog
- Cross-platform CMake configuration (MSVC, MinGW, Linux)

## File Structure
- `main.cpp` – entry point fileciteturn0file1L1-L18  
- `xml_validator_app.*` – validation controller fileciteturn0file5L1-L112  
- `simple_error_handler.*` – SAX2 error handler fileciteturn0file3L1-L109  
- `person_int_schema.xsd` – sample schema  
- `person_invalid.xml` – intentionally invalid sample XML fileciteturn0file2L1-L6  

---

## 1. Build Environment Setup

### 1.1 Windows – Visual Studio (MSVC) + vcpkg

The CMake script assumes vcpkg is installed under `%VCPKG_ROOT%` or `%USERPROFILE%\vcpkg`, and uses the triplet **`x64-windows-static`**. fileciteturn0file0L7-L32

1. Clone and bootstrap vcpkg (PowerShell):
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git "$env:USERPROFILE/vcpkg"
   cd "$env:USERPROFILE/vcpkg"
   bootstrap-vcpkg.bat
   ```

2. Install required libraries:
   ```powershell
   vcpkg.exe install xerces-c:x64-windows-static spdlog:x64-windows-static
   ```

3. (Optional) Enable user-wide integration:
   ```powershell
   vcpkg.exe integrate install
   ```

4. Configure & build with CMake (from project root):
   ```powershell
   cmake -B build -G "Visual Studio 17 2022" -A x64 ^
         -DCMAKE_TOOLCHAIN_FILE="$env:USERPROFILE/vcpkg/scripts/buildsystems/vcpkg.cmake"
   cmake --build build --config Debug
   ```

Visual Studio can also open the CMake project directly and will respect the `CMAKE_TOOLCHAIN_FILE` setting if configured in **CMakePresets.json** or via the IDE.

### 1.2 Windows – MinGW-w64 (Qt / g++) + vcpkg

For MinGW, the CMake script uses the vcpkg triplet **`x64-mingw-static`**, again under `%VCPKG_ROOT%` or `%USERPROFILE%\vcpkg`. fileciteturn0file0L34-L58

1. Clone and bootstrap vcpkg (same location as above):
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git "$env:USERPROFILE/vcpkg"
   cd "$env:USERPROFILE/vcpkg"
   bootstrap-vcpkg.bat
   ```

2. Install MinGW-w64 triplet libraries:
   ```powershell
   vcpkg.exe install xerces-c:x64-mingw-static spdlog:x64-mingw-static
   ```

3. Configure & build using MinGW CMake / Qt CMake (example with Qt MinGW kit):
   ```bash
   cmake -B build-mingw -G "MinGW Makefiles" ^
         -DCMAKE_TOOLCHAIN_FILE=%USERPROFILE%/vcpkg/scripts/buildsystems/vcpkg.cmake ^
         -DVCPKG_TARGET_TRIPLET=x64-mingw-static
   cmake --build build-mingw
   ```

Qt Creator에서 MinGW Kit을 선택한 뒤, 위와 동일한 `CMAKE_TOOLCHAIN_FILE` 과 `VCPKG_TARGET_TRIPLET` 을 설정하면 CMakeLists.txt 의 MinGW 분기가 자동으로 적용됩니다.

### 1.3 Linux – Rocky Linux / Ubuntu

On Linux, the project does **not** use vcpkg. Instead it relies on **pkg-config** to find system-installed packages `xerces-c` and `spdlog`. fileciteturn0file0L60-L86

#### 1.3.1 Rocky Linux (RHEL)

Install development packages (root or sudo):

```bash
sudo dnf install -y     xerces-c-devel     spdlog spdlog-devel     pkgconfig gcc-c++ make
```

Then configure and build:

```bash
cmake -B build
cmake --build build
```

#### 1.3.2 Ubuntu / Debian

Install development packages:

```bash
sudo apt update
sudo apt install -y     libxerces-c-dev     libspdlog-dev     pkg-config g++ make
```

Then configure and build:

```bash
cmake -B build
cmake --build build
```

---

## 2. Basic Build (All Platforms)

Once dependencies are installed and CMake can find Xerces-C++ / spdlog, the build is simply:

```bash
cmake -B build
cmake --build build
```

---

## 3. How it Works

1. Loads XML / XSD paths (from project root) fileciteturn0file1L4-L18  
2. Initializes Xerces-C++ (once per process) fileciteturn0file5L23-L40  
3. Configures SAX2 parser with schema checking and XSD location  
4. Parses the XML and collects validation errors through `simple_error_handler`  
5. Returns an `xml_validator_result` exit code fileciteturn0file6L9-L24  

