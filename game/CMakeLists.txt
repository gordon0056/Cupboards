add_executable(cupboards-game
    src/main.cpp
    src/Application.cpp
    src/TweenManager.cpp
    src/MoveCommand.cpp
    src/GraphRenderer.cpp
    src/AudioManager.cpp
    src/SafeImGui.cpp
    src/AssetValidator.cpp
    src/SaveManager.cpp
    src/screens/MainMenuScreen.cpp
    src/screens/MenuScreen.cpp
    src/screens/GameScreen.cpp
    src/screens/WinScreen.cpp
    src/screens/SettingsScreen.cpp
)

target_include_directories(cupboards-game
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cupboards-game
    PRIVATE
        cupboards-core
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        ImGui-SFML::ImGui-SFML
)

if(WIN32)
    deploy_sfml_dlls(cupboards-game)
    add_custom_command(TARGET cupboards-game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying assets to output directory..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:cupboards-game>/assets
        COMMENT "Assets copied"
    )
else()
    add_custom_command(TARGET cupboards-game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying assets to game binary directory..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            ${CMAKE_CURRENT_BINARY_DIR}/assets
        COMMENT "Assets copied"
    )
endif()
