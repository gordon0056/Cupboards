add_library(cupboards-core STATIC
    src/Graph.cpp
    src/PathFinder.cpp
    src/LevelParser.cpp
    src/GameState.cpp
    src/Logger.cpp
)

target_include_directories(cupboards-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cupboards-core
    PUBLIC
        nlohmann_json::nlohmann_json
)
