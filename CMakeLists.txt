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

# ============================================================
# Tests
# ============================================================
enable_testing()

if(NOT BUILD_GAME)
    add_library(cupboards-game-logic STATIC
        game/src/TweenManager.cpp
        game/src/MoveCommand.cpp
        game/src/AudioManager.cpp
        game/src/SaveManager.cpp
    )
    target_include_directories(cupboards-game-logic PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/game/src
    )
    target_link_libraries(cupboards-game-logic PUBLIC
        cupboards-core
        sfml-system
        sfml-audio
        sfml-graphics
    )
    set(GAME_LOGIC_LIB cupboards-game-logic)
else()
    set(GAME_LOGIC_LIB cupboards-core)
    add_library(cupboards-game-logic-objs OBJECT
        game/src/TweenManager.cpp
        game/src/MoveCommand.cpp
        game/src/AudioManager.cpp
        game/src/SaveManager.cpp
    )
    target_include_directories(cupboards-game-logic-objs PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/game/src
    )
    target_link_libraries(cupboards-game-logic-objs PUBLIC
        cupboards-core
        sfml-system
        sfml-audio
        sfml-graphics
    )
endif()

add_executable(core_tests
    tests/unit/test_main.cpp
    tests/unit/test_graph.cpp
    tests/unit/test_levelparser.cpp
    tests/unit/test_pathfinder.cpp
    tests/unit/test_gamestate.cpp
    tests/unit/test_tween.cpp
    tests/unit/test_movecommand.cpp
    tests/unit/test_levelparser_errors.cpp
    tests/unit/test_audio.cpp
    tests/unit/test_savemanager.cpp
    tests/unit/test_localization.cpp
)

if(TARGET Catch2::Catch2WithMain)
    target_link_libraries(core_tests PRIVATE ${GAME_LOGIC_LIB} Catch2::Catch2WithMain)
else()
    target_link_libraries(core_tests PRIVATE ${GAME_LOGIC_LIB} Catch2::Catch2)
endif()

if(BUILD_GAME AND TARGET cupboards-game-logic-objs)
    target_link_libraries(core_tests PRIVATE cupboards-game-logic-objs)
endif()

target_include_directories(core_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/game/src
)

include(Catch)
catch_discover_tests(core_tests)

add_executable(core_tests_gtest
    tests/unit/test_gtest_main.cpp
    tests/unit/test_graph_gtest.cpp
    tests/unit/test_levelparser_gtest.cpp
    tests/unit/test_pathfinder_gtest.cpp
    tests/unit/test_gamestate_gtest.cpp
)

target_link_libraries(core_tests_gtest PRIVATE
    cupboards-core
    GTest::gtest
    GTest::gtest_main
)

target_include_directories(core_tests_gtest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
)

include(GoogleTest)
gtest_discover_tests(core_tests_gtest)

# ============================================================
# Install rules
# ============================================================
if(BUILD_GAME)
    install(TARGETS cupboards-game
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION bin
    )

    install(DIRECTORY game/assets/
        DESTINATION bin/assets
    )

    install(FILES
        ${CMAKE_SOURCE_DIR}/README.md
        DESTINATION bin
    )
endif()

# ============================================================
# CPack packaging
# ============================================================
set(CPACK_PACKAGE_NAME "Cupboards")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "Cupboards Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++17 board game engine with SFML graphics")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "Cupboards")
    set(CPACK_NSIS_PACKAGE_NAME "Cupboards ${PROJECT_VERSION}")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_CREATE_ICONS_ON_DESKTOP ON)
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cupboards Team")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "C++17 board game puzzle")
    set(CPACK_DEBIAN_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Linux.deb")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)
endif()

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    "/build/"
    "/build_linux/"
    "/.git/"
    "/.vs/"
    "/out/"
    "/${CMAKE_CURRENT_BINARY_DIR}/"
    "\\\\.zip$"
    "\\\\.tar\\\\.gz$"
    "\\\\.exe$"
    "\\\\.pdb$"
    "/imgui\\\\.ini$"
    "[\\/]\\\\.gitignore$"
)

include(CPack)

if(BUILD_GAME)
    add_executable(integration_tests
        tests/integration/test_game_loop_headless.cpp
        tests/integration/test_tween_input_lock.cpp
        game/src/TweenManager.cpp
    )

    target_link_libraries(integration_tests PRIVATE
        cupboards-core
        GTest::gtest
        GTest::gtest_main
        sfml-system
    )

    target_include_directories(integration_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/game/src
    )

    gtest_discover_tests(integration_tests)
endif()
