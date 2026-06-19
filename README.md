# Cupboards Game Engine

Read in: [English](#-english) | [Русский](#-русский)

---

## English

C++17 board game engine with **SFML 2.6.x** graphics and **Dear ImGui** UI.
Data-driven levels (JSON), BFS pathfinding, fixed 60 Hz timestep, and state-machine screen system.

### Features

* **Data-driven levels**: Pure JSON, no hardcoded puzzles
* **BFS pathfinding**: Occupied-node-aware shortest path
* **Fixed timestep**: 60 Hz logic loop with tween animations
* **State machine**: Menu → Level Select → Game → Win
* **Audio**: Streaming music + SFX with graceful degradation
* **Undo (Ctrl+Z)**: Command-pattern move history
* **Debug overlay**: F1 toggles FPS, BFS, and animation state

---

### Build

#### Windows (Visual Studio)

```powershell
cmake -G "Visual Studio 16 2019" -A x64 -B build .
cmake --build build --config Release
.\build\game\Release\cupboards-game.exe
```

#### Ubuntu / WSL

```bash
sudo apt-get install libsfml-dev build-essential cmake
cmake -B build_wsl -DCMAKE_BUILD_TYPE=Release
cmake --build build_wsl -j$(nproc)
./build_wsl/game/cupboards-game
```

Dependencies are fetched automatically via `FetchContent` (SFML, ImGui, nlohmann/json, Catch2).

---

### Build Options

| Option | Default | Description |
|---|---|---|
| `BUILD_GAME` | `ON` | Build the game executable |
| `USE_SYSTEM_SFML` | Linux ON / Win OFF | Use system SFML |
| `USE_SYSTEM_NLOHMANN_JSON` | Linux ON / Win OFF | Use system nlohmann/json |
| `USE_SYSTEM_CATCH2` | Linux ON / Win OFF | Use system Catch2 |

```bash
cmake -B build -DBUILD_GAME=OFF   # core + tests only, no ImGui
```

---

### Controls

| Action | Input |
|---|---|
| Select a piece | Left-click piece |
| Move a piece | Left-click highlighted node |
| Undo last move | Ctrl+Z |
| Toggle debug overlay | F1 |
| Return to menu | Menu button in Game/Win screen |

---

### Level Format

JSON files in `game/assets/levels/`. Detected automatically at startup.

```json
{
  "name": "Classic 3x3",
  "piece_count": 6,
  "nodes": [
    { "id": 1, "x": 100, "y": 100 },
    { "id": 2, "x": 200, "y": 100 }
  ],
  "edges": [
    { "a": 1, "b": 2 }
  ],
  "start_positions": [1, 2],
  "win_positions": [2, 1],
  "piece_colors": ["#FF5733", "#33FF57"],
  "bg_music": "music_game.ogg"
}
```

Add a `.json` file to `game/assets/levels/` — the engine discovers it without recompilation.

---

### Tests

```powershell
cd build
ctest -C Release --output-on-failure
```

---

### Project Structure

```
cupboards/
├── CMakeLists.txt            # Root: FetchContent + subdirs
├── core/                     # cupboards-core (STATIC)
│   ├── include/core/         # Graph, PathFinder, LevelParser, GameState
│   └── src/
├── game/                     # cupboards-game (EXE)
│   ├── src/                  # Application, screens, tweens, audio
│   └── assets/levels/        # JSON puzzles
└── tests/                    # Catch2 + GTest unit/integration tests
```

[⬆ Back to top](#cupboards-game-engine)

---

## Русский

Настольная игра-головоломка на C++17 с графикой **SFML 2.6.x** и UI на **Dear ImGui**.
Уровни в JSON, BFS-поиск пути, фиксированный 60-герцовый такт и стейт-машина экранов.

### Возможности

* **Уровни из JSON**: Никакого хардкода
* **BFS-поиск**: С учётом занятых узлов
* **Fixed timestep**: 60 Hz с Tween-анимациями
* **Стейт-машина**: Меню → Выбор уровня → Игра → Победа
* **Аудио**: Фоновая музыка + звуки, graceful degradation
* **Отмена (Ctrl+Z)**: История ходов через Command pattern
* **Отладка**: F1 для FPS, BFS и состояния анимации

---

### Сборка

#### Windows (Visual Studio)

```powershell
cmake -G "Visual Studio 16 2019" -A x64 -B build .
cmake --build build --config Release
.\build\game\Release\cupboards-game.exe
```

#### Ubuntu / WSL

```bash
sudo apt-get install libsfml-dev build-essential cmake
cmake -B build_wsl -DCMAKE_BUILD_TYPE=Release
cmake --build build_wsl -j$(nproc)
./build_wsl/game/cupboards-game
```

Зависимости подтягиваются автоматически через `FetchContent`.

---

### Параметры сборки

| Параметр | По умолч. | Описание |
|---|---|---|
| `BUILD_GAME` | `ON` | Сборка исполняемого файла игры |
| `USE_SYSTEM_SFML` | Linux ON / Win OFF | Системный SFML |
| `USE_SYSTEM_NLOHMANN_JSON` | Linux ON / Win OFF | Системный nlohmann/json |
| `USE_SYSTEM_CATCH2` | Linux ON / Win OFF | Системный Catch2 |

---

### Управление

| Действие | Управление |
|---|---|
| Выбрать фишку | ЛКМ на фишку |
| Переместить фишку | ЛКМ на подсвеченный узел |
| Отменить ход | Ctrl+Z |
| Отладка | F1 |
| Вернуться в меню | Кнопка «Menu» |

---

### Формат уровня

JSON-файлы в `game/assets/levels/`. Обнаруживаются при запуске.

```json
{
  "name": "Classic 3x3",
  "piece_count": 6,
  "nodes": [
    { "id": 1, "x": 100, "y": 100 }
  ],
  "edges": [
    { "a": 1, "b": 2 }
  ],
  "start_positions": [1, 2],
  "win_positions": [2, 1]
}
```

Положите `.json` в `game/assets/levels/` — движок подхватит без перекомпиляции.

---

### Тесты

```powershell
cd build
ctest -C Release --output-on-failure
```

---

### Структура проекта

```
cupboards/
├── CMakeLists.txt            # Корневой: FetchContent + подкаталоги
├── core/                     # cupboards-core (STATIC)
│   ├── include/core/         # Graph, PathFinder, LevelParser, GameState
│   └── src/
├── game/                     # cupboards-game (EXE)
│   ├── src/                  # Application, экраны, анимации, аудио
│   └── assets/levels/        # JSON-головоломки
└── tests/                    # Catch2 + GTest тесты
```

[⬆ Наверх](#cupboards-game-engine)
