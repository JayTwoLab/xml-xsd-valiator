cmake_minimum_required(VERSION 3.20)
project(xe_xerces_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#############################################
# 1) MSVC (Visual Studio)
#############################################
if (MSVC)
    # vcpkg x64-windows-static 은 /MT(/MTd) CRT 사용
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # vcpkg 위치 자동 탐색
    set(VCPKG_ROOT_HINT "$ENV{VCPKG_ROOT}")
    if(NOT VCPKG_ROOT_HINT OR VCPKG_ROOT_HINT STREQUAL "")
        set(VCPKG_ROOT_HINT "$ENV{USERPROFILE}/vcpkg")
    endif()

    set(VCPKG_TRIPLET "x64-windows-static")
    set(VCPKG_INSTALL_PREFIX "${VCPKG_ROOT_HINT}/installed/${VCPKG_TRIPLET}")

    list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALL_PREFIX}")

    message(STATUS "[MSVC] Using vcpkg prefix = ${VCPKG_INSTALL_PREFIX}")

    find_package(XercesC CONFIG REQUIRED)
    find_package(spdlog  CONFIG REQUIRED)

#############################################
# 2) MinGW (Windows, g++)
#############################################
elseif (MINGW)
    # vcpkg MinGW triplet
    set(VCPKG_ROOT_HINT "$ENV{VCPKG_ROOT}")
    if(NOT VCPKG_ROOT_HINT OR VCPKG_ROOT_HINT STREQUAL "")
        set(VCPKG_ROOT_HINT "$ENV{USERPROFILE}/vcpkg")
    endif()

    set(VCPKG_TRIPLET "x64-mingw-static")
    set(VCPKG_INSTALL_PREFIX "${VCPKG_ROOT_HINT}/installed/${VCPKG_TRIPLET}")

    list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALL_PREFIX}")

    message(STATUS "[MinGW] Using vcpkg prefix = ${VCPKG_INSTALL_PREFIX}")

    find_package(XercesC CONFIG REQUIRED)
    find_package(spdlog  CONFIG REQUIRED)

    # ⚠ 여기서는 target_compile_options 사용 X
    #   => 전역 옵션으로만 추가
    add_compile_options(-Wall -Wextra -Wpedantic)

#############################################
# 3) Linux (gcc/clang)
#############################################
else()
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(XERCESC REQUIRED xerces-c)
    pkg_check_modules(SPDLOG  REQUIRED spdlog)

    add_library(XercesC::XercesC INTERFACE IMPORTED)
    target_include_directories(XercesC::XercesC INTERFACE ${XERCESC_INCLUDE_DIRS})
    target_link_libraries(XercesC::XercesC INTERFACE ${XERCESC_LIBRARIES})
    target_compile_definitions(XercesC::XercesC INTERFACE ${XERCESC_CFLAGS_OTHER})

    add_library(spdlog::spdlog INTERFACE IMPORTED)
    target_include_directories(spdlog::spdlog INTERFACE ${SPDLOG_INCLUDE_DIRS})
    target_link_libraries(spdlog::spdlog INTERFACE ${SPDLOG_LIBRARIES})
    target_compile_definitions(spdlog::spdlog INTERFACE ${SPDLOG_CFLAGS_OTHER})
endif()

#############################################
# 4) Executable
#############################################
add_executable(xe_xerces_example
    main.cpp
    simple_error_handler.cpp
    xml_validator_app.cpp
    .gitignore
)

target_link_libraries(xe_xerces_example
    PRIVATE XercesC::XercesC
    PRIVATE spdlog::spdlog
)

# main.cpp 등에서 CMakeLists.txt 위치를 알 수 있도록 전달
target_compile_definitions(xe_xerces_example PRIVATE
    CMAKE_SOURCE_DIR_PATH="${CMAKE_CURRENT_LIST_DIR}"
)
