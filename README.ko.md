# xe_xerces_example

[English README](README.md)

## 개요
이 프로젝트는 **Xerces-C++** 와 **spdlog** 를 사용하여 XML 파일을 XSD 스키마로 검증하는 예제입니다.

`CMakeLists.txt` 는 다음 환경을 지원하도록 작성되어 있습니다. fileciteturn0file0L1-L6

- **Windows / MSVC (Visual Studio + vcpkg, `x64-windows-static`)**
- **Windows / MinGW-w64 (Qt / g++ + vcpkg, `x64-mingw-static`)**
- **Linux (Rocky / Ubuntu, `pkg-config` 기반 시스템 패키지)**

## 주요 기능
- Xerces-C++ 기반 XML 검증
- 커스텀 SAX2 Error Handler
- spdlog 로깅 지원
- MSVC / MinGW / Linux 크로스 플랫폼 CMake 구성

## 파일 구성
- `main.cpp` – 프로그램 시작점 fileciteturn0file1L1-L18  
- `xml_validator_app.*` – 검증 실행 컨트롤러 fileciteturn0file5L1-L112  
- `simple_error_handler.*` – SAX2 에러 핸들러 fileciteturn0file3L1-L109  
- `person_int_schema.xsd` – 샘플 스키마  
- `person_invalid.xml` – 오류가 포함된 테스트 XML fileciteturn0file2L1-L6  

---

## 1. 빌드 환경 준비

### 1.1 Windows – Visual Studio (MSVC) + vcpkg

CMake 스크립트는 vcpkg 가 `%VCPKG_ROOT%` 또는 `%USERPROFILE%\vcpkg` 에 설치되어 있다고 가정하며, **`x64-windows-static`** 트리플릿을 사용합니다. fileciteturn0file0L7-L32

1. vcpkg 클론 및 부트스트랩(PowerShell):
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git "$env:USERPROFILE/vcpkg"
   cd "$env:USERPROFILE/vcpkg"
   .ootstrap-vcpkg.bat
   ```

2. 의존 라이브러리 설치:
   ```powershell
   .cpkg.exe install xerces-c:x64-windows-static spdlog:x64-windows-static
   ```

3. (선택) 유저 전체 통합:
   ```powershell
   .cpkg.exe integrate install
   ```

4. CMake 구성 및 빌드 (프로젝트 루트에서 실행):
   ```powershell
   cmake -B build -G "Visual Studio 17 2022" -A x64 ^
         -DCMAKE_TOOLCHAIN_FILE="$env:USERPROFILE/vcpkg/scripts/buildsystems/vcpkg.cmake"
   cmake --build build --config Debug
   ```

Visual Studio 에서 CMake 프로젝트를 직접 열어 사용하는 경우, **CMakePresets.json** 또는 IDE 설정에서 `CMAKE_TOOLCHAIN_FILE` 과 `VCPKG_TARGET_TRIPLET` 을 지정하면 `CMakeLists.txt` 의 MSVC 분기가 자동으로 적용됩니다.

### 1.2 Windows – MinGW-w64 (Qt / g++) + vcpkg

MinGW 환경에서는 `x64-mingw-static` 트리플릿을 사용하며, 역시 `%VCPKG_ROOT%` 또는 `%USERPROFILE%\vcpkg` 를 기본 경로로 사용합니다. fileciteturn0file0L34-L58

1. vcpkg 클론 및 부트스트랩:
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git "$env:USERPROFILE/vcpkg"
   cd "$env:USERPROFILE/vcpkg"
   .ootstrap-vcpkg.bat
   ```

2. MinGW 전용 트리플릿 라이브러리 설치:
   ```powershell
   .cpkg.exe install xerces-c:x64-mingw-static spdlog:x64-mingw-static
   ```

3. MinGW / Qt CMake 를 사용한 구성 및 빌드 (예: Qt MinGW Kit):
   ```bash
   cmake -B build-mingw -G "MinGW Makefiles" ^
         -DCMAKE_TOOLCHAIN_FILE=%USERPROFILE%/vcpkg/scripts/buildsystems/vcpkg.cmake ^
         -DVCPKG_TARGET_TRIPLET=x64-mingw-static
   cmake --build build-mingw
   ```

Qt Creator 에서는 MinGW Kit 을 선택한 뒤 위와 동일한 `CMAKE_TOOLCHAIN_FILE`, `VCPKG_TARGET_TRIPLET` 을 설정하면 `CMakeLists.txt` 의 MinGW 분기가 자동으로 적용됩니다.

### 1.3 Linux – Rocky Linux / Ubuntu

리눅스에서는 vcpkg 를 사용하지 않고, `pkg-config` 를 통해 시스템에 설치된 `xerces-c`, `spdlog` 패키지를 찾습니다. fileciteturn0file0L60-L86

#### 1.3.1 Rocky Linux (RHEL 계열)

다음 패키지를 설치합니다(root 또는 sudo):

```bash
sudo dnf install -y     xerces-c-devel     spdlog spdlog-devel     pkgconfig gcc-c++ make
```

이후, 프로젝트 루트에서:

```bash
cmake -B build
cmake --build build
```

#### 1.3.2 Ubuntu / Debian 계열

다음 패키지를 설치합니다:

```bash
sudo apt update
sudo apt install -y     libxerces-c-dev     libspdlog-dev     pkg-config g++ make
```

이후, 프로젝트 루트에서:

```bash
cmake -B build
cmake --build build
```

---

## 2. 공통 빌드 절차

필요한 라이브러리가 설치되고 CMake 가 Xerces-C++ / spdlog 를 찾을 수 있으면, 공통 빌드 절차는 다음과 같습니다.

```bash
cmake -B build
cmake --build build
```

---

## 3. 동작 방식

1. 프로젝트 루트 기준 XML / XSD 경로를 설정합니다. fileciteturn0file1L4-L18  
2. Xerces-C++ 를 프로세스 전체에서 1회 초기화합니다. fileciteturn0file5L23-L40  
3. SAX2 파서에 스키마 검증 옵션과 XSD 위치를 설정합니다.  
4. `simple_error_handler` 를 통해 검증 중 발생하는 에러 메시지를 수집합니다. fileciteturn0file3L41-L108  
5. 결과를 `xml_validator_result` 열거형 값(프로세스 종료 코드)으로 반환합니다. fileciteturn0file6L9-L24  

