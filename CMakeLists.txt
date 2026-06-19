cmake_minimum_required(VERSION 3.20)
project(Cupboards VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# ============================================================
# Platform-specific dependency setup
# ============================================================
option(USE_SYSTEM_SFML "Use system-installed SFML (Linux)" OFF)
option(USE_SYSTEM_NLOHMANN_JSON "Use system nlohmann/json" OFF)
option(USE_SYSTEM_CATCH2 "Use system Catch2" OFF)

if(UNIX AND NOT APPLE)
    set(USE_SYSTEM_SFML ON)
    set(USE_SYSTEM_NLOHMANN_JSON ON)
    set(USE_SYSTEM_CATCH2 ON)
endif()

# SFML
if(USE_SYSTEM_SFML)
    find_package(SFML 2.6 COMPONENTS system window graphics audio REQUIRED)
else()
    include(FetchContent)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(SFML_STATIC_LIBRARIES TRUE)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.2
        GIT_SHALLOW    ON
    )
    FetchContent_MakeAvailable(SFML)
endif()

# nlohmann/json
if(USE_SYSTEM_NLOHMANN_JSON)
    find_package(nlohmann_json 3.2 QUIET)
    if(NOT nlohmann_json_FOUND)
        find_package(nlohmann_json3 3.2 QUIET)
    endif()
    if(NOT nlohmann_json_FOUND AND NOT nlohmann_json3_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG        v3.11.3
            GIT_SHALLOW    ON
        )
        set(JSON_BuildTests OFF CACHE INTERNAL "")
        FetchContent_MakeAvailable(nlohmann_json)
    endif()
else()
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
        GIT_SHALLOW    ON
    )
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Catch2
if(USE_SYSTEM_CATCH2)
    find_package(Catch2 3 QUIET)
    if(NOT Catch2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.7.1
            GIT_SHALLOW    ON
        )
        FetchContent_MakeAvailable(Catch2)
    endif()
else()
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.7.1
        GIT_SHALLOW    ON
    )
    FetchContent_MakeAvailable(Catch2)
endif()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    GIT_SHALLOW    ON
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ============================================================
# Core library
# ============================================================
add_subdirectory(core)

# ============================================================
# Game executable
# ============================================================
option(BUILD_GAME "Build the game executable" ON)
if(BUILD_GAME)
    include(FetchContent)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.0
        GIT_SHALLOW    ON
    )
    FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/eliasdaler/imgui-sfml.git
        GIT_TAG        v2.6
        GIT_SHALLOW    ON
    )
    FetchContent_MakeAvailable(imgui)
    set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "" FORCE)
    set(IMGUI_SFML_FIND_SFML ON)
    set(IMGUI_SFML_IMGUI_DEMO OFF)

    if(UNIX AND NOT APPLE)
        set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
    else()
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    endif()

    if(NOT UNIX)
        set(IMGUI_SFML_FIND_SFML OFF)
    endif()

    FetchContent_MakeAvailable(imgui-sfml)
    add_subdirectory(game)
endif()
