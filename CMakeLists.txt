cmake_minimum_required(VERSION 3.20)
project(xe_xerces_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#############################################
# 1) MSVC (Visual Studio)
#############################################
if (MSVC)
    # vcpkg x64-windows-static uses /MT(/MTd) CRT
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # Automatically detect vcpkg location
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

    # ⚠ Do not use target_compile_options here
    #   => Add only as global options
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

    # On GCC 8.x (default 8.5 on Rocky Linux), link stdc++fs for std::filesystem
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
            set(FILESYSTEM_LIB stdc++fs)
        endif()
    endif()
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
    PRIVATE ${FILESYSTEM_LIB}
)

# Provide CMakeLists.txt location to source files such as main.cpp
target_compile_definitions(xe_xerces_example PRIVATE
    CMAKE_SOURCE_DIR_PATH="${CMAKE_CURRENT_LIST_DIR}"
)
